/*
    Copyright (C) 2013-2018, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>

#define __NOBLIBBASE__

#include <proto/locale.h>
#include <proto/intuition.h>

#include "__stdlib_intbase.h"
#include "__stdlib_optionallibs.h"

/* Internal function __libfindandopen will only open a library when it is
   already in the list of open libraries
*/
static struct Library *__libfindandopen(const char *libname, int version)
{
    struct Node *found;

    Forbid();
    found = FindName(&SysBase->LibList, libname);
    Permit();

    return (found != NULL) ? OpenLibrary(libname, version) : NULL;
}

int __locale_available(struct StdlibIntBase *StdlibBase)
{
    if (StdlibBase->_LocaleBase == NULL)
        StdlibBase->_LocaleBase = (struct LocaleBase *)__libfindandopen("locale.library", 0);

    return StdlibBase->_LocaleBase != NULL;
}

int __intuition_available(struct StdlibIntBase *StdlibBase)
{
    if (StdlibBase->_IntuitionBase == NULL)
        StdlibBase->_IntuitionBase = (struct IntuitionBase *)__libfindandopen("intuition.library", 0);

    return StdlibBase->_IntuitionBase != NULL;
}
