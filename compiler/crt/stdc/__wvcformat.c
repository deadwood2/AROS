/*
    Copyright (C) 1995-2018, The AROS Development Team. All rights reserved.

    Function to format a string like wprintf().
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#ifndef AROS_NO_LIMITS_H
#       include <limits.h>
#else
#       define ULONG_MAX   4294967295UL
#endif
#include <math.h>
#include <float.h>

#include <ctype.h>
#include <wchar.h>

#ifndef STDC_STATIC
#define FULL_SPECIFIERS
#endif
#define IS_WIDE

const unsigned char *const __decimalpoint = ".";

/* support macros for FMTPRINTF */
#define FMTPRINTF_COUT(c)  do                           \
                { if((*outc)((unsigned char)(c),data)==EOF)   \
                    return outcount;         \
                  outcount++;                \
                }while(0)
#define FMTPRINTF_WCOUT(c) do \
                { if(putwc(c,data)==WEOF) \
                    return outcount; \
                  outcount++; \
                } while (0);

#define FMTPRINTF_STRLEN(str) strlen(str)
#define FMTPRINTF_WCSLEN(widestr) wcslen(widestr)

#define FMTPRINTF_DECIMALPOINT  __decimalpoint


#include "fmtprintf_pre.c"

/*****************************************************************************

    NAME */

        int __wvcformat (

/*  SYNOPSIS */
        void       * data,
        int       (* outc)(int, void *),
        const wchar_t * format,
        va_list      args)

/*  FUNCTION
        Format a list of arguments and call a function for each char
        to print.

    INPUTS
        data - This is passed to the user callback outc as its second argument.
        outc - Call this function for every character that should be
                emitted. The function should return EOF on error and
                > 0 otherwise.
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
#include "fmtprintf.c"
  return outcount;
}
