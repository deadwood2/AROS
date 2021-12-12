/*
    Copyright (C) 1995-2013, The AROS Development Team. All rights reserved.

    assert()
*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "__crt_progonly.h"

/*****************************************************************************

    NAME */
#include <assert.h>

        void __progonly_assert (

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
    __progonly_abort();
} /* assert */

/*****************************************************************************

    NAME */
#include <assert.h>

        void __modonly_assert (

/*  SYNOPSIS */
        const char * expr,
        const char * file,
        unsigned int line)

/*  FUNCTION
        This is a function that is used for implementation of the C99 assert()
        function for use in shared libraries.

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
    __modonly_abort();
} /* assert */

AROS_MAKE_ASM_SYM(typeof(__assert), __assert, AROS_CSYM_FROM_ASM_NAME(__assert), AROS_CSYM_FROM_ASM_NAME(__modonly_assert));
AROS_EXPORT_ASM_SYM(AROS_CSYM_FROM_ASM_NAME(__assert));
