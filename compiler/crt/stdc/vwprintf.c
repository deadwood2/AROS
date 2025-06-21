/*
    Copyright (C) 1995-2025, The AROS Development Team. All rights reserved.

    C99 function vwprintf().
*/

/*****************************************************************************

    NAME */
#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>

        int vwprintf (

/*  SYNOPSIS */
        const wchar_t * restrict format,
        va_list args)

/*  FUNCTION
        Format a list of arguments and print them on the standard output.

    INPUTS
        format - A wprintf() format string.
        args - A list of arguments for the format string.

    RESULT
        The number of characters written.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
    //struct PosixCBase *PosixCBase = __aros_getbase_PosixCBase();

    return vfwprintf (stdout, format, args);
} /* vwprintf */
