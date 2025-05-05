/*-
 * Copyright (c) 2002-2004 Tim J. Robbins.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

/*****************************************************************************

    NAME */

size_t mbsrtowcs(

    /*  SYNOPSIS */
    wchar_t * restrict dst, 
    const char ** restrict src, 
    size_t len,
    mbstate_t * restrict ps)

/*  FUNCTION
        Converts a sequence of multibyte characters that begins in the conversion state described by the object pointed to by ps, 
        from the array indirectly pointed to by src into a sequence of corresponding wide characters. 
        If dst is not a null pointer, the converted characters are stored into the array pointed to by dst. 
        Conversion continues up to and including a terminating null character, which is also stored. 
        Conversion stops earlier in two cases: when a sequence of bytes is encountered that does not form a valid multibyte character, 
        or (if dst is not a null pointer) when len wide characters have been stored into the array pointed to by dst. 
        Each conversion takes place as if by a call to the mbrtowc function. 

    INPUTS
        dst - Pointer to an array of wchar_t elements long enough to store a string of len wide characters.
        src - Pointer to a C multibyte character string to be interpreted (an indirect pointer).
        len - Maximum number of wide characters to write to dst.
        ps  - Pointer to a mbstate_t object that defines a conversion state.

    RESULT
        The number of wide characters that make up the converted part of the wide-character string, 
        not including the terminating null wide character.

    NOTES
        stdc.library currently only implements "C" locale.

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
	static mbstate_t mbs;

	if (ps == NULL)
		ps = &mbs;
	return (mbsnrtowcs(dst, src, SIZE_T_MAX, len, ps));
}