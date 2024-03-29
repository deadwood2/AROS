/*
    Copyright (C) 2003-2013, The AROS Development Team. All rights reserved.

    C99 function _Exit().
*/


#include <assert.h>

#include "../crt/__crt_progonly.h"

/*****************************************************************************

    NAME */
#include <stdlib.h>

        void __progonly__Exit (

/*  SYNOPSIS */
        int code)

/*  FUNCTION
        Terminates the running program immediately. The code is returned to
        the program which has called the running program. In contrast to
        exit(), this function does not call user exit-handlers added with
        atexit() or on_exit(). It does, however, close open filehandles.

    INPUTS
        code - Exit code. 0 for success, other values for failure.

    RESULT
        None. This function does not return.

    NOTES
        This function must not be used in a shared library or in a threaded
        application.

    EXAMPLE

    BUGS

    SEE ALSO
        exit()

    INTERNALS

******************************************************************************/
{
    __progonly_jmp2exit(0, code);

    /* never reached */
    assert(0);
} /* _exit */

