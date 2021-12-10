/*
    Copyright (C) 2012-2013, The AROS Development Team. All rights reserved.
*/

#include <aros/symbolsets.h>

#include <proto/exec.h>
#include <libraries/crtutil.h>

const unsigned short int * const *__ctype_b_ptr = NULL;
const unsigned char * const *__ctype_toupper_ptr = NULL;
const unsigned char * const *__ctype_tolower_ptr = NULL;

static struct CrtUtilBase *_CrtUtilBase = NULL;
static int opened = 0;

static int __ctype_init(struct ExecBase *SysBase)
{
    /* Be sure ctype functions may be called from init code.
     * Try to open crtutil.library manually here
     */
    if (!_CrtUtilBase)
    {
        _CrtUtilBase = (struct CrtUtilBase *)OpenLibrary("crtutil.library", 0);
        opened = 1;
    }
    if (!_CrtUtilBase)
        return 0;

    __ctype_b_ptr = &_CrtUtilBase->__ctype_b;
    __ctype_toupper_ptr = &_CrtUtilBase->__ctype_toupper;
    __ctype_tolower_ptr = &_CrtUtilBase->__ctype_tolower;

    return 1;
}

static void __ctype_exit(void)
{
    if (opened && _CrtUtilBase)
        CloseLibrary((struct Library *)_CrtUtilBase);
}

ADD2INIT(__ctype_init, 20);
ADD2EXIT(__ctype_exit, 20);
