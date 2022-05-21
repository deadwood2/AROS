/*
    Copyright (C) 1995-2018, The AROS Development Team. All rights reserved.

    Desc: Formats a message and makes sure the user will see it.
*/

#include <aros/config.h>
#include <aros/arossupportbase.h>
#include <stdarg.h>

#include <aros/system.h>
#include <proto/exec.h>
#include <proto/arossupport.h>
#undef kprintf
#undef vkprintf
#include <exec/execbase.h>

extern int vkprintf (const char * format, va_list args);

/*****************************************************************************

    NAME */
        #include <proto/arossupport.h>

        int kprintf (

/*  SYNOPSIS */
        const char * fmt,
        ...)

/*  FUNCTION
        Formats fmt with the specified arguments like printf() (and *not*
        like RawDoFmt()) and uses a secure way to deliver the message to
        the user; ie. the user *will* see this message no matter what.

    INPUTS
        fmt - printf()-style format string

    RESULT
        The number of characters output.

    NOTES
        This function is not part of a library and may thus be called
        any time.

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
        24-12-95    digulla created

******************************************************************************/
{
    va_list      ap;
    int          result;

    va_start (ap, fmt);
    result = vkprintf (fmt, ap);
    va_end (ap);

    return result;
} /* kprintf */

