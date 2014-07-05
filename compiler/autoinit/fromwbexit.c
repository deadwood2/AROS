/*
    Copyright © 2009-2014, The AROS Development Team. All rights reserved.
    $Id$

    Desc: autoinit library - handle program started from workbench.
*/
#include <proto/dos.h>
#include <workbench/startup.h>

#include <aros/debug.h>

/*****************************************************************************

    NAME */
#include <proto/autoinit.h>

        AROS_LH2(VOID, FromWBExit,

/*  SYNOPSIS */
        AROS_LHA(struct WBStartup *, WBenchMsg, A1),
        AROS_LHA(BPTR, curdir, A2),

/*  LOCATION */
        struct Library *, AutoinitBase, 11, Autoinit)

/*  FUNCTION
        Clean up for FromWBInit.

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

    /* Reply startup message to Workbench */
    if (WBenchMsg)
    {
        /* ABI_V0 compatibility */
#if 0
        /* Close original lock */
        CurrentDir(BNULL);
        UnLock(curdir);
#endif

        Forbid(); /* make sure we're not UnLoadseg()ed before we're really done */
        ReplyMsg((struct Message *) WBenchMsg);
    }

    D(bug("Leaving __startup_fromwb\n"));

    AROS_LIBFUNC_EXIT
} /* FromWBExit */
