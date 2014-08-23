/*
    Copyright © 2009-2014, The AROS Development Team. All rights reserved.
    $Id$

    Desc: autoinit library - open IO window when started from WB
*/
#include <dos/stdio.h>
#include <proto/dos.h>

#include <aros/debug.h>
#include "autoinit_intern.h"

/*****************************************************************************

    NAME */
#include <proto/autoinit.h>

        AROS_LH2(VOID, StdIOWinExit,

/*  SYNOPSIS */
        AROS_LHA(BPTR *, handles, A1),
        AROS_LHA(APTR, WBenchMsg, D5),

/*  LOCATION */
        struct Library *, AutoinitBase, 6, Autoinit)

/*  FUNCTION
        Clean up for StdIOWinInit

    INPUTS
        handles - array of 6 BPTR

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

    struct Process *me = (struct Process *)FindTask(NULL);

    if (!WBenchMsg)
        return;

    D(bug("[__startup_stdiowin] Program executed\n"));

    SelectInput(__old_in);
    SelectOutput(__old_out);
    me->pr_CES = __old_err;

    Close(__iowinr);
    Close(__iowine);
    Close(__iowinw);

    D(bug("[__startup_stdiowin] Done!\n"));

    AROS_LIBFUNC_EXIT
} /* StdIOWinExit */
