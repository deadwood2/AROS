/*
    Copyright © 1995-2011, The AROS Development Team. All rights reserved.
    $Id$

    Desc: (AROS only) Graphics function CopyRegion()
    Lang: english
*/
#include "graphics_intern.h"
#include <graphics/regions.h>
#include "intregions.h"

/*****************************************************************************

    NAME */
#include <proto/graphics.h>

    AROS_LH1(struct Region *, CopyRegion,

/*  SYNOPSIS */
    AROS_LHA(struct Region *, region, A0),

/*  LOCATION */
    struct GfxBase *, GfxBase, 188, Graphics)

/*  FUNCTION
    	Make a copy of the given Region.

    INPUTS
	region - pointer to a Region structure

    RESULT
	the copy of the Region, or NULL if not enough memory.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	13-12-2000  stegerg implemented

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    struct Region *nreg = NewRegion();
    
    if (nreg)
    {
    	if (OrRegionRegion(region, nreg))
    	    return nreg;

        DisposeRegion(nreg);
    }
    
    return NULL;

    AROS_LIBFUNC_EXIT

} /* CopyRegion */
