/*
    Copyright (C) 1995-2025, The AROS Development Team. All rights reserved.

    Format a wide string and call a usercallback to output each char.
*/
/* Original source from libnix */

#include <proto/dos.h>
#include <errno.h>
#include <stdarg.h>
#include "__fdesc.h"
#include "__stdio.h"

static int __putc(int c, void *fh);
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
    fdesc *fdesc = __getfdesc(stream->fd);

    if (!fdesc)
    {
        errno = EBADF;
        return 0;
    }

    return __wvcformat ((void *)BADDR(fdesc->fcb->handle), __putc, format, args);
} /* vfwprintf */

static int __putc(int c, void *fhp)
{
    BPTR fh = MKBADDR(fhp);
    if (FWrite(fh, &c, 1, sizeof(wchar_t)) == WEOF)
    {
        errno = __stdc_ioerr2errno(IoErr());
        return WEOF;
    }

    return c;
}
