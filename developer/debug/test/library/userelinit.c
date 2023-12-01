/*
    Copyright (C) 2023, The AROS Development Team. All rights reserved.

*/

#include <aros/symbolsets.h>
#include <proto/exec.h>
#include <proto/peropener.h>

#include "userelbase.h"

const ULONG __aros_rellib_base_PeropenerBase = 0;
SETRELLIBOFFSET(PeropenerBase, struct UseRelBase, peropenerBase);

int OpenLib(struct UseRelBase *UseRelBase)
{
    struct Library *tmp = NULL;

    UseRelBase->peropenerBase = OpenLibrary("peropener.library", 0L);
    if (UseRelBase->peropenerBase == NULL) return 0;
    UseRelBase->peropenerBaseClose = TRUE;

    tmp = Peropener_GetShareable(UseRelBase->peropenerBase);
    if (tmp != UseRelBase->peropenerBase) /* UseRelBase->peropenerBase must not be used and... */
    {
        CloseLibrary(UseRelBase->peropenerBase);
        UseRelBase->peropenerBaseClose = FALSE;
        if (tmp == NULL) /* ...there is no sharable base available or...*/
            return 0;

        UseRelBase->peropenerBase = tmp; /* ...there is a sharable base that should be used. */
    }

    return 1;
}

void CloseLib(struct UseRelBase *UseRelBase)
{
    if (UseRelBase->peropenerBaseClose)
        CloseLibrary(UseRelBase->peropenerBase);
}

ADD2OPENLIB(OpenLib, 0);
ADD2CLOSELIB(CloseLib, 0);
