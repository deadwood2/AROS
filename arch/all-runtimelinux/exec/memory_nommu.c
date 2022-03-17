/*
    Copyright (C) 1995-2011, The AROS Development Team. All rights reserved.

    Desc: System memory allocator for MMU-less systems.
          Used also as boot-time memory allocator on systems with MMU.
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

static BOOL installed   = FALSE;
static APTR space32bit  = NULL;
static char *nextpage   = NULL;
#define SPACE32SIZE     (1 << 27) /* 128 MB */
/* Notes: implementation of MEMF_31BIT is good enough to support loading ELF objects into < 2GB address
   range a few times without restart but nothing more. Page memory is not re-used, so it can be
   exhaused*/

APTR nommu_AllocMem(IPTR byteSize, ULONG flags, struct TraceLocation *loc, struct ExecBase *SysBase)
{
    APTR res = NULL;

    if ((flags & MEMF_31BIT) && !installed)
    {
        size_t len = SPACE32SIZE;
        space32bit = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED | MAP_32BIT, -1, 0);
        nextpage = space32bit;
        installed = TRUE;
    }


    if (flags & MEMF_31BIT)
    {
        /* Always allocate full pages for now. Used together with internalloadseg_elf */
        LONG pagecount = AROS_ROUNDUP2(byteSize, 4096) / 4096;
        if (nextpage + pagecount * 4096 > (char *)space32bit + SPACE32SIZE)
            return NULL;

        res = nextpage;
        nextpage += pagecount * 4096;
    }
    else
    {
        res = malloc(byteSize);
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
    if (memoryBlock >= space32bit && memoryBlock < space32bit + SPACE32SIZE)
        return;

    free(memoryBlock);
}

IPTR nommu_AvailMem(ULONG attributes, struct ExecBase *SysBase)
{
    // TODO: this just shows total memory, not available
    IPTR _return;

    IPTR    pages       = sysconf(_SC_PHYS_PAGES);
    ULONG   page_size   = sysconf(_SC_PAGE_SIZE);

    _return = pages * page_size;

    return _return;
}

