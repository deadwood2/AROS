/*
    Copyright (C) 1995-2014, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Delete a memory pool including all its memory.
    Lang: english
*/

#include <aros/libcall.h>
#include <exec/memory.h>
#include <exec/memheaderext.h>
#include <proto/exec.h>

#include "exec_intern.h"
#include "exec_util.h"
#include "memory.h"
#include "mungwall.h"

/*****************************************************************************

    NAME */

        AROS_LH1(void, DeletePool,

/*  SYNOPSIS */
        AROS_LHA(APTR, poolHeader, A0),

/*  LOCATION */
        struct ExecBase *, SysBase, 117, Exec)

/*  FUNCTION
        Delete a pool including all its memory.

    INPUTS
        poolHeader - The pool allocated with CreatePool() or NULL.

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
        CreatePool(), AllocPooled(), FreePooled(), AllocVecPooled(),
        FreeVecPooled()

    INTERNALS

******************************************************************************/
{
    AROS_LIBFUNC_INIT

    ASSERT_VALID_PTR_OR_NULL(poolHeader);

    nommu_FreeMem(poolHeader, 0, NULL, NULL);

    AROS_LIBFUNC_EXIT
} /* DeletePool */
