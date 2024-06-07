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

static struct MemHeader *mh31bit = NULL;
#define SPACE32SIZE     (1 << 28) /* 256 MB */

/* Note: MEMF_31BIT is used for:
    a) loading AROS x86_64 ELF objects. Code resides in 31-bit spaces, stack and heap is normal 64-bit memory.
    b) loading AROS i386 ELF objects. Code resisede in 31-bit space.
    c) stack and heap for AROS i386 ELF objects

   Using standard memory header mechanism should be good enought for now
*/
/* Note on protection flags:
   Because of usage b) above all of MEMF_31BIT is market as PROT_EXEC. When there is system-side function
   which loads/unloads both i386 and x86_64 AROS ELF objects, it will be possible to map pages in that
   function as PROT_EXEC and do only PROT_READ | PROT_WRITE here
*/

APTR nommu_AllocMem(IPTR byteSize, ULONG flags, struct TraceLocation *loc, struct ExecBase *SysBase)
{
    APTR res = NULL;

    if ((flags & MEMF_31BIT) && mh31bit == NULL)
    {
        size_t len = SPACE32SIZE;
        APTR p = mmap(NULL, len, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANON | MAP_SHARED | MAP_32BIT, -1, 0);

        mh31bit = p;
        mh31bit->mh_Node.ln_Succ    = NULL;
        mh31bit->mh_Node.ln_Pred    = NULL;
        mh31bit->mh_Node.ln_Type    = NT_MEMORY;
        mh31bit->mh_Node.ln_Name    = (STRPTR)"CHIP RAM";
        mh31bit->mh_Node.ln_Pri     = 0;
        mh31bit->mh_Attributes      = MEMF_31BIT | MEMF_24BITDMA | MEMF_CHIP;

        /* The first MemChunk needs to be aligned. We do it by adding MEMHEADER_TOTAL. */
        mh31bit->mh_First           = p + MEMHEADER_TOTAL;
        mh31bit->mh_First->mc_Next  = NULL;
        mh31bit->mh_First->mc_Bytes = len - MEMHEADER_TOTAL;

        /*
        * mh_Lower and mh_Upper are informational only. Since our MemHeader resides
        * inside the region it describes, the region includes MemHeader.
        */
        mh31bit->mh_Lower           = p;
        mh31bit->mh_Upper           = p + len;
        mh31bit->mh_Free            = mh31bit->mh_First->mc_Bytes;
    }


    if (flags & MEMF_31BIT)
    {
        res = stdAlloc(mh31bit, /*mhac_GetSysCtx(mh, SysBase)*/ NULL, byteSize, flags, loc, SysBase);
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
    if (memoryBlock >= (APTR)mh31bit && memoryBlock < (APTR)mh31bit + SPACE32SIZE)
    {
        stdDealloc(mh31bit, NULL /*mhac_GetSysCtx(mh, SysBase)*/, memoryBlock, byteSize, loc, SysBase);
    }
    else
    {
        free(memoryBlock);
    }
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

