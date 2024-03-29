/*
    Copyright (C) 2013-2023, The AROS Development Team. All rights reserved
*/

#include <aros/debug.h>

#include <proto/exec.h>

/* We want all other bases obtained from our base */
#define __NOLIBBASE__

#include <proto/timer.h>

#include <exec/types.h>
#include <devices/timer.h>
#include <exec/io.h>

#include "timer.h"
#include "ata.h"

/* Waits for a signal or a timeout */
ULONG ata_WaitTO(struct IORequest* tmr, ULONG secs, ULONG micro, ULONG sigs)
{
    ULONG sig = 1 << tmr->io_Message.mn_ReplyPort->mp_SigBit;
    ULONG tsigs;

    D(
        struct Node *t = (struct Node *)FindTask(NULL)
        bug("[ATA  ] Timed wait %lds %ldu (task='%s')\n", secs, micro, t->ln_Name);
    )

    tmr->io_Command = TR_ADDREQUEST;
    ((struct timerequest*)tmr)->tr_time.tv_secs = secs;
    ((struct timerequest*)tmr)->tr_time.tv_micro = micro;

    tsigs = SetSignal(0, 0);
    SendIO(tmr);
    D(
        bug("[ATA  ] Preset signals: %lx ('%s')\n", SetSignal(0, 0), t->ln_Name);
        bug("[ATA  ] Signals requested: %lx ('%s')\n", sigs, t->ln_Name);
        bug("[ATA  ] Timer signal: %lx ('%s')\n", sig, t->ln_Name);
    )
    sigs = Wait(sigs | sig);
    D(bug("[ATA  ] Signals received: %lx ('%s')\n", sigs, t->ln_Name));
    if (0 == (sigs & sig))
    {
        if (!CheckIO(tmr))
            AbortIO(tmr);
    }
    WaitIO(tmr);

    SetSignal(tsigs, sig | tsigs);

    return sigs & ~sig;
}
