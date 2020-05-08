/*
    Copyright © 2020, The AROS Development Team. All rights reserved.
    $Id$
*/


#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

#include "internal.h"

typedef unsigned short mode_t;

__attribute__((visibility("default"))) int mkdir(const char * path, mode_t mode)
{
    int _return = 0;
    int (*fun)(const char * path, mode_t mode);
    char lpathname[1024];

    __shims_amiga2host(path, lpathname);

    fun = dlsym(RTLD_NEXT, "mkdir");
    _return = fun(lpathname, mode);

    return _return;
}

