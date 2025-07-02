/*
    Copyright (C) 1995-2022, The AROS Development Team. All rights reserved.

    C99 function fgetws().
	Original source from libnix
*/

/*****************************************************************************

    NAME */
#include <stdio.h>
#include <wchar.h>

wchar_t *fgetws(

/*  SYNOPSIS */
	wchar_t * restrict s, 
	int n, 
	FILE * restrict stream)

/*  FUNCTION
        Read one line of wide characters from the stream into the buffer.
        Reading will stop, when a newline ('\n') is encountered, WEOF
        or when the buffer is full. If a newline is read, then it is
        put into the buffer. The last wide character in the buffer is always
        '\0' (Therefore at most size-1 characters can be read in one go).

    INPUTS
        s - Write wide characters into this buffer
        n - This is the size of the buffer in wide characters.
        stream - Read from this stream

    RESULT
        buffer or NULL in case of an error or EOF.

    NOTES

    EXAMPLE
        // Read a file line by line
        wchar_t line[256];

        // Read until WEOF
        while (fgetws (line, sizeof (line), fh))
        {
            // Evaluate the line
        }

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
	wchar_t *s2 = s;

	flockfile(stream);
	while (--n) {
		int c = fgetwc(stream);
		if (c == WEOF) {
			if (s2 == s) {
				funlockfile(stream);
				return NULL;
			}
			break;
		}
		*s2++ = c;
		if (c == '\n')
			break;
	}
	*s2++ = '\0';
	funlockfile(stream);
	return s;
}
