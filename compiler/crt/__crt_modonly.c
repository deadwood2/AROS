/*
    Copyright (C) 2021, The AROS Development Team. All rights reserved.

    This file contains support for functions that can only be called from
    a library not a program.
*/
#include <dos/stdio.h>
#include <exec/alerts.h>
#include <proto/exec.h>

#include <assert.h>
#include <setjmp.h>

#include <aros/debug.h>

#include "__crt_intbase.h"
#include "__crt_progonly.h"

void __modonly_abort(void)
{
    if (__aros_get_ProgCtx())
        __progonly_abort();
    else
    {
        int a = 0 / 0; // FIXME!!!
    }
}
