/*
    Copyright (C) 2026, The AROS Development Team. All rights reserved.

    alsaseq_bridge.c - the ONLY file that includes <alsa/asoundlib.h>.

    It resolves the snd_seq / snd_midi_event / libc symbols it needs through
    hostlib.resource (exactly like workbench/devs/AHI/Drivers/Alsa/alsa-bridge)
    and implements the plain-C entry points declared in alsaseq_bridge.h.

    ----------------------------------------------------------------------------
    KEY DESIGN NOTES (read these first)

    1. snd_seq is an EVENT api; CAMD's driver interface is a raw BYTE STREAM.
       Bridged with libasound's snd_midi_event coder (encode: bytes -> events,
       buffering running-status/SysEx across calls; decode: events -> bytes).

    2. TIMING. CAMD hands the driver no timestamp, so every outbound event uses
       snd_seq_ev_set_direct() - immediate, no queue. Inbound ALSA timestamps
       are dropped (the CAMD receive callback is per-byte, no time field).

    3. THREADING / SIGNAL-MASK HACK. libasound over PulseAudio/PipeWire spawns
       helper pthreads on snd_seq_open(); they inherit AROS's signal mask and
       AROS drives "interrupts" with host signals. Fix (from the AHI driver):
       block all host signals around the thread-spawning open so the new threads
       inherit an all-blocked mask.

    4. SINGLE OWNER. Every function here is meant to run on the one I/O slave
       task in alsaseq.c, so exactly one AROS task ever touches snd_seq_t. The
       handle is put in NON-BLOCKING mode so neither input nor output can stall
       that task inside libasound.
*/

#include <alsa/asoundlib.h>
#include <signal.h>
#include <poll.h>

#include <proto/exec.h>
#include <proto/hostlib.h>
#include <aros/debug.h>

#include "alsaseq_bridge.h"

#define LIBASOUND_SOFILE "libasound.so.2"
#define LIBC_SOFILE      "libc.so.6"

#define MAXPORTS 16
#define MAXINFD  8      /* max snd_seq input poll descriptors we watch */

/* ------------------------------------------------------------------ */
/* hostlib symbol table (subset of libasound we actually use)         */
/* ------------------------------------------------------------------ */

struct seq_func
{
    int  (*snd_seq_open)(snd_seq_t **, const char *, int, int);
    int  (*snd_seq_close)(snd_seq_t *);
    int  (*snd_seq_nonblock)(snd_seq_t *, int);
    int  (*snd_seq_set_client_name)(snd_seq_t *, const char *);
    int  (*snd_seq_create_simple_port)(snd_seq_t *, const char *,
                                       unsigned int, unsigned int);
    int  (*snd_seq_delete_simple_port)(snd_seq_t *, int);
    int  (*snd_seq_poll_descriptors_count)(snd_seq_t *, short);
    int  (*snd_seq_poll_descriptors)(snd_seq_t *, struct pollfd *,
                                     unsigned int, short);
    int  (*snd_seq_event_output_direct)(snd_seq_t *, snd_seq_event_t *);
    int  (*snd_seq_drain_output)(snd_seq_t *);
    int  (*snd_seq_event_input)(snd_seq_t *, snd_seq_event_t **);

    int  (*snd_midi_event_new)(size_t, snd_midi_event_t **);
    void (*snd_midi_event_free)(snd_midi_event_t *);
    void (*snd_midi_event_init)(snd_midi_event_t *);
    void (*snd_midi_event_no_status)(snd_midi_event_t *, int);
    /* encode one byte: >0 => an event was completed into *ev */
    int  (*snd_midi_event_encode_byte)(snd_midi_event_t *, int, snd_seq_event_t *);
    /* decode an event to raw bytes: returns count written */
    long (*snd_midi_event_decode)(snd_midi_event_t *, unsigned char *, long,
                                  const snd_seq_event_t *);
};

struct libc_func
{
    int (*sigfillset)(sigset_t *);
    int (*sigprocmask)(int, const sigset_t *, sigset_t *);
};

static const char *seq_func_names[] =
{
    "snd_seq_open", "snd_seq_close", "snd_seq_nonblock",
    "snd_seq_set_client_name",
    "snd_seq_create_simple_port", "snd_seq_delete_simple_port",
    "snd_seq_poll_descriptors_count",
    "snd_seq_poll_descriptors",
    "snd_seq_event_output_direct", "snd_seq_drain_output",
    "snd_seq_event_input",
    "snd_midi_event_new", "snd_midi_event_free", "snd_midi_event_init",
    "snd_midi_event_no_status", "snd_midi_event_encode_byte",
    "snd_midi_event_decode",
};
static const char *libc_func_names[] = { "sigfillset", "sigprocmask" };

