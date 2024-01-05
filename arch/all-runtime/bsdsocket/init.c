/*
    Copyright (C) 2020, The AROS Development Team. All rights reserved.
*/

#include <aros/symbolsets.h>
#include <proto/exec.h>

#include "bsdsocket_libdefs.h"

struct DosLibrary *DOSBase = NULL;
struct UtilityBase *UtilityBase = NULL;

static int BSDSocket_Init(struct SocketBase *SocketBase)
{
    if (!DOSBase) DOSBase = (struct DosLibrary *)OpenLibrary("dos.library", 0L);
    if (!UtilityBase) UtilityBase = (struct UtilityBase *)OpenLibrary("utility.library", 0L);

    SocketBase->sb_ErrnoPtr     = &SocketBase->sb_Errno;
    SocketBase->sb_Flags        = 0;

    return 1;
}

static int BSDSocket_Expunge(struct SocketBase *SocketBase)
{
    if (UtilityBase) CloseLibrary((struct Library *)UtilityBase);
    if (DOSBase) CloseLibrary((struct Library *)DOSBase);
    return 1;
}

ADD2INITLIB(BSDSocket_Init, 0);
ADD2EXPUNGELIB(BSDSocket_Expunge, 0);
