/*
    Copyright (C) 2023, The AROS Development Team. All rights reserved.

    Note: to support per opener global variables, each time the library is opened, the program wrapping external
    library code is loaded. This generates new segment each time which allows external library global variables
    to be per opener.
*/

#include <aros/symbolsets.h>
#include <proto/dos.h>

#include "peropenerbase.h"


int OpenLib(struct PeropenerBase *PeropenerBase)
{
    PeropenerBase->value = 3;
    PeropenerBase->extlibseg = LoadSeg("extlib1.seg");
    if (PeropenerBase->extlibseg != BNULL)
    {
        APTR (*init)() = (APTR)((BPTR *)BADDR(PeropenerBase->extlibseg) + 1);
        PeropenerBase->jumptable = init();
        return 1;
    }
    return 0;
}

void CloseLib(struct PeropenerBase *PeropenerBase)
{
    if (PeropenerBase->extlibseg != BNULL)
        UnLoadSeg(PeropenerBase->extlibseg);
}

ADD2OPENLIB(OpenLib, 0);
ADD2CLOSELIB(CloseLib, 0);
