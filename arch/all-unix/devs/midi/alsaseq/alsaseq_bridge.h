/*
    Copyright (C) 2026, The AROS Development Team. All rights reserved.

    *** STUDY SKETCH -- not wired into the build, not compile-tested. ***

    alsaseq_bridge.h - the boundary between AROS/CAMD code and host libasound.

    Rationale for a bridge layer (same split AHI's ALSA driver uses):
    the host header <alsa/asoundlib.h> drags in Linux/glibc types (sigset_t,
    snd_seq_event_t, ...) that clash with AROS's own exec/dos headers if you
    try to include both in one file. So NOTHING in this header exposes an ALSA
    type. The CAMD driver (alsaseq.c) includes ONLY this header and speaks in
    plain integers and byte buffers; alsaseq_bridge.c is the ONLY file that
    includes <alsa/asoundlib.h> and touches the hostlib.

    The bridge is deliberately thin: open a duplex snd_seq client with N ports,
    hand back the client's input poll fd (so the AROS side can wait on it via
    the unixio.hidd), and translate raw-MIDI-byte <-> snd_seq_event with the
    libasound snd_midi_event coders. See NOTES in alsaseq_bridge.c.
*/

#ifndef ALSASEQ_BRIDGE_H
#define ALSASEQ_BRIDGE_H

#include <exec/types.h>

/* Opaque handle to the whole ALSA-seq client (all ports live under it). */
typedef APTR ALSASEQ;

/*
    Open libasound via hostlib, create one snd_seq client in DUPLEX mode with
    `nports` ports (each port readable+writable+subscribable), and one
    snd_midi_event coder per port.

    MUST be called from the context that is allowed to spawn host threads
    (libasound-over-PipeWire starts helper pthreads on open) - i.e. from inside
    the I/O slave process, wrapped in the signal-mask hack (see .c). Returns
    NULL on failure.
*/
ALSASEQ ALSASeq_Open(const char *clientname, LONG nports);

/* Tear everything down (drain, delete ports, close client, close hostlib). */
void    ALSASeq_Close(ALSASEQ h);

/*
    The host file descriptor libasound wants us to poll for *input* readiness.
    We hand this straight to unixio.hidd's AddInterrupt so the slave wakes when
    MIDI arrives, instead of spinning. (snd_seq exposes exactly one input poll
    fd for the client; all ports share it.)
*/
/*
    Fill `fds` with the host file descriptors libasound wants us to poll for
    *input* readiness, up to `max`; returns how many were written. snd_seq can
    expose more than one (e.g. over PipeWire), so the driver must register a
    UnixIO interrupt on each, not just the first.
*/
int     ALSASeq_InputFDs(ALSASEQ h, int *fds, int max);

/*
    Push one raw MIDI byte destined for CAMD port `port` toward ALSA.
    The byte is fed to that port's snd_midi_event *encoder*; when a complete
    message (or a full SysEx) has accumulated, the encoder yields a
    snd_seq_event which we output on that port with DIRECT (immediate)
    dispatch - CAMD gives us no usable timestamp, so we never schedule.
    Call ALSASeq_Flush() once after a burst to actually drain to the kernel.
*/
void    ALSASeq_PutByte(ALSASEQ h, LONG port, UBYTE byte);

/* snd_seq_drain_output: flush queued events to the host sequencer. */
void    ALSASeq_Flush(ALSASEQ h);

/*
    Drain all pending input events from ALSA. For each event, decode it back to
    raw MIDI bytes and hand each byte to the supplied callback together with the
    CAMD port it arrived on (derived from the event's destination seq port).
    Returns when no more input is currently available (non-blocking).

    `deliver` is the CAMD receive path, wrapped by the driver:
        deliver(port, byte, arg)
*/
void    ALSASeq_PumpInput(ALSASEQ h,
                          void (*deliver)(LONG port, UBYTE byte, APTR arg),
                          APTR arg);

#endif /* ALSASEQ_BRIDGE_H */
