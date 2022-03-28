/*
    Copyright (C) 2020, The AROS Development Team. All rights reserved.
*/

#include "internal.h"
#include "libc_funcs.h"

__attribute__((visibility("default"))) int mkdir(const char * path, mode_t mode)
{
    int _return = 0;
    char lpathname[1024];

    __shims_amiga2host(__func__, path, lpathname);

    _return = __libcfuncs.mkdir(lpathname, mode);

    return _return;
}

