/*
    Copyright (C) 1995-2017, The AROS Development Team. All rights reserved.

    Desc:
*/

#include <stdarg.h>
#include <exec/types.h>
#include <aros/libcall.h>
#include <proto/dos.h>
#include <proto/exec.h>

#include LC_LIBDEFS_FILE

#include "singlebase.h"

AROS_LH1(void, RegSetValue,
    AROS_LHA(LONG, v, D0),
    struct SingleBase *, SingleBase, 5, Single
)
{
    AROS_LIBFUNC_INIT

    SingleBase->value = v;

    AROS_LIBFUNC_EXIT
}

AROS_LH4(LONG, RegAdd4,
    AROS_LHA(LONG, a1, D0),
    AROS_LHA(LONG, a2, D1),
    AROS_LHA(LONG, a3, D2),
    AROS_LHA(LONG, a4, D3),
    struct SingleBase *, SingleBase, 6, Single
)
{
    AROS_LIBFUNC_INIT

    return SingleBase->value + a1 + a2 + a3 + a4;
    
    AROS_LIBFUNC_EXIT
}

AROS_LH6(LONG, RegAdd6,
    AROS_LHA(LONG, a1, D0),
    AROS_LHA(LONG, a2, D1),
    AROS_LHA(LONG, a3, D2),
    AROS_LHA(LONG, a4, D3),
    AROS_LHA(LONG, a5, D4),
    AROS_LHA(LONG, a6, D5),
    struct SingleBase *, SingleBase, 7, Single
)
{
    AROS_LIBFUNC_INIT

    return SingleBase->value + a1 + a2 + a3 + a4 + a5 + a6;
    
    AROS_LIBFUNC_EXIT
}

AROS_LH9(LONG, RegAdd9,
    AROS_LHA(LONG, a1, D0),
    AROS_LHA(LONG, a2, D1),
    AROS_LHA(LONG, a3, D2),
    AROS_LHA(LONG, a4, D3),
    AROS_LHA(LONG, a5, D4),
    AROS_LHA(LONG, a6, D5),
    AROS_LHA(LONG, a7, D6),
    AROS_LHA(LONG, a8, D7),
    AROS_LHA(LONG, a9, A0),
    struct SingleBase *, SingleBase, 8, Single
)
{
    AROS_LIBFUNC_INIT

    return SingleBase->value + a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8 + a9;
    
    AROS_LIBFUNC_EXIT
}

AROS_LH0(double, RegReturnMinusPi,
    struct SingleBase *, SingleBase, 9, Single
)
{
    AROS_LIBFUNC_INIT

    return -3.14;
    
    AROS_LIBFUNC_EXIT
}

AROS_LHDOUBLE2(double, RegAddDouble2,
    AROS_LHA2(double, a1, D0, D1),
    AROS_LHA2(double, a2, D2, D3),
    struct SingleBase *, SingleBase, 10, Single
)
{
    AROS_LIBFUNC_INIT

    return SingleBase->value + a1 + a2;

    AROS_LIBFUNC_EXIT
}

AROS_LH3QUAD1(QUAD, RegAdd3Quad1,
    AROS_LHA(LONG, a1, D0),
    AROS_LHA(LONG, a2, D1),
    AROS_LHA(LONG, a3, D2),
    AROS_LHA2(QUAD, a4, D3, D4),
    struct SingleBase *, SingleBase, 11, Single
)
{
    AROS_LIBFUNC_INIT

    return SingleBase->value + a1 + a2 + a3 + a4;

    AROS_LIBFUNC_EXIT
}

void StackSetValue(int v)
{
    struct SingleBase *SingleBase = (struct SingleBase *)__aros_getbase_SingleBase();
    SingleBase->value = v;
}

int StackAdd4(int a1, int a2, int a3, int a4)
{
    struct SingleBase *SingleBase = (struct SingleBase *)__aros_getbase_SingleBase();
    return SingleBase->value + a1 + a2 + a3 + a4;
}

int StackAdd6(int a1, int a2, int a3, int a4, int a5, int a6)
{
    struct SingleBase *SingleBase = (struct SingleBase *)__aros_getbase_SingleBase();
    return SingleBase->value + a1 + a2 + a3 + a4 + a5 + a6;
}

int StackAdd9(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9)
{
    struct SingleBase *SingleBase = (struct SingleBase *)__aros_getbase_SingleBase();
    return SingleBase->value + a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8 + a9;
}

int StackAdd4OrMore(int total, int a1, int a2, int a3, int a4, ...)
{
    struct SingleBase *SingleBase = (struct SingleBase *)__aros_getbase_SingleBase();
    int _return = SingleBase->value + a1 + a2 + a3 + a4;
    int i = 4;
    va_list args;
    va_start(args, a4);
    while (i < total)
    {
        _return += va_arg(args, int);
        i++;
    }

    va_end(args);
    return _return;
}

double StackAddDouble2(double a1, double a2)
{
    struct SingleBase *SingleBase = (struct SingleBase *)__aros_getbase_SingleBase();
    return SingleBase->value + a1 + a2;
}
