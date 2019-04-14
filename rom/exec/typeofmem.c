/*
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Examine memory
    Lang: english
*/

#include <aros/debug.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <aros/libcall.h>
#include <proto/exec.h>

#include "memory.h"

/*****************************************************************************

    NAME */

	AROS_LH1(ULONG, TypeOfMem,

/*  SYNOPSIS */
	AROS_LHA(APTR, address, A1),

/*  LOCATION */
	struct ExecBase *, SysBase, 89, Exec)

/*  FUNCTION
	Return type of memory at a given address or 0 if there is no memory
	there.

    INPUTS
	address - Address to test

    RESULT
	The memory flags you would give to AllocMem().

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
    AROS_LIBFUNC_INIT

    return MEMF_PUBLIC;

    AROS_LIBFUNC_EXIT
} /* TypeOfMem */

