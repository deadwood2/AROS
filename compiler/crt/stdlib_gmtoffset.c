/*
    Copyright (C) 1995-2018, The AROS Development Team. All rights reserved.

    Internal function to get current GMT offset
*/
#include <proto/exec.h>

#define __NOBLIBBASE__

#include <proto/locale.h>
#include <exec/execbase.h>

#include "__stdlib_intbase.h"
#include "__stdlib_optionallibs.h"

/*****************************************************************************

    NAME */
#include <time.h>

        int __stdc_gmtoffset (

/*  SYNOPSIS */
        void)

/*  FUNCTION

    INPUTS

    RESULT
        The offset to GMT in minutes

    NOTES
        Will return 0 when locale.library is not loaded into memory yet.

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS
        Will always query the current locale through locale.library to
        get the GMT offset.

******************************************************************************/
{
    struct StdlibIntBase *StdlibIntBase =
        (struct StdlibIntBase *)__aros_getbase_StdlibBase();
    struct LocaleBase *LocaleBase;
    int gmtoffset = 0;

    if (__locale_available(StdlibIntBase))
    {
        struct Locale *loc;
        LocaleBase = StdlibIntBase->_LocaleBase;
        if ((loc = OpenLocale(NULL)))
        {
            gmtoffset = (int)loc->loc_GMTOffset;
            CloseLocale(loc);
        }
    }

    return gmtoffset;
}
