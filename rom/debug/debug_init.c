/*
    Copyright (C) 1995-2013, The AROS Development Team. All rights reserved.

    Desc:
*/

#include <aros/config.h>
#include <aros/debug.h>
#include <aros/kernel.h>
#include <aros/symbolsets.h>
#include <proto/exec.h>
#include <proto/kernel.h>

#include <inttypes.h>

#include "debug_intern.h"

/*
 * A horrible hack. It works only under UNIX.
 * TODO: rewrite all this crap and provide C subroutines with well
 * defined API to call them from gdb for debug info lookup. This would
 * remove a requirement to rewrite _gdbinit every time when something
 * changes in this library.
 */
#ifndef HOST_OS_unix
#undef AROS_MODULES_DEBUG
#endif

#if AROS_MODULES_DEBUG
#include "../../arch/all-unix/kernel/hostinterface.h"
#endif

struct MinList *Debug_ModList = NULL;

static int Debug_Init(struct DebugBase *DebugBase)
{
    struct TagItem *bootMsg;
    struct ELF_ModuleInfo *kmod;
#if AROS_MODULES_DEBUG
    struct HostInterface *HostIFace;
#endif

    NEWLIST(&DebugBase->db_Modules);
    InitSemaphore(&DebugBase->db_ModSem);

#if AROS_MODULES_DEBUG
    Debug_ModList = &DebugBase->db_Modules;
#endif

    D(bug("[Debug] Debug_Init() done\n"));
    return 1;
}

ADD2INITLIB(Debug_Init, 0)
