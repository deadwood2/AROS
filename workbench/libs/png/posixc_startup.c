/*
    Copyright © 2010-2013, The AROS Development Team. All rights reserved.
    $Id$

*/

#include <aros/symbolsets.h>
#include <proto/stdc.h>
#include <libraries/posixc.h>

static int __posixc_startup(void)
{
    struct PosixCBase *PosixCBase = __aros_getbase_PosixCBase();

    PosixCBase->StdCBase = __aros_getbase_StdCBase();

    return PosixCBase->StdCBase != NULL;
}

ADD2OPENLIB(__posixc_startup, -50);
