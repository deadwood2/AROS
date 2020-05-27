/*
    Copyright © 2020, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/symbolsets.h>
#include <proto/exec.h>

#include LC_LIBDEFS_FILE

struct DosLibrary *DOSBase = NULL;

static int BSDSocket_Init(struct Library *SocketBase)
{
    if (!DOSBase) DOSBase = (struct DosLibrary *)OpenLibrary("dos.library", 0L);
    return 1;
}

static int BSDSocket_Expunge(struct Library *SocketBase)
{
    if (DOSBase) CloseLibrary((struct Library *)DOSBase);
    return 1;
}

ADD2INITLIB(BSDSocket_Init, 0);
ADD2EXPUNGELIB(BSDSocket_Expunge, 0);
