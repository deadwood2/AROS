/*
    Copyright (C) 2020, The AROS Development Team. All rights reserved.
*/

#include "internal.h"
#include "libc_funcs.h"

/* open64() defined in startup.o */

__attribute__((visibility("default"))) int __shims_libc_open64(const char * pathname, int oflag, ...)
{
    int _return = 0;
    char lpathname[1024];

    __shims_amiga2host("open64", pathname, lpathname);

    _return = __libcfuncs.open64(lpathname, oflag);

    return _return;
}
