/*
    Copyright © 2012, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <sys/arosc.h>
#include <aros/symbolsets.h>

#include <proto/exec.h>

const unsigned short int * const *__ctype_b_ptr = NULL;
/* ABI_V0 compatibility */
const int * const *__ctype_toupper_ptr = NULL;
const int * const *__ctype_tolower_ptr = NULL;

struct aroscbase * __aros_getbase_aroscbase();

static int __ctype_init(struct ExecBase *SysBase)
{
    const struct arosc_ctype *ctype;

    if (!__aros_getbase_aroscbase())
        return 0;

    ctype = __get_arosc_ctype();

    __ctype_b_ptr = &ctype->b;
    __ctype_toupper_ptr = &ctype->toupper;
    __ctype_tolower_ptr = &ctype->tolower;

    return 1;
}

ADD2INIT(__ctype_init, 20);
