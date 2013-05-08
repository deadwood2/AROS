/*
    Copyright © 1995-2011, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Graphics function AndRectRect()
    Lang: english
*/

#include <aros/debug.h>
#include "graphics_intern.h"
#include <graphics/regions.h>
#include <clib/macros.h>

/*****************************************************************************

    NAME */
#include <clib/graphics_protos.h>

    AROS_LH3(BOOL, AndRectRect,

/*  SYNOPSIS */
        AROS_LHA(struct Rectangle *, rect1, A0),
    AROS_LHA(struct Rectangle *, rect2, A1),
    AROS_LHA(struct Rectangle *, intersect, A2),

/*  LOCATION */
    struct GfxBase *, GfxBase, 193, Graphics)

/*  FUNCTION
    	Calculate the intersection rectangle between the
	given Rectangle rect1 and the given Rectangle rect2
	leaving the result in intersect (if intersect != NULL).

    INPUTS
    	rect1 - pointer to 1st Rectangle
	rect2 - pointer to 2nd Rectangle
	intersect - pointer to rectangle which will hold result.

    RESULT
	TRUE if rect1 and rect2 do intersect. In this case intersect
	will contain the intersection rectangle.

	FALSE if rect1 and rect2 do not overlap. "intersect" will
	then be left unchanged.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	15-12-2000  stegerg implemented

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    WORD MinX = MAX(rect1->MinX, rect2->MinX);
    WORD MinY = MAX(rect1->MinY, rect2->MinY);
    WORD MaxX = MIN(rect1->MaxX, rect2->MaxX);
    WORD MaxY = MIN(rect1->MaxY, rect2->MaxY);

    if ((MinX > MaxX) || (MinY > MaxY))
        return FALSE;
    else
    {
        if (intersect) {
            intersect->MinX = MinX;
            intersect->MinY = MinY;
            intersect->MaxX = MaxX;
            intersect->MaxY = MaxY;
        }

        return TRUE;
    }

    AROS_LIBFUNC_EXIT

} /* AndRectRect */
