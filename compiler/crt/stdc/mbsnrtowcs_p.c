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

#include <stdlib.h>
#include <string.h>
#include <wchar.h>

/*****************************************************************************

    NAME */

size_t mbsnrtowcs(

        /*  SYNOPSIS */
        wchar_t * restrict dst, 
        const char ** restrict src,
        size_t nms, 
        size_t len, 
        mbstate_t * restrict ps)
    
    /*  FUNCTION
            The mbsnrtowcs() function is like the mbsrtowcs(3) function, 
            except that the number of bytes to be converted, 
            starting at *src, is limited to at most nms bytes.
    
        INPUTS
            dst - Pointer to an array of wchar_t elements long enough to store a string of len wide characters.
            src - Pointer to a C multibyte character string to be interpreted (an indirect pointer).
            nms - Maximum number of bytes the conversion is limited to.
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
     const char *s;
     size_t nchr;
     wchar_t wc;
     size_t nb;

     if (nms == 0 || len == 0)
        return 0;

     s = *src;
     nchr = 0;
 
     if (dst == NULL) {
         for (;;) {
             if ((nb = mbrtowc(&wc, s, nms, ps)) == (size_t)-1)
                 /* Invalid sequence - mbrtowc() sets errno. */
                 return ((size_t)-1);
             else if (nb == 0 || nb == (size_t)-2)
                 return (nchr);
             s += nb;
             nms -= nb;
             nchr++;
         }
         /*NOTREACHED*/
     }
 
     while (len-- > 0) {
         if ((nb = mbrtowc(dst, s, nms, ps)) == (size_t)-1) {
             *src = s;
             return ((size_t)-1);
         } else if (nb == (size_t)-2) {
             *src = s + nms;
             return (nchr);
         } else if (nb == 0) {
             *src = NULL;
             return (nchr);
         }
         s += nb;
         nms -= nb;
         nchr++;
         dst++;
     }
     *src = s;
     return (nchr);
}
