/*-
 * Copyright (c) 2002-2004 Tim J. Robbins. All rights reserved.
 * Copyright (c) 1993
 *      The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Paul Borman at Krystal Technologies.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the University of
 *      California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <wchar.h>
#include <limits.h>
#include <errno.h>

size_t __wcrtomb(char * restrict s, wchar_t wc, mbstate_t * restrict ps __unused)
{

    if (s == NULL)
            /* Reset to initial shift state (no-op) */
            return (1);
    if (wc < 0 || wc > UCHAR_MAX) {
            errno = EILSEQ;
            return ((size_t)-1);
    }
    *s = (unsigned char)wc;
    return (1);
}

/*****************************************************************************

    NAME */

size_t wcrtomb(

    /*  SYNOPSIS */
    char * restrict s, 
    wchar_t wc,
    mbstate_t * restrict ps)

/*  FUNCTION
        Copies the value of c into each of the first n wide characters of the object pointed to by s.

    INPUTS
        s  - Pointer to an array large enough to hold a multibyte sequence.
        wc - Wide character of type wchar_t.
        ps - Pointer to a mbstate_t object that defines a conversion state.

    RESULT
        Returns 1. (see notes)

    NOTES
        stdc.library currently only implements "C" locale
        this means the function will always return 1.

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
	static mbstate_t mbs;

	if (ps == NULL)
		ps = &mbs;
	return (__wcrtomb(s, wc, ps));
}