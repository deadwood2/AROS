/*
    Copyright © 1995-2012, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Free memory allocated by Allocate().
    Lang: english
*/

#include <aros/debug.h>
#include <exec/execbase.h>
#include <exec/alerts.h>
#include <aros/libcall.h>
#include <exec/memory.h>
#include <exec/memheaderext.h>
#include <proto/exec.h>

#include "exec_util.h"
#include "memory.h"

/*****************************************************************************

    NAME */

        AROS_LH3(void, Deallocate,

/*  SYNOPSIS */
        AROS_LHA(struct MemHeader *, freeList,    A0),
        AROS_LHA(APTR,               memoryBlock, A1),
        AROS_LHA(IPTR,               byteSize,    D0),

/*  LOCATION */
    struct ExecBase *, SysBase, 32, Exec)

/*  FUNCTION
        Free block of memory associated with a given MemHandler structure.

    INPUTS
        freeList    - Pointer to the MemHeader structure
        memoryBlock - Pointer to the memory to be freed
        byteSize    - Size of the block

    RESULT

    NOTES
        The start and end borders of the block are aligned to
        a multiple of sizeof(struct MemChunk) and to include the block.

    EXAMPLE

    BUGS

    SEE ALSO
        Allocate()

    INTERNALS

******************************************************************************/
{
    AROS_LIBFUNC_INIT

    bug("AROS RUNTIME does not support Deallocate\n");

    AROS_LIBFUNC_EXIT
} /* Deallocate */
