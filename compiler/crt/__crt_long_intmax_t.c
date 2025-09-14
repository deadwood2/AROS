/*
    Copyright (C) 2008, The AROS Development Team. All rights reserved.

    Legacy version of C99 functions strtoimax() and strtoumax().
*/

#include <stdlib.h>

long legacy_strtoimax (const char * nptr,
                       char      ** endptr,
                       int          base)
{
#if defined(AROS_HAVE_LONG_LONG)
    return (long) strtoll(nptr, endptr, base);
#else
    return (long) strtol(nptr, endptr, base);
#endif
}

unsigned long legacy_strtoumax (const char * nptr,
                                char      ** endptr,
                                int          base)
{
#if defined(AROS_HAVE_LONG_LONG)
    return (unsigned long) strtoull(nptr, endptr, base);
#else
    return (unsigned long) strtoul(nptr, endptr, base);
#endif
}