static struct seq_func  seq;
static struct libc_func libc;
static void *libasoundhandle, *libchandle;

/* proto/hostlib.h's inline stubs use this global base symbol. */
APTR HostLibBase;

#define A(f,...)  (seq.f(__VA_ARGS__))     /* call an ALSA symbol */

/* one CAMD port == one snd_seq simple port + its own encode/decode coders */
struct portstate
{
    int                seqport;    /* ALSA port number                     */
    snd_midi_event_t  *enc;        /* raw bytes  -> events (outbound)      */
    snd_midi_event_t  *dec;        /* events -> raw bytes (inbound)        */
};

struct alsaseq_client
{
    snd_seq_t        *seqh;
    LONG              nports;
    struct portstate  port[MAXPORTS];
    int               infd[MAXINFD]; /* all input poll fds (PipeWire may >1) */
    int               ninfd;
};

/* Exactly one client per loaded driver, so a static instance is enough and we
   avoid allocating memory that would have to cross the AROS/host boundary. */
static struct alsaseq_client the_client;

/* ------------------------------------------------------------------ */
/* small helpers                                                      */
/* ------------------------------------------------------------------ */

/* build "<base> N" without dragging a libc/stdio formatter across hostlib */
static void make_portname(char *dst, const char *base, LONG idx)
{
    LONG i = 0;
    while (base[i] && i < 24) { dst[i] = base[i]; i++; }
    dst[i++] = ' ';
    if (idx >= 10) dst[i++] = '0' + (char)(idx / 10);
    dst[i++] = '0' + (char)(idx % 10);
    dst[i]   = '\0';
}

static void *load_so(const char *sofile, const char **names, int n, void **slots)
{
    void *handle; char *err; int i;

    if ((handle = HostLib_Open(sofile, &err)) == NULL) {
        bug("[alsaseq] open %s failed: %s\n", sofile, err);
        return NULL;
    }
    for (i = 0; i < n; i++) {
        slots[i] = HostLib_GetPointer(handle, names[i], &err);
        if (err) {
            bug("[alsaseq] symbol %s: %s\n", names[i], err);
            HostLib_Close(handle, NULL);
            return NULL;
        }
    }
    return handle;
}

static BOOL hostlib_init(void)
{
    HostLibBase = OpenResource("hostlib.resource");
    if (!HostLibBase) return FALSE;

    libasoundhandle = load_so(LIBASOUND_SOFILE, seq_func_names,
                              (int)(sizeof(seq_func_names)/sizeof(seq_func_names[0])),
                              (void **)&seq);
    if (!libasoundhandle) return FALSE;

    /* libc is only for the signal-mask hack; tolerate its absence */
    libchandle = load_so(LIBC_SOFILE, libc_func_names,
                         (int)(sizeof(libc_func_names)/sizeof(libc_func_names[0])),
                         (void **)&libc);
    return TRUE;
}

/* ------------------------------------------------------------------ */
/* public bridge API                                                  */
/* ------------------------------------------------------------------ */

