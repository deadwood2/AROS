/*
    Copyright © 2020, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "internal.h"
#include "libc_funcs.h"

__attribute__((visibility("default"))) int __xstat64(int ver, char * path, void * buf)
{
    int _return = 0;
    char lpathname[1024];

    __shims_amiga2host(path, lpathname);

    _return = __libcfuncs.__xstat64(ver, lpathname, buf);

    return _return;
}

