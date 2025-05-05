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

size_t __wcsnrtombs(char * restrict dst, const wchar_t ** restrict src, size_t nwc, size_t len, mbstate_t * restrict ps)
{
        mbstate_t mbsbak;
        char buf[MB_CUR_MAX];
        const wchar_t *s;
        size_t nbytes;
        size_t nb;

        if (nwc == 0 || len == 0)
                return 0;

        s = *src;
        nbytes = 0;

        if (dst == NULL) {
                while (nwc-- > 0) {
                        if ((nb = wcrtomb(buf, *s, ps)) == (size_t)-1)
                                /* Invalid character - wcrtomb() sets errno. */
                                return ((size_t)-1);
                        else if (*s == L'\0')
                                break;
                        s++;
                        nbytes += nb;
                }
                return (nbytes + nb - 1);
        }

        while (len > 0 && nwc-- > 0) {
                if (len > (size_t)MB_CUR_MAX) {
                        /* Enough space to translate in-place. */
                        if ((nb = (int)wcrtomb(dst, *s, ps)) < 0) {
                                *src = s;
                                return ((size_t)-1);
                        }
                } else {
                        /*
                         * May not be enough space; use temp. buffer.
                         *
                         * We need to save a copy of the conversion state
                         * here so we can restore it if the multibyte
                         * character is too long for the buffer.
                         */
                        mbsbak = *ps;
                        if ((nb = (int)wcrtomb(buf, *s, ps)) < 0) {
                                *src = s;
                                return ((size_t)-1);
                        }
                        if (nb > (int)len) {
                                /* MB sequence for character won't fit. */
                                *ps = mbsbak;
                                break;
                        }
                        memcpy(dst, buf, nb);
                }
                if (*s == L'\0') {
                        *src = NULL;
                        return (nbytes + nb - 1);
                }
                s++;
                dst += nb;
                len -= nb;
                nbytes += nb;
        }
        *src = s;
        return (nbytes);
}

/*****************************************************************************

    NAME */
size_t wcsnrtombs(

    /*  SYNOPSIS */
        char * restrict dst, 
        const wchar_t ** restrict src,
        size_t nwc, 
        size_t len, 
        mbstate_t * restrict ps)

/*  FUNCTION
        The wcsnrtombs() function is like the wcsrtombs() function, 
        except that the number of wide characters to be converted, starting at *src, 
        is limited to nwc.

    INPUTS
        dst - Pointer to an array of char elements long enough to store a C string of len bytes.
        src - Pointer to a C wide string to be translated.
        nwc - Maximum number of bytes the conversion is limited to.
        len - Maximum number of bytes characters to write to dst.
        ps  - Pointer to a mbstate_t object that defines a conversion state.

    RESULT
        Returns the number of bytes that make up the converted part of multibyte sequence,
        not including the terminating null byte.

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
        return (__wcsnrtombs(dst, src, nwc, len, ps));
}