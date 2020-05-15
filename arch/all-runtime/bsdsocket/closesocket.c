/*
    Copyright © 2020, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <unistd.h>

/*****************************************************************************

    NAME */
#include <proto/socket.h>

        AROS_LH1(int, CloseSocket,

/*  SYNOPSIS */
        AROS_LHA(int, s, D0),

/*  LOCATION */
        struct Library *, SocketBase, 20, BSDSocket)

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

    return close(s);

    AROS_LIBFUNC_EXIT
} /* CloseSocket */

