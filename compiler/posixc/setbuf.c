/*
    Copyright (C) 1995-2021, The AROS Development Team. All rights reserved.

    C99 function setbuf().
*/

/*****************************************************************************

    NAME */
#include <stdio.h>

        void setbuf (

/*  SYNOPSIS */
        FILE *stream,
        char *buf)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES
        This is a simpler alias for setvbuf() according to manpage.

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
    setvbuf(stream, buf, buf ? _IOFBF : _IONBF, BUFSIZ);
} /* setbuf */

