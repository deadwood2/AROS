/*
    Copyright (C) 1995-2020, The AROS Development Team. All rights reserved.

    C99 function exit()
*/


#include <aros/debug.h>

#include <assert.h>

#include "debug.h"
#include "../crt/__crt_progonly.h"

/*****************************************************************************

    NAME */
#include <stdlib.h>

        void __progonly_exit (

/*  SYNOPSIS */
        int code)

/*  FUNCTION
        Terminates the running program. The code is returned to the
        program which has called the running program.

    INPUTS
        code - Exit code. 0 for success, other values for failure.

    RESULT
        None. This function does not return.

    NOTES
        This function must not be used in a shared library or
        in a threaded application.

   EXAMPLE
        if (no_problems)
            exit (0);

        if (warning)
            exit (5);

        if (error)
            exit (10);

        if (fatal)
            exit (20);

    BUGS

    SEE ALSO
        atexit(), on_exit()

    INTERNALS

******************************************************************************/
{
    D(bug("[%s] %s(%d)\n", STDCNAME, __func__, code));

    __progonly_jmp2exit(1, code);

    /* never reached */
    assert(0);
} /* exit */

