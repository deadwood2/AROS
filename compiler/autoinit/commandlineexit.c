/*
    Copyright © 1995-2014, The AROS Development Team. All rights reserved.
    $Id$
*/
#include <aros/debug.h>

/*****************************************************************************

    NAME */
#include <proto/autoinit.h>

        AROS_LH5(VOID, CommandLineExit,

/*  SYNOPSIS */
        AROS_LHA(ULONG, __argsize, D1),
        AROS_LHA(STRPTR , __args, A2),
        AROS_LHA(LONG, __argmax, D2),
        AROS_LHA(STRPTR *, __argv, A4),
        AROS_LHA(APTR, WBenchMsg, D5),

/*  LOCATION */
        struct Library *, AutoinitBase, 9, Autoinit)

/*  FUNCTION
        Clean up for CommandLineInit.

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

    if (WBenchMsg != NULL)
        return; /* Values of __argv and __args cannot be trusted */

    if (__argv)
        FreeMem(__argv, sizeof (char *) * (__argmax+1));

    if (__args)
        FreeMem(__args, __argsize+1);

    AROS_LIBFUNC_EXIT
} /* CommandLineExit */
