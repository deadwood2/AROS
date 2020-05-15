/*
    Copyright © 2020, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/debug.h>

/*****************************************************************************

    NAME */
#include <proto/socket.h>

        AROS_LH1(ULONG, SocketBaseTagList,

/*  SYNOPSIS */
        AROS_LHA(struct TagItem *, tagList, A0),

/*  LOCATION */
        struct Library *, SocketBase, 49, BSDSocket)

/*  FUNCTION

    INPUTS

    RESULT

    TAGS

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY


*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    (bug("%s is not implemented. Please submit issuet at https://github.com/deadw00d/AROS/issues.\n", __FUNCTION__));
    return 0;

    AROS_LIBFUNC_EXIT
} /* SocketBaseTagList */

