/*
    Copyright (C) 2024-2026, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/console.h>
#include <devices/inputevent.h>
#include <aros/debug.h>

#include "../../include/exec/structures.h"
#include "../../include/exec/proxy_structures.h"
#include "../../include/exec/functions.h"
#include "../../include/aros/cpu.h"
#include "../../include/aros/proxy.h"
#include "../../include/input/structures.h"

#include "../../support.h"

struct DeviceProxy *abiv0ConsoleBase;

LONG abiv0_RawKeyConvert(struct InputEventV0 *events, STRPTR buffer, LONG length, struct KeyMap * keyMap, struct LibraryV0 *ConsoleBaseV0)
{
    /* Support SDL->CGX_TranslateKey case */
    if (length != 5 || keyMap != NULL)
    {
unhandledCodePath(__func__, "length or keymap", length, (ULONG)(IPTR)keyMap);
        return 0;
    }
    if ((APTR)(IPTR)events->ie_position.ie_addr != NULL)
    {
bug("abiv0_RawKeyConvert: STUB\n");
        return 0;
    }

    struct Library *ConsoleDevice = &(((struct DeviceProxy *)ConsoleBaseV0)->native->dd_Library);
    struct InputEvent eventnative;
    eventnative.ie_Qualifier    = events->ie_Qualifier;
    eventnative.ie_Class        = events->ie_Class;
    eventnative.ie_SubClass     = events->ie_SubClass;
    eventnative.ie_Code         = events->ie_Code;
    eventnative.ie_position.ie_addr = NULL;
    eventnative.ie_NextEvent = NULL; /* RawKeyConvert calls MapRawKey which ignores ie_NextEvent anyhow */
    return RawKeyConvert(&eventnative, buffer, length, NULL);
}
MAKE_PROXY_ARG_5(RawKeyConvert)

void init_console(struct ExecBaseV0 *SysBaseV0)
{
    UWORD negsize, possize, lastlvo;
    APTR tmpmem;

    lastlvo = 12;
    negsize = (lastlvo + 1) * sizeof(struct JumpVecV0);
    possize = sizeof(struct DeviceProxy);
    tmpmem  = abiv0_AllocMem(negsize + possize, MEMF_CLEAR, SysBaseV0);
    abiv0ConsoleBase = (tmpmem + negsize);
    /* Set all LVO addresses to their number so that code jumps to "number" of the LVO and crashes */
    for (int i = 5; i <= lastlvo; i++) __AROS_SETVECADDRV0(abiv0ConsoleBase, i, (APTR32)(IPTR)i + 1200);
    __AROS_SETVECADDRV0(abiv0ConsoleBase,  8, (APTR32)(IPTR)proxy_RawKeyConvert);
    abiv0ConsoleBase->type                        = DEVPROXY_TYPE_CONSOLE;
    abiv0ConsoleBase->base.dd_Library.lib_NegSize = negsize;
    abiv0ConsoleBase->base.dd_Library.lib_PosSize = possize;
}

void exit_console(struct ExecBaseV0 *SysBaseV0)
{
    abiv0_FreeMem((APTR)((IPTR)abiv0ConsoleBase - abiv0ConsoleBase->base.dd_Library.lib_NegSize),
        abiv0ConsoleBase->base.dd_Library.lib_NegSize + abiv0ConsoleBase->base.dd_Library.lib_PosSize, SysBaseV0);
}
