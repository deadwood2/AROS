/*
    Copyright © 1995-2011, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: english
*/

#include <dos/dosextens.h>
#include <proto/exec.h>

/*****************************************************************************

    NAME */
#include <proto/dos.h>

	AROS_LH0(BPTR, ErrorOutput,

/*  SYNOPSIS */

/*  LOCATION */
	struct DosLibrary *, DOSBase, 142, Dos)

/*  FUNCTION
	Returns the current error stream or 0 if there is no current
	input stream.

    INPUTS

    RESULT
	Error stream handle.

    NOTES
	This function is source-compatible with AmigaOS v4.

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    /* Get pointer to process structure */
    struct Process *me=(struct Process *)FindTask(NULL);

    /* Nothing spectacular */
    return me->pr_CES;

    AROS_LIBFUNC_EXIT
}
