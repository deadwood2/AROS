/*
    Copyright © 2020, The AROS Development Team. All rights reserved.
    $Id$
*/


#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

#include "internal.h"

/* open64() defined in startup.o */ 

__attribute__((visibility("default"))) int __shims_libc_open64(const char * pathname, int oflag, ...)
{
    int _return = 0;
    int (*fun)(const char * pathname, int oflag);
    char lpathname[1024];

    __shims_amiga2host(pathname, lpathname);

    fun = dlsym(RTLD_NEXT, "open64");
    _return = fun(lpathname, oflag);

    return _return;
}

/* Backwards compatibility. Do not remove in ABI 2.0 */
__attribute__((visibility("default"))) int __shim_open64(const char * pathname, int oflag, ...) __attribute__ ((alias ("__shims_libc_open64")));
