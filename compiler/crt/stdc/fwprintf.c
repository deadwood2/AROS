/*
    Copyright (C) 1995-2025, The AROS Development Team. All rights reserved.

    C99 function fwprintf().
*/

#include <stdarg.h>

/*****************************************************************************

    NAME */
#include <stdio.h>
#include <wchar.h>

        int fwprintf (

/*  SYNOPSIS */
        FILE * restrict fh,
        const wchar_t * restrict format,
        ...)

/*  FUNCTION
        Format a wide string with the specified arguments and write it to
        the stream.

    INPUTS
        fh - Write to this stream
        format - How to format the arguments
        ... - The additional arguments

    RESULT
        The number of characters written to the stream or EOF on error.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
    int     retval;
    va_list args;


    va_start (args, format);

    retval = vfwprintf (fh, format, args);

    va_end (args);

    return retval;
} /* fwprintf */
