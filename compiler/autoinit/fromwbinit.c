/*
    Copyright © 2009-2014, The AROS Development Team. All rights reserved.
    $Id$

    Desc: autoinit library - handle program started from workbench.
*/
#include <proto/dos.h>
#include <workbench/startup.h>

#include <aros/debug.h>

#include "autoinit_intern.h"

#define __argv      (*__argvp)
#define __argc      (*__argcp)
#define WBenchMsg   (*WBenchMsgp)
#define curdir      (*curdirp)

/*****************************************************************************

    NAME */
#include <proto/autoinit.h>

        AROS_LH4(BOOL, FromWBInit,

/*  SYNOPSIS */
        AROS_LHA(struct WBStartup **, WBenchMsgp, A1),
        AROS_LHA(BPTR *, curdirp, A2),
        AROS_LHA(STRPTR **, __argvp, A4),
        AROS_LHA(LONG *, __argcp, A5),


/*  LOCATION */
        struct Library *, AutoinitBase, 10, Autoinit)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES
        If the startup code has to change in backwards incompatible way
        do not change this function. Create a new one and keep this compatible
        so that already compiled executatables continue running.

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    struct Process *myproc;

    D(bug("Entering __startup_fromwb()\n"));

    myproc = (struct Process *)FindTask(NULL);

    /* Do we have a CLI structure? */
    if (!myproc->pr_CLI)
    {
        /* Workbench startup. Get WBenchMsg and pass it to main() */

        WaitPort(&myproc->pr_MsgPort);
        WBenchMsg = (struct WBStartup *)GetMsg(&myproc->pr_MsgPort);
        __argv = (STRPTR *) WBenchMsg;
        __argc = 0;

        /* ABI_V0 compatibility */
#if 0
        /* WB started processes' pr_CurrentDir = BNULL */
        curdir = DupLock(WBenchMsg->sm_ArgList->wa_Lock);
        CurrentDir(curdir);
#endif

        D(bug("[startup] Started from Workbench\n"));
    }

    return TRUE;

    AROS_LIBFUNC_EXIT
} /* FromWBInit */
