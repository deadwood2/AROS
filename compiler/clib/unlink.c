/*
    Copyright © 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

    Delete a file.
*/

#include <stdio.h>

/*****************************************************************************

    NAME */
#include <unistd.h>

	int unlink (

/*  SYNOPSIS */
	const char * pathname)

/*  FUNCTION
	Delete a file from disk.

    INPUTS
	pathname - Complete path to the file

    RESULT
	0 on success and -1 on error. In case of an error, errno is set.

    NOTES
    	Identical to remove

    EXAMPLE
	// Delete the file xyz in the current directory
	unlink ("xyz");

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
    return remove(pathname);
} /* unlink */
