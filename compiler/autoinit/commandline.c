/*
    Copyright (C) 1995-2011, The AROS Development Team. All rights reserved.
*/
#include <aros/symbolsets.h>
#include <aros/startup.h>
#include <proto/autoinit.h>

#define DEBUG 0
#include <aros/debug.h>

int __nocommandline __attribute__((weak)) = 0;

extern char *__argstr;
extern ULONG __argsize;

extern char **__argv;
extern int  __argc;

static void __initcommandline(struct ExecBase *SysBase)
{
    STRPTR __args;
    LONG   __argmax;

    if (!CommandLineInit(__argstr, __argsize, &__args, &__argmax, (STRPTR **)&__argv, &__argc,
            WBenchMsg, __nocommandline))
        return;

    __startup_entries_next();

    CommandLineExit(__argsize, __args, __argmax, (STRPTR *)__argv, WBenchMsg);
}

ADD2SET(__initcommandline, PROGRAM_ENTRIES, -10);
