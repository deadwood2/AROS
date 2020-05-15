/*
    Copyright © 2020, The AROS Development Team. All rights reserved.
    $Id$
*/

/*****************************************************************************

    NAME */
#include <proto/socket.h>

        AROS_LH4(LONG, ObtainSocket,

/*  SYNOPSIS */
        AROS_LHA(LONG, id      , D0),
        AROS_LHA(LONG, domain  , D2),
        AROS_LHA(LONG, type    , D3),
        AROS_LHA(LONG, protocol, D4),

/*  LOCATION */
        struct Library *, SocketBase, 24, BSDSocket)

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

    return id;

    AROS_LIBFUNC_EXIT
} /* ObstainSocket */

