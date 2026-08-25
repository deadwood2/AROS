/*
    Copyright (C) 2024-2026, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/timer.h>
#include <aros/debug.h>

#include "../../include/exec/structures.h"
#include "../../include/exec/proxy_structures.h"
#include "../../include/exec/functions.h"
#include "../../include/aros/cpu.h"
#include "../../include/aros/proxy.h"
#include "../../include/timer/structures.h"

struct DeviceProxy *abiv0TimerBase;

void abiv0_GetSysTime(struct timeval *dest, struct LibraryV0 *TimerBaseV0)
{
    GetSysTime(dest);
}
MAKE_PROXY_ARG_2(GetSysTime)

void abiv0_SubTime(struct timeval *dest, struct timeval *src, struct LibraryV0 *TimerBaseV0)
{
    SubTime(dest, src);
}
MAKE_PROXY_ARG_3(SubTime)

void abiv0_AddTime(struct timeval *dest, struct timeval *src, struct LibraryV0 *TimerBaseV0)
{
    AddTime(dest, src);
}
MAKE_PROXY_ARG_3(AddTime)

LONG abiv0_CmpTime(struct timeval *dest, struct timeval *src, struct LibraryV0 *TimerBaseV0)
{
    return CmpTime(dest, src);
}
MAKE_PROXY_ARG_3(CmpTime)

void init_timer(struct ExecBaseV0 *SysBaseV0)
{
    UWORD negsize, possize, lastlvo;
    APTR tmpmem;

    lastlvo = 12;
    negsize = (lastlvo + 1) * sizeof(struct JumpVecV0);
    possize = sizeof(struct DeviceProxy);
    tmpmem  = abiv0_AllocMem(negsize + possize, MEMF_CLEAR, SysBaseV0);
    abiv0TimerBase = (tmpmem + negsize);
        /* Set all LVO addresses to their number so that code jumps to "number" of the LVO and crashes */
    for (int i = 5; i <= lastlvo; i++) __AROS_SETVECADDRV0(abiv0TimerBase, i, (APTR32)(IPTR)i + 1100);
    __AROS_SETVECADDRV0(abiv0TimerBase, 11, (APTR32)(IPTR)proxy_GetSysTime);
    __AROS_SETVECADDRV0(abiv0TimerBase,  8, (APTR32)(IPTR)proxy_SubTime);
    __AROS_SETVECADDRV0(abiv0TimerBase,  7, (APTR32)(IPTR)proxy_AddTime);
    __AROS_SETVECADDRV0(abiv0TimerBase,  9, (APTR32)(IPTR)proxy_CmpTime);
    abiv0TimerBase->type                        = DEVPROXY_TYPE_TIMER;
    abiv0TimerBase->base.dd_Library.lib_NegSize = negsize;
    abiv0TimerBase->base.dd_Library.lib_PosSize = possize;
}

void exit_timer(struct ExecBaseV0 *SysBaseV0)
{
    abiv0_FreeMem((APTR)((IPTR)abiv0TimerBase - abiv0TimerBase->base.dd_Library.lib_NegSize),
        abiv0TimerBase->base.dd_Library.lib_NegSize + abiv0TimerBase->base.dd_Library.lib_PosSize, SysBaseV0);
}
