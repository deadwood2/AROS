/*
    Copyright (C) 2026, The AROS Development Team. All rights reserved.

    *** STUDY SKETCH -- not wired into the build, not compile-tested. ***

    alsaseq - a hosted-AROS CAMD MIDI driver that exposes each CAMD port as an
    ALSA sequencer (snd_seq) port, so AROS MIDI apps appear on the host's MIDI
    graph (visible to aconnect/qjackctl, connectable to fluidsynth, DAWs, HW).

    This file is pure AROS/CAMD - it never sees an ALSA type. All libasound work
    is behind alsaseq_bridge.h and happens on ONE slave process, so a single
    task owns the snd_seq handle.

    Shape and conventions are lifted from the working example
    arch/all-unix/devs/midi/hostmidi.c (LoadSeg module, MDD_Magic, SAVEDS/ASM
    callbacks, Name == filename), and the I/O slave loop is modelled on
    arch/all-unix/devs/networks/eth/iotask.c (unixio.hidd AddInterrupt + a
    combined Wait()).

    ============================================================================
    DATA-FLOW SUMMARY

      OUT (AROS app -> host):
        app PutMidi -> camd.library queues bytes -> calls our ActivateXmit()
        ActivateXmit() just Signal()s the slave (like enabling a TX interrupt)
        slave: drain camd's transmitfunc() byte-by-byte -> ALSASeq_PutByte()
               -> ALSASeq_Flush()

      IN (host -> AROS app):
        host sends events to our seq port -> input fd becomes readable
        unixio irq handler Signal()s the slave
        slave: ALSASeq_PumpInput() -> decode to bytes -> camd's receivefunc()
               per byte -> camd.library parses & distributes to app nodes
    ============================================================================
*/

#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/memory.h>
#include <exec/io.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <devices/timer.h>
#include <midi/camddevices.h>
#include <hidd/unixio.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/oop.h>
#include <libcore/compiler.h>

#include "alsaseq_bridge.h"

#include <aros/debug.h>   /* bug() for the no-timer warning */

#define MAXINFD 8         /* must match the bridge's MAXINFD */

#define NUMPORTS   1               /* how many CAMD ports we publish        */
#define CLIENTNAME "AROS CAMD"

/* signals the slave understands (allocated by the slave for itself) */
struct ExecBase *SysBase;
struct DosLibrary *DOSBase;
struct Library  *OOPBase;
struct Library  *UnixIOBase;
OOP_Object      *unixio;

int main(void) { return -1; }   /* a camd driver must never be run directly */

/* ---- per-port CAMD callback registration (filled by OpenPort) ---------- */

struct PortHooks
{
    ULONG (ASM *transmitfunc)(APTR REG(a2));               /* pull one out byte */
    void  (ASM *receivefunc)(UWORD REG(d0), APTR REG(a2)); /* push one in byte  */
    APTR  userdata;                                        /* camd's driverdata */
    BOOL  open;
};

static struct PortHooks porthooks[NUMPORTS];

/* ---- shared driver state ---------------------------------------------- */

static ALSASEQ          seqclient;
static struct Process  *slave;
static struct Task     *slavetask;
static BYTE             drainsig  = -1;   /* app -> slave: "output waiting"  */
static volatile LONG    slavealive = 0;   /* 0 init, 1 ok, 2 failed          */
static struct uioInterrupt inirq[MAXINFD]; /* one per seq input poll fd      */
static int                 ninirq;         /* how many are registered         */

/* ---- prototypes -------------------------------------------------------- */

BOOL ASM Init(REG(a6) APTR sysbase);
void Expunge(void);
SAVEDS ASM struct MidiPortData *OpenPort(
    REG(a3) struct MidiDeviceData *data, REG(d0) LONG portnum,
    REG(a0) ULONG (* ASM transmitfunc)(APTR REG(a2)),
    REG(a1) void  (* ASM receivefunc)(UWORD REG(d0), APTR REG(a2)),
    REG(a2) APTR userdata);
ASM void ClosePort(REG(a3) struct MidiDeviceData *data, REG(d0) LONG portnum);
SAVEDS ASM void ActivateXmit(REG(a2) APTR userdata, REG(d0) ULONG portnum);

static void IOSlave(void);

/* ---- the resident MidiDeviceData (Name MUST equal the filename) -------- */

static const char version[] =
    "$VER: alsaseq V41.0 (c) 2026 AROS - The AROS Research OS";

const struct MidiDeviceData mididevicedata =
{
    MDD_Magic,
    "alsaseq",              /* == filename, enforced by openmididevice.c     */
    (char *)&version[6],
    41, 0,
    Init, Expunge,
    OpenPort, ClosePort,
    NUMPORTS,
    1                       /* new (byte-stream) format - required on AROS   */
};

