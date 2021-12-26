/*
    Copyright (C) 1995-2017, The AROS Development Team. All rights reserved.

    Desc:
*/

#include <stdarg.h>
#include <exec/types.h>
#include <aros/libcall.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/single.h>
#include <proto/peropener.h>

AROS_LH3(LONG, UseRelAdd2,
    AROS_LHA(LONG, base, D0),
    AROS_LHA(LONG,    a, D1),
    AROS_LHA(LONG,    b, D2),
    struct UseRelBase *, UseRelBase, 5, UseRel
)
{
    AROS_LIBFUNC_INIT

    RegSetValue(base);
    return RegAdd4(a, b, 0, 0);

    AROS_LIBFUNC_EXIT
}

LONG UseRelAdd5(LONG base, LONG a, LONG b, LONG c, LONG d, LONG e)
{
    StackSetValue(base);
    return StackAdd4OrMore(5, a, b, c, d, e);
}

#include "peropenervalue.h"

void SetPeropenerLibraryValue(int v)
{
    peropenervalue = v;
}

int GetPeropenerLibraryValue()
{
    return PeropenerGetValueReg();
}