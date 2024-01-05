/*
    Copyright (C) 2024, The AROS Development Team. All rights reserved.
*/

#ifndef _SOCKET_BASE_H
#define _SOCKET_BASE_H

#include <exec/libraries.h>

struct SocketBase
{
    struct Library  sb_Base;
    int             sb_Errno;       /* default location of errnoptr bufffer */
    int            *sb_ErrnoPtr;
    ULONG           sb_Flags;
};


#define SB_FLAG_CLIENT_IS_AROS_PROGRAM      0x1

#endif
