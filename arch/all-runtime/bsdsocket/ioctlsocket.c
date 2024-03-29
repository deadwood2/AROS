/*
    Copyright (C) 2020-2024, The AROS Development Team. All rights reserved.
*/

#include <sys/ioctl.h>

#include "forwarders_support.h"

/*****************************************************************************

    NAME */
#include <proto/socket.h>

        AROS_LH3(int, IoctlSocket,

/*  SYNOPSIS */
        AROS_LHA(int,           s,       D0),
        AROS_LHA(unsigned long, request, D1),
        AROS_LHA(char *,        argp,    A0),

/*  LOCATION */
        struct SocketBase *, SocketBase, 19, BSDSocket)

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
    {
        s = __fs_translate_socket(s);
        request = __fs_translate_ioctl_request(request);
    }

    return ioctl(s, request, argp);

    AROS_LIBFUNC_EXIT
} /* IoctlSocket */

