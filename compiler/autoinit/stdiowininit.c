/*
    Copyright © 2009-2014, The AROS Development Team. All rights reserved.
    $Id$

    Desc: autoinit library - open IO window when started from WB
*/
#include <dos/stdio.h>
#include <proto/dos.h>

#include <aros/debug.h>
#include "autoinit_intern.h"

static BPTR DupFH(BPTR fh, LONG mode, struct DosLibrary * DOSBase)
{
    BPTR nfh;
    struct MsgPort *old;

    if (!fh)
        return BNULL;
    old = SetConsoleTask(((struct FileHandle*)BADDR(fh))->fh_Type);
    nfh = Open("*", mode);
    SetConsoleTask(old);
    return nfh;
}

/*****************************************************************************

    NAME */
#include <proto/autoinit.h>

        AROS_LH3(BOOL, StdIOWinInit,

/*  SYNOPSIS */
        AROS_LHA(BPTR *, handles, A1),
        AROS_LHA(CONST_STRPTR, __stdiowin, A2),
        AROS_LHA(APTR, WBenchMsg, D5),

/*  LOCATION */
        struct Library *, AutoinitBase, 5, Autoinit)

/*  FUNCTION
        If started from Workbench, initializes new console window based
        on __stdiowin descriptor

    INPUTS
        handles - array of 6 BPTRs

    RESULT
        FALSE if there was a failure with initializing new console windows

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

    struct Process *me;

    if (!WBenchMsg)
        return TRUE; /* Continue with processing */

    D(bug("[__startup_stdiowin] Opening console window: '%s'\n", __stdiowin));

    __iowinw = Open(__stdiowin, MODE_OLDFILE);
    __iowinr = DupFH(__iowinw, MODE_OLDFILE, DOSBase);
    if (__iowinr) {
        /* this is so ugly but ReadArgs() needs EOF or
         * console window will always open and program
         * pauses until RETURN is pressed.
         */
        struct FileHandle *fh = BADDR(__iowinr);
        SetVBuf(__iowinr, NULL, BUF_LINE, 1);
        /* force EOF */
        fh->fh_Pos = fh->fh_End + 1;
    }
    __iowine = DupFH(__iowinw, MODE_OLDFILE, DOSBase);

    if (!__iowinr || !__iowinw || !__iowine)
    {
        Close(__iowinr);
        Close(__iowine);
        Close(__iowinw);
        D(bug("[__startup_stdiowin] Failed!\n"));
        return FALSE;
    }

    D(bug("[__startup_stdiowin] Setting standard file handles\n"));
    D(bug("[__startup_stdiowin]     in %p out %p err %p\n", __iowinr, __iowinw, __iowine));

    __old_in = SelectInput(__iowinr);
    __old_out = SelectOutput(__iowinw);
    me = (struct Process *)FindTask(NULL);
    __old_err = me->pr_CES;
    me->pr_CES = __iowine;

    D(bug("[__startup_stdiowin] old in %p out %p err %p\n", __old_in, __old_out, __old_err));

    return TRUE;

    AROS_LIBFUNC_EXIT
} /* StdIOWinInit */
