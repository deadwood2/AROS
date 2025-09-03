/*
    Copyright (C) 2025, The AROS Development Team. All rights reserved.

    Wide-character version of scanf() parser.
*/

#include <wchar.h>
#include <wctype.h>
#include <stdlib.h>
#include <limits.h>
#include <stdarg.h>
#include <math.h>

#ifndef STDC_STATIC
#define FULL_SPECIFIERS
#endif

#define NEXT(c) ((c)=(*get_char)(data), size++, incount++)
#define PREV(c) do { if ((c) != WEOF) (*unget_char)((c), data); size--; incount--; } while (0)
#define VAL(a) ((a) && size <= width)

extern wchar_t *__stdc_wchar_decimalpoint;

struct vcs_ieeetype
{
    union
    {
        double doub;
        unsigned char uchar[sizeof(double)];
    };
};

#ifdef FULL_SPECIFIERS
const static struct vcs_ieeetype undef[3] =
{
    { .uchar = { 0x7f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }},
    { .uchar = { 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }},
    { .uchar = { 0x7f, 0xf1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }}
};
#endif

/*****************************************************************************

    NAME */

        int __vwscanf (

/*  SYNOPSIS */
        void       * data,
        wint_t    (* get_char)(void *),
        int       (* unget_char)(wint_t, void *),
        const wchar_t * format,
        va_list      args)

/*  FUNCTION
        Core wide-character scanf parser. Reads formatted wide-character input
        from the data source using the provided get/unget character functions,
        parsing according to the given format string, and stores the results
        into the locations described by the argument list.

    INPUTS
        data       - Opaque pointer passed to get_char and unget_char
        get_char   - Function to read the next character from the input
        unget_char - Function to push back a character into the input
        format     - How to convert the input into the arguments
        args       - List of argument pointers to receive results

    RESULT
        The number of converted arguments.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
        vwscanf()

    INTERNALS
        This is the internal implementation of wide-character scanf parsing.
        All other wide-character scanf functions wrap this by providing suitable
        character input and push-back functions for their respective sources.

******************************************************************************/
{
    size_t blocks = 0, incount = 0;
    wint_t c = 0;

    while (*format)
    {
        size_t size = 0;

        if (*format == '%')
        {
            size_t width = ULONG_MAX;
            wchar_t type, subtype = 'i', ignore = 0;
            char lltype = 0;
            const wchar_t *ptr = format + 1;

            if (*ptr == '*')
            {
                ignore = 1;
                ptr++;
            }

            if (iswdigit(*ptr))
            {
                width = 0;
                while (iswdigit(*ptr))
                    width = width * 10 + (*ptr++ - L'0');
            }

            while (*ptr == 'h' || *ptr == 'l' || *ptr == 'L')
            {
                if (subtype == 'l' && *ptr == 'l')
                    lltype = 1;
                else
                    subtype = *ptr;
                ptr++;
            }

            type = *ptr++;

            if (type && type != '%' && type != 'c' && type != 'n' && type != '[')
            {
                do NEXT(c); while (iswspace(c));
                size = 1;
            }

            switch (type)
            {
            case 'c':
            {
                wchar_t *bp = ignore ? NULL : va_arg(args, wchar_t *);

                if (width == ULONG_MAX) width = 1;

                NEXT(c);
                while (VAL(c != WEOF))
                {
                    if (!ignore) *bp++ = c;
                    NEXT(c);
                }
                PREV(c);

                if (!ignore && size) blocks++;
                break;
            }
            case 's':
            {
                wchar_t *bp = ignore ? NULL : va_arg(args, wchar_t *);

                while (VAL(c != WEOF && !iswspace(c)))
                {
                    if (!ignore) *bp++ = c;
                    NEXT(c);
                }
                PREV(c);

                if (!ignore && size)
                {
                    *bp++ = L'\0';
                    blocks++;
                }
                break;
            }
#ifdef FULL_SPECIFIERS
            case 'e':
            case 'f':
            case 'g':
            {
                wchar_t buf[128];
                size_t pos = 0;

                while (VAL(pos < sizeof(buf) / sizeof(buf[0]) - 1 &&
                        (iswdigit(c) || c == L'+' || c == L'-' || c == L'.' || towlower(c) == L'e' || towlower(c) == L'n' || towlower(c) == L'i')))
                {
                    buf[pos++] = c;
                    NEXT(c);
                }
                PREV(c);

                buf[pos] = L'\0';

                double v = wcstod(buf, NULL);

                if (!ignore && size)
                {
                    switch (subtype)
                    {
                    case 'l':
                    case 'L':
                        *va_arg(args, double *) = v;
                        break;
                    case 'i':
                        *va_arg(args, float *) = (float)v;
                        break;
                    }
                    blocks++;
                }
                break;
            }
#endif
            default:
            {
                wchar_t buf[128];
                size_t pos = 0;
                int base = (type == 'x' || type == 'X') ? 16 : (type == 'o' ? 8 : 10);

                if (c == L'+' || c == L'-')
                {
                    buf[pos++] = c;
                    NEXT(c);
                }

                while (VAL(pos < sizeof(buf) / sizeof(buf[0]) - 1 &&
                        ((base == 16 && iswxdigit(c)) ||
                        (base == 10 && iswdigit(c)) ||
                        (base == 8 && c >= L'0' && c <= L'7'))))
                {
                    buf[pos++] = c;
                    NEXT(c);
                }
                PREV(c);

                buf[pos] = L'\0';

                if (!ignore && size)
                {
                    if (type == 'u')
                    {
                        unsigned long long v = wcstoull(buf, NULL, base);
                        switch (subtype)
                        {
                        case 'l': case 'L':
                            *va_arg(args, unsigned long long *) = v;
                            break;
                        case 'i':
                            *va_arg(args, unsigned int *) = (unsigned int)v;
                            break;
                        case 'h':
                            *va_arg(args, unsigned short *) = (unsigned short)v;
                            break;
                        }
                    }
                    else
                    {
                        long long v = wcstoll(buf, NULL, base);
                        switch (subtype)
                        {
                        case 'l': case 'L':
                            *va_arg(args, long long *) = v;
                            break;
                        case 'i':
                            *va_arg(args, int *) = (int)v;
                            break;
                        case 'h':
                            *va_arg(args, short *) = (short)v;
                            break;
                        }
                    }
                    blocks++;
                }
                break;
            }
            }

            format = ptr;
        }
        else
        {
            if (iswspace(*format))
            {
                do NEXT(c); while (iswspace(c));
                PREV(c);
                size = 1;
            }
            else
            {
                NEXT(c);
                if (c != *format) PREV(c);
            }
            format++;
        }

        if (!size)
            break;
    }

    if (c == WEOF && !blocks)
        return c;
    else
        return blocks;
}
