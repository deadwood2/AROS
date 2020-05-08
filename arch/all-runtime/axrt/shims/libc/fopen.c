/*
    Copyright © 2019-2020, The AROS Development Team. All rights reserved.
    $Id$
*/


#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

#include "internal.h"

__attribute__((visibility("default"))) FILE * fopen64 (const char * restrict path, const char * restrict mode)
{
    FILE *_return = NULL;
    FILE *(*fun)(const char * restrict path, const char * restrict mode);
    char lpathname[1024];

    __shims_amiga2host(path, lpathname);

    fun = dlsym(RTLD_NEXT, "fopen64");
    _return = fun(lpathname, mode);

    return _return;
}

__attribute__((visibility("default"))) FILE * fopen (const char * restrict path, const char * restrict mode)
{
    return fopen64(path, mode);
}

