/*
    Copyright (C) 1995-2025, The AROS Development Team. All rights reserved.

    Format a wide string and call a usercallback to output each char.
*/
/* Original source from libnix */

#include <stdarg.h>

/*****************************************************************************

    NAME */
#include <stdio.h>
#include <wchar.h>

        int vfwprintf (

/*  SYNOPSIS */
        FILE  * restrict stream,
        const wchar_t * restrict format,
        va_list args)

/*  FUNCTION
        Format a list of arguments and print them on the specified stream.

    INPUTS
        stream - A stream on which one can write
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
    int retval;

    retval = __wvcformat (stream, (int (*)(int, void *))fputc, format, args);

    return retval;
} /* vfwprintf */
