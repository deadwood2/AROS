/*
    Copyright (C) 1995-2023, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>
#include "peropenerbase.h"

struct Library * __aros_getbase_PeropenerBase();

void PeropenerSetValueStack(int value)
{
    struct PeropenerBase *PeropenerBase = (struct PeropenerBase *)__aros_getbase_PeropenerBase();

    PeropenerBase->value = value;
}


int PeropenerGetValueStack(void)
{
    struct PeropenerBase *PeropenerBase = (struct PeropenerBase *)__aros_getbase_PeropenerBase();

    return PeropenerBase->value;
}

AROS_LH1(void, PeropenerSetValueReg,
AROS_LHA(int, value, D0),
struct PeropenerBase *, PeropenerBase, 7, Peropener)
{
    AROS_LIBFUNC_INIT

    PeropenerBase->value = value;

    AROS_LIBFUNC_EXIT
}

AROS_LH0(int, PeropenerGetValueReg,
struct PeropenerBase *, PeropenerBase, 8, Peropener)
{
    AROS_LIBFUNC_INIT

    return PeropenerBase->value;

    AROS_LIBFUNC_EXIT
}

#include "peropenervalue_extlibseg.h"

AROS_LH1(void, PeropenerSetGlobalPeropenerValueReg,
AROS_LHA(int, value, D0),
struct PeropenerBase *, PeropenerBase, 11, Peropener)
{
    AROS_LIBFUNC_INIT

    ((struct ExtLibSegJumpTable *)(PeropenerBase->jumptable))->SetGlobalValue(value);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(int, PeropenerGetGlobalPeropenerValueReg,
struct PeropenerBase *, PeropenerBase, 12, Peropener)
{
    AROS_LIBFUNC_INIT

    return ((struct ExtLibSegJumpTable *)(PeropenerBase->jumptable))->GetGlobalValue();

    AROS_LIBFUNC_EXIT
}