ALSASEQ ALSASeq_Open(const char *clientname, LONG nports)
{
    struct alsaseq_client *c = &the_client;
    sigset_t saved;
    LONG i;
    int rc;

    if (nports > MAXPORTS) nports = MAXPORTS;

    if (!hostlib_init())
        return NULL;

    c->nports = nports;

    /*
        --- signal-mask hack around the thread-spawning call ---
        Block every host signal so any pthread libasound spins up inherits an
        all-blocked mask and can never be picked to run an AROS irq handler.
    */
    if (libc.sigfillset) {
        sigset_t full;
        libc.sigfillset(&full);
        libc.sigprocmask(SIG_SETMASK, &full, &saved);
    }

    rc = A(snd_seq_open, &c->seqh, "default", SND_SEQ_OPEN_DUPLEX, 0);

    if (libc.sigfillset)
        libc.sigprocmask(SIG_SETMASK, &saved, NULL);

    if (rc < 0) {
        bug("[alsaseq] snd_seq_open failed: %d\n", rc);
        return NULL;
    }

    /* never block the owning task inside libasound */
    A(snd_seq_nonblock, c->seqh, 1);
    A(snd_seq_set_client_name, c->seqh, clientname);

    for (i = 0; i < nports; i++) {
        char nm[32];
        make_portname(nm, clientname, i);

        /* bidirectional + subscribable both ways, so host apps can wire to us
           on either side (aconnect / qjackctl / fluidsynth / a DAW) */
        c->port[i].seqport = A(snd_seq_create_simple_port, c->seqh, nm,
            SND_SEQ_PORT_CAP_READ  | SND_SEQ_PORT_CAP_SUBS_READ |
            SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
            SND_SEQ_PORT_TYPE_MIDI_GENERIC | SND_SEQ_PORT_TYPE_APPLICATION);

        A(snd_midi_event_new, 256, &c->port[i].enc);
        A(snd_midi_event_new, 256, &c->port[i].dec);
        A(snd_midi_event_init, c->port[i].enc);
        A(snd_midi_event_init, c->port[i].dec);
        A(snd_midi_event_no_status, c->port[i].dec, 1);  /* full status out */
    }

    /* Cache ALL input poll fds for the AROS side to watch. snd_seq over
       PipeWire can expose more than one - watching only the first misses
       input. (Same reason the reference libdispatch/ObjC code loops over
       snd_seq_poll_descriptors_count.) */
    {
        struct pollfd pfd[MAXINFD];
        int want = A(snd_seq_poll_descriptors_count, c->seqh, POLLIN);
        int got, k;
        if (want > MAXINFD) want = MAXINFD;
        if (want < 1) want = 1;
        got = A(snd_seq_poll_descriptors, c->seqh, pfd, want, POLLIN);
        c->ninfd = 0;
        for (k = 0; k < got && k < MAXINFD; k++)
            c->infd[c->ninfd++] = pfd[k].fd;
    }

    return (ALSASEQ)c;
}

void ALSASeq_Close(ALSASEQ h)
{
    struct alsaseq_client *c = h;
    LONG i;
    if (!c || !c->seqh) return;

    for (i = 0; i < c->nports; i++) {
        if (c->port[i].enc) A(snd_midi_event_free, c->port[i].enc);
        if (c->port[i].dec) A(snd_midi_event_free, c->port[i].dec);
        A(snd_seq_delete_simple_port, c->seqh, c->port[i].seqport);
    }
    A(snd_seq_close, c->seqh);
    c->seqh = NULL;

    if (libasoundhandle) { HostLib_Close(libasoundhandle, NULL); libasoundhandle = NULL; }
    if (libchandle)      { HostLib_Close(libchandle, NULL);      libchandle = NULL; }
}

int ALSASeq_InputFDs(ALSASEQ h, int *fds, int max)
{
    struct alsaseq_client *c = h;
    int k, n = 0;
    for (k = 0; k < c->ninfd && n < max; k++)
        fds[n++] = c->infd[k];
    return n;
}

void ALSASeq_PutByte(ALSASEQ h, LONG port, UBYTE byte)
{
    struct alsaseq_client *c = h;
    snd_seq_event_t ev;

    if (port < 0 || port >= c->nports) return;

    /* feed the byte to the encoder; >0 means a full event popped out */
    if (A(snd_midi_event_encode_byte, c->port[port].enc, byte, &ev) > 0) {
        snd_seq_ev_set_source(&ev, c->port[port].seqport);
        snd_seq_ev_set_subs(&ev);        /* -> whoever subscribed to us   */
        snd_seq_ev_set_direct(&ev);      /* immediate: no queue, no time  */
        A(snd_seq_event_output_direct, c->seqh, &ev);
    }
}

void ALSASeq_Flush(ALSASEQ h)
{
    A(snd_seq_drain_output, ((struct alsaseq_client *)h)->seqh);
}

void ALSASeq_PumpInput(ALSASEQ h,
                       void (*deliver)(LONG port, UBYTE byte, APTR arg),
                       APTR arg)
{
    struct alsaseq_client *c = h;
    snd_seq_event_t *ev;
    unsigned char buf[512];
    LONG p;
    long n, i;

    /* non-blocking handle: snd_seq_event_input returns <0 (-EAGAIN) when the
       input buffer is empty, so this drains what's there and then stops. */
    while (A(snd_seq_event_input, c->seqh, &ev) >= 0) {

        /* which CAMD port did this land on? map seq dest.port -> index */
        for (p = 0; p < c->nports; p++)
            if (c->port[p].seqport == ev->dest.port)
                break;
        if (p >= c->nports)
            continue;

        /* decode the event back into raw MIDI bytes and stream them up */
        n = A(snd_midi_event_decode, c->port[p].dec, buf, sizeof(buf), ev);
        for (i = 0; i < n; i++)
            deliver(p, buf[i], arg);
    }
}
