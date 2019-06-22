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
#include <sys/mman.h>

//#define COMP_32BIT

#ifdef COMP_32BIT
static BOOL installed = FALSE;
static APTR space = NULL;
#endif

APTR nommu_AllocMem(IPTR byteSize, ULONG flags, struct TraceLocation *loc, struct ExecBase *SysBase)
{
    APTR res = NULL;

#ifdef COMP_32BIT
    if (!installed)
    {
        size_t len = 1 << 30;
        space = mmap((APTR)0x1000, len, PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED|MAP_32BIT, -1, 0);
        installed = TRUE;
    }
#endif


    if (flags & MEMF_31BIT)
    {
        res = mmap(NULL, byteSize, PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED|MAP_32BIT, -1, 0);
    }
    else
    {
#ifdef COMP_32BIT
        res = space;
        space += byteSize + 128;
        space = (APTR)((IPTR)space & (~0x3));
#else
        res = malloc(byteSize);
#endif
    }

    if (flags & MEMF_CLEAR)
        memset(res, 0, byteSize);

    return res;
}

APTR nommu_AllocAbs(APTR location, IPTR byteSize, struct ExecBase *SysBase)
{
    bug("AxRuntime does not support AllocAbs(nommu_AllocAbs)\n");
    return NULL;
}

void nommu_FreeMem(APTR memoryBlock, IPTR byteSize, struct TraceLocation *loc, struct ExecBase *SysBase)
{
#ifdef COMP_32BIT
#else
    if (memoryBlock < (APTR)0x100000000)
        return;

    free(memoryBlock);
#endif
}

IPTR nommu_AvailMem(ULONG attributes, struct ExecBase *SysBase)
{
    // TODO: this just shows total memory, not available
    IPTR _return;

    IPTR    pages       = sysconf(_SC_PHYS_PAGES);
    ULONG   page_size   = sysconf(_SC_PAGE_SIZE);

    _return = pages * page_size;
#if COMP_32BIT
    // Limit reported value to 1 GB
    if (_return > 1 << 30) _return = 1 << 30;
#endif
    return _return;
}

