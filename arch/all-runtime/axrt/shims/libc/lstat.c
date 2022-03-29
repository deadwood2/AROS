/*
    Copyright (C) 2022, The AROS Development Team. All rights reserved.
*/

#include "internal.h"
#include "libc_funcs.h"

__attribute__((visibility("default"))) int __lxstat64(int ver, char * path, void * buf)
{
    int _return = 0;
    char lpathname[1024];

    __shims_amiga2host(__func__, path, lpathname);

    _return = __libcfuncs.__lxstat64(ver, lpathname, buf);

    return _return;
}

