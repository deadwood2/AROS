/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$

    Desc: autoinit library - handle program started from workbench.
*/
#include <aros/symbolsets.h>
#include <aros/startup.h>
#include <proto/autoinit.h>

struct WBStartup *WBenchMsg;

int __nowbsupport __attribute__((weak)) = 0;

static void __startup_fromwb(struct ExecBase *SysBase)
{
    BPTR curdir = BNULL;

    FromWBInit(&WBenchMsg, &curdir, &__argv, &__argc);

    __startup_entries_next();

    FromWBExit(WBenchMsg, curdir);
}

ADD2SET(__startup_fromwb, PROGRAM_ENTRIES, -50);
