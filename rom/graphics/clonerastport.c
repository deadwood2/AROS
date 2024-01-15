/*
    Copyright © 1995-2011, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Graphics function CloneRastPort()
    Lang: english
*/

#include <exec/memory.h>
#include <proto/exec.h>

/*****************************************************************************

    NAME */
#include <graphics/rastport.h>
#include <proto/graphics.h>

    AROS_LH1(struct RastPort *, CloneRastPort,

/*  SYNOPSIS */
    AROS_LHA(struct RastPort *, rp, A1),

/*  LOCATION */
    struct GfxBase *, GfxBase, 178, Graphics)

/*  FUNCTION
    This function creates a copy of a RastPort.

    INPUTS
    rp - The RastPort to clone.

    RESULT
    A pointer to a RastPort with the same attributes as the RastPort
    which was specified or NULL if there was not enough memory to perform
    the operation.

    NOTES
    This function is AROS specific. For compatibility, there is a
    function in aros.lib which does the same on Amiga.

    EXAMPLE

    BUGS

    SEE ALSO
    CreateRastPort(), FreeRastPort()

    INTERNALS

    HISTORY
    29-10-95    digulla automatically created from
                graphics_lib.fd and clib/graphics_protos.h

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    struct RastPort *newRP = AllocMem (sizeof (struct RastPort), MEMF_ANY);

    if (newRP)
	CopyMem (rp, newRP, sizeof (struct RastPort));

    return newRP;

    AROS_LIBFUNC_EXIT

} /* CloneRastPort */
