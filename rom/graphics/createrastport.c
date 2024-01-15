/*
    Copyright © 1995-2011, The AROS Development Team. All rights reserved.
    $Id$

    Desc: AROS Graphics function CreateRastPort()
    Lang: english
*/

#include <exec/memory.h>
#include <proto/exec.h>
#include <proto/graphics.h>

/*****************************************************************************

    NAME */
#include <graphics/rastport.h>
#include <proto/graphics.h>

    AROS_LH0(struct RastPort *, CreateRastPort,

/*  SYNOPSIS */

/*  LOCATION */
    struct GfxBase *, GfxBase, 177, Graphics)

/*  FUNCTION
    This function creates a new RastPort.

    INPUTS
    None.

    RESULT
    A pointer to a new RastPort or NULL if there was not enough memory
    to perform the operation.

    NOTES
    This function is AROS specific. For compatibility, there is a
    function in aros.lib which does the same on Amiga.

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
    29-10-95    digulla automatically created from
                graphics_lib.fd and clib/graphics_protos.h

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    struct RastPort *newRP = AllocMem (sizeof (struct RastPort), MEMF_ANY);

    if (newRP)
	InitRastPort(newRP);

    return newRP;

    AROS_LIBFUNC_EXIT
} /* CreateRastPort */
