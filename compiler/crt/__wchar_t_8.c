/*
    Copyright (C) 2025, The AROS Development Team. All rights reserved.

    stdlib.library 1.0 backwards compatibility
*/

#include <dos/dos.h>
#include <dos/dosextens.h>
#include <proto/exec.h>

char *__wcscpy_wchar_t_8(char *wcdst, const char *wcsrc)
{
    char *_wcptr;

    /* copy the wide characters */
    for (_wcptr = wcdst; *wcsrc; wcsrc++)
        *_wcptr++ = *wcsrc;

    /* terminate the wide string */
    *_wcptr = 0x0000;

    return wcdst;
} /* wcscpy */

size_t __wcslen_wchar_t_8(const char *wcstr)
{
    int cnt = 0;

    while (*wcstr++) cnt++;

    return cnt;
} /* wcslen */

int __wcsncmp_wchar_t_8(const char *wcstra, const char *wcstrb, size_t cnt)
{
    while ((cnt-- > 0) &&
                *wcstra &&
                *wcstrb &&
                (*wcstra == *wcstrb))
    {
        wcstra++;
        wcstrb++;
    }

    if ((cnt > 0) && *wcstra < *wcstrb)
        return -1;
    else if ((cnt > 0) && *wcstra > *wcstrb)
        return 1;
    return 0;
} /* wcsncmp */

char *__wcsncpy_wchar_t_8(char *wcdst, const char *wcsrc, size_t cnt)
{
    char *_wcptr;

    /* copy the wide characters */
    for (_wcptr = wcdst; (cnt > 0) && *wcsrc; wcsrc++)
    {
        *_wcptr++ = *wcsrc;
        cnt--;
    }

    /* terminate the wide string, and fill the remaining
     * wide characters as specified in the spec... */
    while (cnt > 0)
    {
        *_wcptr++ = 0x0000;
        cnt--;
    }

    return wcdst;
} /* wcsncpy */

char *__wcscat_wchar_t_8(char *wcdst, const char *wcsrc)
{
    char *_wcptr = wcdst;

    /* skip existing characters ... */
    while (*_wcptr) _wcptr++;

    /* and copy the string .. */
    __wcscpy_wchar_t_8(_wcptr, wcsrc);

    return wcdst;
} /* wcscat */

#include <ctype.h>

int __mblen_noerrno(const char *s, size_t n)
{
    if (s == NULL)
        /* No state-dependent encondings */
        return 0;

    if (n == 0 || *s == '\0')
        return 0;

    if (isascii(*s))
        return 1;
    else
        return -1;
} /* mblen */
