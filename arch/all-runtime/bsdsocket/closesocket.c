/*
    Copyright (C) 2020-2024, The AROS Development Team. All rights reserved.
*/

#include <unistd.h>

#include "forwarders_support.h"

/*****************************************************************************

    NAME */
#include <proto/socket.h>

        AROS_LH1(int, CloseSocket,

/*  SYNOPSIS */
        AROS_LHA(int, s, D0),

/*  LOCATION */
        struct SocketBase *, SocketBase, 20, BSDSocket)

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

    if (SocketBase->sb_Flags & SB_FLAG_CLIENT_IS_AROS_PROGRAM)
        s =__fs_release_mapping(s);

    return close(s);

    AROS_LIBFUNC_EXIT
} /* CloseSocket */