static struct MidiPortData midiportdata = { ActivateXmit };

/* ======================================================================= */
/* Init / Expunge                                                          */
/* ======================================================================= */

SAVEDS ASM BOOL Init(REG(a6) APTR sysbase)
{
    SysBase = sysbase;
    DOSBase = (struct DosLibrary *)OpenLibrary("dos.library", 0);
    OOPBase = OpenLibrary("oop.library", 0);
    UnixIOBase = OpenLibrary("unixio.hidd", 0);
    if (!DOSBase || !OOPBase || !UnixIOBase) return FALSE;

    unixio = OOP_NewObject(NULL, CLID_Hidd_UnixIO, NULL);
    if (!unixio) return FALSE;

    /*
        Start the single I/O slave. It opens libasound + snd_seq (so the host
        helper threads are created in the slave's context) and then owns the
        handle for its whole life. We block until it reports alive, exactly like
        AHI's master waits on its slaves.
    */
    slave = CreateNewProcTags(
        NP_Entry,    (IPTR)IOSlave,
        NP_Name,     (IPTR)"alsaseq I/O",
        NP_Priority, 5,
        TAG_END);
    if (!slave) return FALSE;

    while (slavealive == 0) Delay(1);
    if (slavealive != 1) return FALSE;

    return TRUE;
}

void Expunge(void)
{
    if (slave) {
        Signal(slavetask, SIGBREAKF_CTRL_C);   /* ask the slave to exit      */
        while (slavealive != 3) Delay(1);       /* 3 == "dead"                */
    }
    if (unixio)     OOP_DisposeObject(unixio);
    if (UnixIOBase) CloseLibrary(UnixIOBase);
    if (OOPBase)    CloseLibrary(OOPBase);
    if (DOSBase)    CloseLibrary((struct Library *)DOSBase);
}

/* ======================================================================= */
/* OpenPort / ClosePort - just (de)register the camd callbacks             */
/* ======================================================================= */

SAVEDS ASM struct MidiPortData *OpenPort(
    REG(a3) struct MidiDeviceData *data, REG(d0) LONG portnum,
    REG(a0) ULONG (* ASM transmitfunc)(APTR REG(a2)),
    REG(a1) void  (* ASM receivefunc)(UWORD REG(d0), APTR REG(a2)),
    REG(a2) APTR userdata)
{
    if (portnum < 0 || portnum >= NUMPORTS) return NULL;

    porthooks[portnum].transmitfunc = transmitfunc;
    porthooks[portnum].receivefunc  = receivefunc;
    porthooks[portnum].userdata     = userdata;
    porthooks[portnum].open         = TRUE;

    return &midiportdata;
}

ASM void ClosePort(REG(a3) struct MidiDeviceData *data, REG(d0) LONG portnum)
{
    if (portnum < 0 || portnum >= NUMPORTS) return;
    porthooks[portnum].open        = FALSE;
    porthooks[portnum].transmitfunc = NULL;
    porthooks[portnum].receivefunc  = NULL;
}

/* ======================================================================= */
/* ActivateXmit - runs in the APP's task context. Do NO host work here.    */
/* Just poke the slave, the way a real driver would enable its TX irq.     */
/* ======================================================================= */

SAVEDS ASM void ActivateXmit(REG(a2) APTR userdata, REG(d0) ULONG portnum)
{
    if (slavetask && drainsig != -1)
        Signal(slavetask, 1L << drainsig);
}

/* ======================================================================= */
/* The I/O slave: the one task that ever touches libasound.                */
/* ======================================================================= */

/* unixio irq handler: runs at "interrupt" time, only allowed to Signal.   */
static BYTE insig = -1;
static void InputReady(int fd, int mode, void *data)
{
    /* interrupt/host-signal context: only Signal, no host calls */
    if (slavetask && insig != -1)
        Signal(slavetask, 1L << insig);
}

/* the camd receive path, wrapped so the bridge can stay AROS-type-free.   */
static void DeliverByte(LONG port, UBYTE byte, APTR arg)
{
    struct PortHooks *ph = &porthooks[port];
    if (ph->open && ph->receivefunc)
        (*ph->receivefunc)((UWORD)byte, ph->userdata);
}

/* drain every port's outbound queue into ALSA */
static void DrainOutput(void)
{
    LONG p;
    ULONG b;
    BOOL any = FALSE;

    for (p = 0; p < NUMPORTS; p++) {
        struct PortHooks *ph = &porthooks[p];
        if (!ph->open || !ph->transmitfunc) continue;

        /* 0x100 is camd's "nothing more to send" sentinel */
        while ((b = (*ph->transmitfunc)(ph->userdata)) != 0x100) {
            ALSASeq_PutByte(seqclient, p, (UBYTE)b);
            any = TRUE;
        }
    }
    if (any)
        ALSASeq_Flush(seqclient);
}

