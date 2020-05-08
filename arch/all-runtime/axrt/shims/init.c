/*
    Copyright © 2020, The AROS Development Team. All rights reserved.
    $Id$
*/


#include <proto/exec.h>

#include "shimsinit.h"
#include "internal.h"

static BOOL inited = FALSE;
struct ShimsBase SB;

APTR __get_sysbase();

void __shims_init_internals()
{
    if (inited)
        return;

    SB.sysBase = (struct ExecBase *)__get_sysbase();

    if (!SB.sysBase)
        return;

    SB.dosBase = OpenLibrary("dos.library", 0L);

    if (!SB.dosBase)
        return;

    inited = TRUE;
}

VOID __shims_libc_init_jumptables();

VOID __shims_init_jumptables()
{
    __shims_libc_init_jumptables();
}
