/*
    Copyright � 1995-2011, The AROS Development Team. All rights reserved.
    $Id$

    Desc: System memory allocator for MMU-less systems.
          Used also as boot-time memory allocator on systems with MMU.
    Lang: english
*/

#include <aros/debug.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <exec/memheaderext.h>
#include <proto/exec.h>

#include <string.h>

#include "exec_intern.h"
#include "exec_util.h"
#include "memory.h"

#include <stdlib.h>
#include <unistd.h>

APTR nommu_AllocMem(IPTR byteSize, ULONG flags, struct TraceLocation *loc, struct ExecBase *SysBase)
{
    APTR res = NULL;
    res = malloc(byteSize);
    if (flags & MEMF_CLEAR)
        memset(res, 0, byteSize);

    return res;
}

APTR nommu_AllocAbs(APTR location, IPTR byteSize, struct ExecBase *SysBase)
{
    bug("AROS RUNTIME does not support AllocAbs(nommu_AllocAbs)\n");
    return NULL;
}

void nommu_FreeMem(APTR memoryBlock, IPTR byteSize, struct TraceLocation *loc, struct ExecBase *SysBase)
{
    free(memoryBlock);
}

IPTR nommu_AvailMem(ULONG attributes, struct ExecBase *SysBase)
{
    // TODO: this just shows total memory, not available
    IPTR    pages       = sysconf(_SC_PHYS_PAGES);
    ULONG   page_size   = sysconf(_SC_PAGE_SIZE);
    return pages * page_size;
}