static void IOSlave(void)
{
    ULONG sigmask, got;
    int   fds[MAXINFD];
    int   n, i;
    struct MsgPort     *timerport = NULL;
    struct timerequest *timerio   = NULL;
    ULONG               timersig  = 0;
    BOOL                timeropen = FALSE;
    BOOL                timerpending = FALSE;

    slavetask = FindTask(NULL);

    insig    = AllocSignal(-1);
    drainsig = AllocSignal(-1);
    if (insig == -1 || drainsig == -1) { slavealive = 2; return; }

    /* open libasound + create the seq client/ports HERE (host threads land
       in this task's context; the sig-mask hack lives inside ALSASeq_Open) */
    seqclient = ALSASeq_Open(CLIENTNAME, NUMPORTS);
    if (!seqclient) { slavealive = 2; return; }

    /* Wake this task whenever ANY of the seq input poll fds is readable.
       snd_seq (esp. over PipeWire) can expose more than one fd, so register
       an interrupt on each - watching only the first silently loses input. */
    n = ALSASeq_InputFDs(seqclient, fds, MAXINFD);
    ninirq = 0;
    for (i = 0; i < n; i++) {
        inirq[ninirq].fd          = fds[i];
        inirq[ninirq].mode        = vHidd_UnixIO_Read;
        inirq[ninirq].handler     = InputReady;
        inirq[ninirq].handlerData = NULL;
        Hidd_UnixIO_AddInterrupt(unixio, &inirq[ninirq]);
        ninirq++;
    }

    /* 10ms periodic poll of the seq input fd. PipeWire's snd_seq poll fd is an
       eventfd, which never raises SIGIO, so the AddInterrupt above may never
       fire. A timer.device tick wakes us to pump input non-blocking (the AROS
       analog of a libdispatch READ source). Wait() yields to the scheduler
       between ticks, so this neither busy-spins nor stalls AROS. */
    timerport = CreateMsgPort();
    if (timerport) {
        timerio = (struct timerequest *)
                  CreateIORequest(timerport, sizeof(struct timerequest));
        if (timerio &&
            OpenDevice("timer.device", UNIT_MICROHZ,
                       (struct IORequest *)timerio, 0) == 0) {
            timeropen = TRUE;
            timersig  = 1L << timerport->mp_SigBit;
            timerio->tr_node.io_Command = TR_ADDREQUEST;
            timerio->tr_time.tv_secs    = 0;
            timerio->tr_time.tv_micro   = 10000;   /* 10 ms */
            SendIO((struct IORequest *)timerio);
            timerpending = TRUE;
        }
    }
    if (!timeropen)
        bug("[alsaseq] WARNING: no timer - input relies on SIGIO only\n");

    slavealive = 1;                              /* tell Init() we're up      */

    sigmask = (1L << insig) | (1L << drainsig) | timersig | SIGBREAKF_CTRL_C;

    for (;;) {
        /* Drain whatever is ready right now (input first, then output),
           mirroring eth/iotask.c's "poll, service, then Wait" structure. */
        ALSASeq_PumpInput(seqclient, DeliverByte, NULL);
        DrainOutput();

        got = Wait(sigmask);

        if ((got & timersig) && timerpending) {  /* re-arm the 10ms tick */
            WaitIO((struct IORequest *)timerio);
            timerio->tr_node.io_Command = TR_ADDREQUEST;
            timerio->tr_time.tv_secs    = 0;
            timerio->tr_time.tv_micro   = 10000;
            SendIO((struct IORequest *)timerio);
        }

        if (got & SIGBREAKF_CTRL_C)
            break;
        /* on insig/timersig -> PumpInput handles input
           on drainsig       -> DrainOutput handles output
           (we service both every wake, so no per-signal branch needed) */
    }

    if (timeropen) {
        if (timerpending) {          /* AbortIO+WaitIO is safe whether or not
                                        the request has already completed */
            AbortIO((struct IORequest *)timerio);
            WaitIO((struct IORequest *)timerio);
        }
        CloseDevice((struct IORequest *)timerio);
    }
    if (timerio)   DeleteIORequest((struct IORequest *)timerio);
    if (timerport) DeleteMsgPort(timerport);

    for (i = 0; i < ninirq; i++)
        Hidd_UnixIO_RemInterrupt(unixio, &inirq[i]);
    ninirq = 0;
    ALSASeq_Close(seqclient);
    seqclient = NULL;

    FreeSignal(insig);
    FreeSignal(drainsig);
    insig = drainsig = -1;

    slavealive = 3;                              /* Expunge() waits for this  */
}
