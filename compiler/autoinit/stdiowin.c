/*
    Copyright © 2009-2011, The AROS Development Team. All rights reserved.
    $Id$

    Desc: autoinit library - open IO window when started from WB
*/

#include <proto/autoinit.h>
#include <aros/startup.h>

#include <aros/debug.h>

/* programmers can define the __stdiowin for opening the win that will be used for
   IO to the standard file descriptors.
   If none is provided a default value will be used
*/
extern char const __stdiowin[];

int __nostdiowin __attribute__((weak)) = 0;

static void __startup_stdiowin(struct ExecBase *SysBase)
{
    BPTR handles[6] = { BNULL };

    if (!StdIOWinInit(handles, __stdiowin, WBenchMsg))
        return;

    __startup_entries_next();

    StdIOWinExit(handles, WBenchMsg);
}

ADD2SET(__startup_stdiowin, PROGRAM_ENTRIES, -20);
