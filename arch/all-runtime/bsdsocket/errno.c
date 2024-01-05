/*
    Copyright (C) 2024, The AROS Development Team. All rights reserved.
*/

#include <aros/debug.h>

#include "socketbase.h"


/*****************************************************************************

    NAME */
#include <proto/socket.h>

        AROS_LH0(LONG, Errno,

/*  SYNOPSIS */

/*  LOCATION */
        struct SocketBase *, SocketBase, 27, BSDSocket)

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

    return *SocketBase->sb_ErrnoPtr;

    AROS_LIBFUNC_EXIT
} /* SocketBaseTagList */

