/*
    Copyright (C) 2012-2013, The AROS Development Team. All rights reserved.
*/

#include <aros/symbolsets.h>

#include <proto/exec.h>
#include <libraries/crt.h>

const unsigned short int * const *__ctype_b_ptr = NULL;
const unsigned char * const *__ctype_toupper_ptr = NULL;
const unsigned char * const *__ctype_tolower_ptr = NULL;

static struct CrtBase *_CrtBase = NULL;
static int opened = 0;

static int __ctype_init(struct ExecBase *SysBase)
{
    /* Be sure ctype functions may be called from init code.
     * Try to open crt.library manually here
     */
    if (!_CrtBase)
    {
        _CrtBase = (struct CrtBase *)OpenLibrary("crt.library", 0);
        opened = 1;
    }
    if (!_CrtBase)
        return 0;

    __ctype_b_ptr = &_CrtBase->__ctype_b;
    __ctype_toupper_ptr = &_CrtBase->__ctype_toupper;
    __ctype_tolower_ptr = &_CrtBase->__ctype_tolower;

    return 1;
}

static void __ctype_exit(void)
{
    if (opened && _CrtBase)
        CloseLibrary((struct Library *)_CrtBase);
}

ADD2INIT(__ctype_init, 20);
ADD2EXIT(__ctype_exit, 20);
