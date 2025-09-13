/*
    Copyright (C) 2008, The AROS Development Team. All rights reserved.

    Legacy version of C99 functions strtoimax() and strtoumax().
*/

#include <stdlib.h>
#include <inttypes.h>

legacy_intmax_t legacy_strtoimax (const char * nptr,
                                  char      ** endptr,
                                  int          base)
{
#if defined(AROS_HAVE_LONG_LONG)
    return (legacy_intmax_t) strtoll(nptr, endptr, base);
#else
    return (legacy_intmax_t) strtol(nptr, endptr, base);
#endif
}

legacy_uintmax_t legacy_strtoumax (const char * nptr,
                                   char      ** endptr,
                                   int          base)
{
#if defined(AROS_HAVE_LONG_LONG)
    return (legacy_uintmax_t) strtoull(nptr, endptr, base);
#else
    return (legacy_uintmax_t) strtoul(nptr, endptr, base);
#endif
}
