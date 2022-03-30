/*
    Copyright (C) 2020, The AROS Development Team. All rights reserved.
*/


#include <proto/exec.h>
#include <stdlib.h>

#include "shimsinit.h"
#include "internal.h"

static BOOL inited = FALSE;
struct ShimsBase SB;

APTR __get_sysbase();

void __shims_init_internals()
{
    char *t;

    if (inited)
        return;

    SB.sysBase = (struct ExecBase *)__get_sysbase();

    if (!SB.sysBase)
        return;

    SB.dosBase = OpenLibrary("dos.library", 0L);

    if (!SB.dosBase)
        return;

    SB.sb_debugpath = FALSE;
    if (getenv("AXRT_DEBUG_PATH") != NULL)
        SB.sb_debugpath = TRUE;

    /* This field is already set to an initial value in __set_runtime_env */
    /* SB.sb_EnhPathMode = FALSE; */
    if ((t = getenv("AXRT_ENHANCED_PATH_MODE")) != NULL)
    {
        int v = atoi(t);
        if (v == 1)
            SB.sb_EnhPathMode = TRUE;
        else if (v == 0)
            SB.sb_EnhPathMode = FALSE;
        /* else do nothing */
    }

    inited = TRUE;
}

VOID __shims_libc_init_jumptables();

VOID __shims_init_jumptables()
{
    __shims_libc_init_jumptables();
}
