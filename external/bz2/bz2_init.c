/*
  Copyright © 2025, The AROS Development Team. All rights reserved.
*/

#define DEBUG 0
#include <exec/types.h>
#include <exec/memory.h>
#include <aros/libcall.h>
#include <aros/debug.h>
#include <proto/exec.h>
#include <proto/alib.h>
#include <proto/dos.h>

#include <stddef.h>

#include <aros/symbolsets.h>
#include LC_LIBDEFS_FILE

const ULONG __aros_rellib_base_CrtBase = 0;
SETRELLIBOFFSET(CrtBase, LIBBASETYPE, crtBase);

struct Library * __modonly_mayget_shareable(struct Library *base);

int _OpenLib(LIBBASETYPEPTR BZ2Base)
{
    struct Library *tmp = NULL;

    BZ2Base->crtBase = OpenLibrary("crt.library", 3L);
    if (BZ2Base->crtBase == NULL) return 0;
    BZ2Base->crtBaseClose = TRUE;

    tmp = __modonly_mayget_shareable(BZ2Base->crtBase);
    if (tmp != BZ2Base->crtBase) /* ReginaBase->crtBase must not be used and... */
    {
        CloseLibrary(BZ2Base->crtBase);
        BZ2Base->crtBaseClose = FALSE;
        if (tmp == NULL) /* ...there is no sharable base available or...*/
            return 0;

        BZ2Base->crtBase = tmp; /* ...there is a sharable base that should be used. */
    }

    return 1;
}

void _CloseLib(LIBBASETYPEPTR BZ2Base)
{
    if (BZ2Base->crtBaseClose)
        CloseLibrary(BZ2Base->crtBase);
}

ADD2OPENLIB(_OpenLib, 0);
ADD2CLOSELIB(_CloseLib, 0);
