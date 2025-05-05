/*-
 * Copyright (c) 2002-2004 Tim J. Robbins. All rights reserved.
 * Copyright (c) 1993
 *	The Regents of the University of California.  All rights reserved.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
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

#include <errno.h> 
#include <wchar.h>

/*****************************************************************************

    NAME */

size_t mbrtowc(
    
/*  SYNOPSIS */    
    wchar_t * restrict pwc, 
    const char * restrict s, 
    size_t n,
    mbstate_t * restrict ps __unused)

/*  FUNCTION
        Convert a multibyte sequence to a wide character.

    INPUTS
        pwc - Pointer to an object of type wchar_t.
        s   - Pointer to the first byte of a multibyte character.
        n   - Maximum number of bytes to read from s.
        ps  - Pointer to a mbstate_t object that defines a conversion state.

    RESULT
        The number of bytes from s used to produce the wide character. 

    NOTES
        stdc.library currently only implements "C" locale
        this means the object ps is marked as unused

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{

        if (s == NULL)
                /* Reset to initial shift state (no-op) */
                return (0);
        if (n == 0)
                /* Incomplete multibyte sequence */
                return ((size_t)-2);
        if (*s & 0x80) {
                    errno = EILSEQ;
                    return ((size_t)-1);
        }
        if (pwc != NULL)
                *pwc = (unsigned char)*s;
        return (*s == '\0' ? 0 : 1);
}