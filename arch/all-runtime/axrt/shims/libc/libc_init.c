/*
    Copyright © 2020, The AROS Development Team. All rights reserved.
    $Id$
*/

#define _GNU_SOURCE
#include <dlfcn.h>
#include <exec/types.h>

#include "libc_funcs.h"

struct libcfuncs __libcfuncs;

VOID __shims_libc_init_jumptables()
{
    __libcfuncs.fopen64     = dlsym(RTLD_NEXT, "fopen64");
    __libcfuncs.mkdir       = dlsym(RTLD_NEXT, "mkdir");
    __libcfuncs.open64      = dlsym(RTLD_NEXT, "open64");
    __libcfuncs.__xstat64   = dlsym(RTLD_NEXT, "__xstat64");
}

