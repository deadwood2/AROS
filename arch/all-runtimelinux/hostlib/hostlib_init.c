/*
    Copyright © 1995-2014, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/debug.h>
#include <aros/kernel.h>
#include <aros/symbolsets.h>
#include <utility/tagitem.h>
#include <proto/arossupport.h>
#include <proto/exec.h>
#include <proto/kernel.h>

#include "hostlib_intern.h"

static int HostLib_Init(struct HostLibBase *HostLibBase)
{
#ifndef USE_FORBID_LOCK
    InitSemaphore(&HostLibBase->HostSem);
#endif
    return TRUE;
}

ADD2INITLIB(HostLib_Init, 0)
