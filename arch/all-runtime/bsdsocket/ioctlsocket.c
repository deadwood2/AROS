/*
    Copyright © 2020, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <sys/ioctl.h>

/*****************************************************************************

    NAME */
#include <proto/socket.h>

        AROS_LH3(int, IoctlSocket,

/*  SYNOPSIS */
        AROS_LHA(int,           s,       D0),
        AROS_LHA(unsigned long, request, D1),
        AROS_LHA(char *,        argp,    A0),

/*  LOCATION */
        struct Library *, SocketBase, 19, BSDSocket)

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

    return ioctl(s, request, argp);

    AROS_LIBFUNC_EXIT
} /* IoctlSocket */

