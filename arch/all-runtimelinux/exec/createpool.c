/*
    Copyright (C) 1995-2014, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Create a memory pool.
    Lang: english
*/

#include <aros/kernel.h>
#include <aros/libcall.h>
#include <clib/alib_protos.h>

#include "exec_intern.h"
#include "exec_util.h"
#include "memory.h"
#include "mungwall.h"

/*****************************************************************************

    NAME */
#include <exec/memory.h>
#include <exec/memheaderext.h>
#include <proto/exec.h>

        AROS_LH3(APTR, CreatePool,

/*  SYNOPSIS */
        AROS_LHA(ULONG, requirements, D0),
        AROS_LHA(IPTR, puddleSize,   D1),
        AROS_LHA(IPTR, threshSize,   D2),

/*  LOCATION */
        struct ExecBase *, SysBase, 116, Exec)

/*  FUNCTION
        Create a private pool for memory allocations.

    INPUTS
        requirements - The type of the memory
        puddleSize   - The number of bytes that the pool expands by
                   if it is too small.
        threshSize   - Allocations beyond the threshSize are given
                   directly to the system. threshSize must be
                   smaller than or equal to the puddleSize.

    RESULT
        A handle for the memory pool or NULL if the pool couldn't
        be created

    NOTES
        Since exec.library v41.12, the implementation of pools has been
        rewritten to make use of memory protection capabilities. The
        threshSize parameter is effectively ignored and is present only
        for backwards compatibility.

    EXAMPLE
        \* Get the handle to a private memory pool *\
        po=CreatePool(MEMF_ANY,16384,8192);
        if(po!=NULL)
        {
            \* Use the pool *\
            UBYTE *mem1,*mem2;
            mem1=AllocPooled(po,1000);
            mem2=AllocPooled(po,2000);
            \* Do something with the memory... *\

            \* Free everything at once *\
            DeletePool(po);
        }

    BUGS

    SEE ALSO
        DeletePool(), AllocPooled(), FreePooled()

    INTERNALS

******************************************************************************/
{
    AROS_LIBFUNC_INIT

    struct ProtectedPool *pool = nommu_AllocMem(sizeof(struct ProtectedPool), MEMF_CLEAR, NULL, NULL);
    pool->pool.Requirements = requirements;
    return pool;

    AROS_LIBFUNC_EXIT
} /* CreatePool */
