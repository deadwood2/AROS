/*
    Copyright © 2020, The AROS Development Team. All rights reserved.
    $Id$
*/


#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

#include "internal.h"

__attribute__((visibility("default"))) int __xstat64(int ver, char * path, void * buf)
{
    int _return = 0;
    int (*fun)(int ver, const char * path, void * buf);
    char lpathname[1024];

    __shims_amiga2host(path, lpathname);

    fun = dlsym(RTLD_NEXT, "__xstat64");
    _return = fun(ver, lpathname, buf);

    return _return;
}

