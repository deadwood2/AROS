/*
    Copyright (C) 2021, The AROS Development Team. All rights reserved.

*/

#include <aros/debug.h>

#include <aros/startup.h>
#include <proto/exec.h>
#include <aros/symbolsets.h>

#include <proto/crt.h>

AROS_MAKE_ASM_SYM(int, dummy, __includecrtprogramstartuphandling, 0);
AROS_EXPORT_ASM_SYM(__includecrtprogramstartuphandling);

static void __program_startup(struct ExecBase *SysBase)
{
    jmp_buf exitjmp;

    if (setjmp(exitjmp) == 0)
    {
        D(bug("%s: setjmp() called\n", __func__));

        /* Tell library a program using it has started */
        __progonly_program_startup(exitjmp, (int *)&__startup_error);
        D(bug("%s: Library startup called\n", __func__));

        __startup_entries_next();
    }
    else
    {
        D(bug("%s: setjmp() return from longjmp\n", __func__));
    }

    /* Tell library program has reached the end */
    __progonly_program_end();

    D(bug("%s: Leave\n", __func__));
}

ADD2SET(__program_startup, PROGRAM_ENTRIES, 0);
