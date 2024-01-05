/*
    Copyright (C) 2024, The AROS Development Team. All rights reserved.
*/

#include <aros/debug.h>

#include "forwarders_support.h"
#include <errno.h>

void __fs_translate_errno(int unix_errno, struct SocketBase *SocketBase)
{
    switch(unix_errno)
    {

    case(EAGAIN):
        *SocketBase->sb_ErrnoPtr = 35;
        break;
    case(EINPROGRESS):
        *SocketBase->sb_ErrnoPtr = 36;
        break;
    default:
        bug("<<WARN>>: %s. Translation for errno(%d) is not implemented. Please submit issuet at https://github.com/deadw00d/AROS/issues.\n",
            __FUNCTION__, unix_errno);
    }
}
