/*
    Copyright (C) 2020-2024, The AROS Development Team. All rights reserved.
*/

#include <proto/utility.h>
#include <aros/debug.h>

#include "socketbase.h"

/*****************************************************************************

    NAME */
#include <proto/socket.h>

        AROS_LH1(ULONG, SocketBaseTagList,

/*  SYNOPSIS */
        AROS_LHA(struct TagItem *, tagList, A0),

/*  LOCATION */
        struct SocketBase *, SocketBase, 49, BSDSocket)

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
    struct TagItem *tag;

    while((tag = NextTagItem ((struct TagItem **)&tagList)))
    {
        switch(tag->ti_Tag)
        {
        case(0x80000031):
            SocketBase->sb_ErrnoPtr = (int *)tag->ti_Data;
            break;
        default:
            (bug("%s is not implemented for Tag(0x%x). Please submit issuet at https://github.com/deadw00d/AROS/issues.\n",
                __FUNCTION__, tag->ti_Tag));
        }
    }

    return 0;

    AROS_LIBFUNC_EXIT
} /* SocketBaseTagList */

