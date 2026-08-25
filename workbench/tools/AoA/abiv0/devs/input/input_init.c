/*
    Copyright (C) 2024-2026, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/input.h>
#include <aros/debug.h>

#include "../../include/exec/structures.h"
#include "../../include/exec/proxy_structures.h"
#include "../../include/exec/functions.h"
#include "../../include/aros/cpu.h"
#include "../../include/aros/proxy.h"

struct DeviceProxy *abiv0InputBase;

UWORD abiv0_PeekQualifier(struct LibraryV0 *InputBaseV0)
{
    struct Library *InputBase = &(((struct DeviceProxy *)InputBaseV0)->native->dd_Library);
    return PeekQualifier();
}
MAKE_PROXY_ARG_1(PeekQualifier)

void init_input(struct ExecBaseV0 *SysBaseV0)
{
    UWORD negsize, possize, lastlvo;
    APTR tmpmem;

    lastlvo = 7;
    negsize = (lastlvo + 1) * sizeof(struct JumpVecV0);
    possize = sizeof(struct DeviceProxy);
    tmpmem  = abiv0_AllocMem(negsize + possize, MEMF_CLEAR, SysBaseV0);
    abiv0InputBase = (tmpmem + negsize);
    /* Set all LVO addresses to their number so that code jumps to "number" of the LVO and crashes */
    for (int i = 5; i <= lastlvo; i++) __AROS_SETVECADDRV0(abiv0InputBase, i, (APTR32)(IPTR)i + 1150);
    __AROS_SETVECADDRV0(abiv0InputBase,  7, (APTR32)(IPTR)proxy_PeekQualifier);
    abiv0InputBase->type                        = DEVPROXY_TYPE_INPUT;
    abiv0InputBase->base.dd_Library.lib_NegSize = negsize;
    abiv0InputBase->base.dd_Library.lib_PosSize = possize;
}

void exit_input(struct ExecBaseV0 *SysBaseV0)
{
    abiv0_FreeMem((APTR)((IPTR)abiv0InputBase - abiv0InputBase->base.dd_Library.lib_NegSize),
        abiv0InputBase->base.dd_Library.lib_NegSize + abiv0InputBase->base.dd_Library.lib_PosSize, SysBaseV0);
}
