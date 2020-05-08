/*
    Copyright © 2020, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "internal.h"
#include "libc_funcs.h"

/* open64() defined in startup.o */

__attribute__((visibility("default"))) int __shims_libc_open64(const char * pathname, int oflag, ...)
{
    int _return = 0;
    char lpathname[1024];

    __shims_amiga2host(pathname, lpathname);

    _return = __libcfuncs.open64(lpathname, oflag);

    return _return;
}

/* Backwards compatibility. Do not remove in ABI 2.0 */
__attribute__((visibility("default"))) int __shim_open64(const char * pathname, int oflag, ...) __attribute__ ((alias ("__shims_libc_open64")));
