/*
    Copyright (C) 2015-2026, The AROS Development Team. All rights reserved.

    Record slave for the hosted ALSA AHI driver. Mirror of alsa-playslave.c:
    the play slave polls ALSA_Avail for room to WRITE and mixes into the buffer;
    this slave polls ALSA_Avail for frames to READ and hands each buffer up to
    AHI via the record hook (ahiac_SamplerFunc), following the pattern in
    Filesave/filesave-recslave.c.

    Like the play slave, it never blocks in the ALSA call - it polls and only
    reads what is already available - so AHIsub_Stop's kill signal is noticed
    promptly instead of being stuck inside snd_pcm_readi.
*/

#include <aros/debug.h>
#include <config.h>

#include <devices/ahi.h>
#include <exec/execbase.h>
#include <libraries/ahi_sub.h>

#include "DriverData.h"
#include "library.h"

#include "alsa-bridge/alsa.h"

#define dd ((struct AlsaData*) AudioCtrl->ahiac_DriverData)

#define min(a,b) ( (a) < (b) ? (a) : (b) )

/******************************************************************************
** The record slave process **************************************************
******************************************************************************/

#undef SysBase

void RecordSlave( struct ExecBase* SysBase );

#include <aros/asmcall.h>

AROS_UFH3(LONG, RecordSlaveEntry,
      AROS_UFHA(STRPTR, argPtr, A0),
      AROS_UFHA(ULONG, argSize, D0),
      AROS_UFHA(struct ExecBase *, SysBase, A6))
{
   AROS_USERFUNC_INIT
   RecordSlave( SysBase );
   return 0;
   AROS_USERFUNC_EXIT
}

void
RecordSlave( struct ExecBase* SysBase )
{
  struct AHIAudioCtrlDrv*  AudioCtrl;
  struct DriverBase*       AHIsubBase;
  struct AlsaBase*         AlsaBase;   /* needed by the DOSBase macro (Delay) */
  BOOL                     running;
  ULONG                    signals;
  ULONG                    filled = 0; /* frames already accumulated in recordbuffer[cur] */
  int                      cur    = 0; /* which of the two record buffers we fill now     */

  struct AHIRecordMessage  recmsg =
  {
    AHIST_S16S,      /* ahirm_Type   - 16-bit stereo, matches ALSA_SetHWParams */
    NULL,            /* ahirm_Buffer - set per call                           */
    0                /* ahirm_Length - set per call (in sample frames)        */
  };

  Wait(SIGF_SINGLE);

  AudioCtrl  = (struct AHIAudioCtrlDrv*) FindTask(NULL)->tc_UserData;
  AHIsubBase = (struct DriverBase*) dd->ahisubbase;
  AlsaBase   = (struct AlsaBase*) AHIsubBase;   /* for DOSBase, used by Delay() */
  dd->recordslavesignal = AllocSignal( -1 );

  if( dd->recordslavesignal != -1 )
  {
    // Everything set up. Tell Master we're alive and healthy.

    Signal( (struct Task*) dd->mastertask,
            1L << dd->recordmastersignal );

    running = TRUE;

    while( running )
    {
      signals = SetSignal(0L,0L);

      if( signals & ( SIGBREAKF_CTRL_C | (1L << dd->recordslavesignal) ) )
      {
        running = FALSE;
      }
      else
      {
        /* Deliver FIXED-size record blocks of exactly RECORD_BUFFER_SAMPLES
         * frames (like the reference Filesave driver), not variable chunks.
         * Accumulate consecutive reads into recordbuffer until it is full,
         * then fire the sampler hook once. Capture is AHIST_S16S = 4 bytes
         * per frame (stereo 16-bit), matching ALSA_SetHWParams. */
        LONG need  = (LONG)( RECORD_BUFFER_SAMPLES - filled );  /* frames still needed */
        LONG avail = ALSA_Avail( dd->capturehandle );

        if( avail == ALSA_XRUN )
        {
          D(bug("[Alsa] capture ALSA_Avail() == XRUN\n"));
          ALSA_StartCapture( dd->capturehandle );  // re-prepare + re-start
          avail  = 0;
          filled = 0;                              // drop partial block; stay aligned
        }

        if( avail >= 64 )
        {
          LONG toread = ( avail < need ) ? avail : need;   /* only up to what completes the block */
          LONG frames = ALSA_Read( dd->capturehandle,
                                   (UBYTE*)dd->recordbuffer[cur] + filled * 4,
                                   toread );

          if( frames == ALSA_XRUN )
          {
            D(bug("[Alsa] capture ALSA_Read() == XRUN\n"));
            ALSA_StartCapture( dd->capturehandle );
            filled = 0;
          }
          else if( frames > 0 )
          {
            filled += (ULONG)frames;

            if( filled >= RECORD_BUFFER_SAMPLES )   /* full block -> deliver once */
            {
              recmsg.ahirm_Buffer = dd->recordbuffer[cur];
              recmsg.ahirm_Length = RECORD_BUFFER_SAMPLES;   /* FIXED length, in frames */
              CallHookPkt( AudioCtrl->ahiac_SamplerFunc, AudioCtrl, &recmsg );
              filled = 0;
              cur   ^= 1;   /* fill the OTHER buffer next; the master task drains this one */
            }
          }
        }
        else
        {
          Delay( 1 );   // ~20 ms; nothing captured yet, don't busy-spin
        }
      }
    }
  }

  FreeSignal( dd->recordslavesignal );
  dd->recordslavesignal = -1;

  Forbid();

  // Tell the Master we're dying

  Signal( (struct Task*) dd->mastertask, 1L << dd->recordmastersignal );

  dd->recordslavetask = NULL;

  // Multitasking will resume when we are dead.
}
