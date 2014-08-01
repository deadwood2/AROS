/*
    Copyright © 2010-2013, The AROS Development Team. All rights reserved.
    $Id$

*/

#include <aros/symbolsets.h>
#include <proto/stdc.h>
#include <libraries/stdcio.h>

static int __stdcio_startup(void)
{
    struct StdCIOBase *StdCIOBase = __aros_getbase_StdCIOBase();

    StdCIOBase->StdCBase = __aros_getbase_StdCBase();

    return StdCIOBase->StdCBase != NULL;
}

ADD2OPENLIB(__stdcio_startup, -50);
