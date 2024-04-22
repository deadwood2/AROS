/*
    Copyright (C) 2024, The AROS Development Team. All rights reserved.

    Desc:
*/

#include <stdarg.h>
#include <exec/types.h>
#include <aros/libcall.h>
#include <proto/dos.h>
#include <proto/exec.h>

#include LC_LIBDEFS_FILE

#include "singlebase.h"

LONG val;

AROS_LH1(void, RegSetValue,
    AROS_LHA(LONG, v, D0),
    struct Library *, NoincludesBase, 5, Noincludes
)
{
    AROS_LIBFUNC_INIT

    val = v;

    AROS_LIBFUNC_EXIT
}

int StackGetValue()
{
    return val;
}

