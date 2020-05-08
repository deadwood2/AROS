/*
    Copyright © 2019-2020, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "internal.h"
#include "libc_funcs.h"

__attribute__((visibility("default"))) FILE * fopen64 (const char * restrict path, const char * restrict mode)
{
    FILE *_return = NULL;
    char lpathname[1024];

    __shims_amiga2host(path, lpathname);

    _return = __libcfuncs.fopen64(lpathname, mode);

    return _return;
}

__attribute__((visibility("default"))) FILE * fopen (const char * restrict path, const char * restrict mode)
{
    return fopen64(path, mode);
}

