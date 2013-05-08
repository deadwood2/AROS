/*
    Copyright © 1995-2011, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Graphics function NewRectRegion()
    Lang: english
*/
#include "graphics_intern.h"
#include <graphics/regions.h>
#include <clib/macros.h>
#include "intregions.h"

/*****************************************************************************

    NAME */
#include <proto/graphics.h>

    AROS_LH4(struct Region *, NewRectRegion,

/*  SYNOPSIS */
        AROS_LHA(WORD, MinX, D0),
    AROS_LHA(WORD, MinY, D1),
    AROS_LHA(WORD, MaxX, D2),
    AROS_LHA(WORD, MaxY, D3),

/*  LOCATION */
    struct GfxBase *, GfxBase, 194, Graphics)

/*  FUNCTION
    	Creates a new rectangular Region
		
    INPUTS
    	MinX, MinY, MaxX, MaxY - The extent of the Rect
		
    RESULT
    	Pointer to the newly created Region. NULL on failure.

    NOTES
	This function is a shorthand for:

	    struct Rectangle rect;
	    struct Region *region;

	    rect.MinX = MinX;
	    rect.MinY = MinY;
	    rect.MaxX = MaxX;
	    rect.MaxY = MaxY;

	    region = NewRegion();
	    OrRectRegion(region, &rect);

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	15-12-2000  stegerg implemented

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    struct Region *region = NewRegion();
    
    if (region)
    {
    	struct Rectangle rect = {MinX, MinY, MaxX, MaxY};
    	BOOL res = OrRectRegion(region, &rect);

	if (res)
	    return region;

	DisposeRegion(region);
    }

    return NULL;

    AROS_LIBFUNC_EXIT

} /* NewRectRegion */
