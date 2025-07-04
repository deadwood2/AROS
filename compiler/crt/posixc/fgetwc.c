/*
    Public Domain
    Original source from libnix
    
    C99 function fgetwc().
	
*/

/*****************************************************************************

    NAME */
#include <errno.h>
#include <stdlib.h>
#include <wchar.h>
#include <stdio.h>

wint_t fgetwc(

/*  SYNOPSIS */
	FILE *stream)

/*  FUNCTION
        Read one wide character from the stream. If there is no character
        available or an error occurred, the function returns WEOF.

    INPUTS
        stream - Read from this stream

    RESULT
        The wide character read or WEOF on end of file or error.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
	wint_t b;
	if (fread((char *) &b, 1, sizeof(wint_t), stream))
		return b;
	return -1;
}
