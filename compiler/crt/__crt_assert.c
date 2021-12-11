/*
    Copyright (C) 1995-2013, The AROS Development Team. All rights reserved.

    assert()
*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "__crt_intbase.h"

/*****************************************************************************

    NAME */
#include <assert.h>

        void __assert (

/*  SYNOPSIS */
        const char * expr,
        const char * file,
        unsigned int line)

/*  FUNCTION
        This is a function that is used for implementation of the C99 assert()
        function.

    INPUTS
        expr - The expression to evaluate. The type of the expression does
                not matter, only if its zero/NULL or not.
        file - Name of the source file.
        line - Line number of assert() call.

    RESULT
        The function doesn't return.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
    fprintf (stderr, "Assertion (%s) failed in %s:%u\n", expr, file, line);
    if (__aros_get_ProgCtx())
        __progonly_exit (10);
    else
    {
        int a = 0 / 0;
        // FIXME!!! WHAT TO DO HERE
    }
} /* assert */

