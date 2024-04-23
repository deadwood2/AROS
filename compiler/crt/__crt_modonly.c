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
#include <stdio.h>

#include <aros/debug.h>

#include "__crt_intbase.h"
#include "__stdc_intbase.h"
#include "__crt_progonly.h"
#include "../stdc/__optionallibs.h"

static void __deadend(struct CrtIntBase *CrtBase, const char *s)
{
    if (__intuition_available(CrtBase->StdCBase))
    {
        struct EasyStruct es =
        {
            sizeof(struct EasyStruct),
            0,
            "Critical!",
            "%s",
            "OK"
        };

        stdcEasyRequest(CrtBase->StdCBase->StdCIntuitionBase, NULL, &es, NULL, s);
    }

    Alert(AT_DeadEnd);
}
void __modonly_abort(void)
{
    if (__aros_get_ProgCtx())
        __progonly_abort();
    else
    {
        char s[50];
        char *taskname = FindTask(NULL)->tc_Node.ln_Name;
        struct CrtIntBase * CrtBase = (struct CrtIntBase *)__aros_getbase_CrtBase();
        sprintf(s, "Program '%s' called abort() without doing program startup.", taskname);
        __deadend(CrtBase, s);
    }
}

void __modonly_exit(int code)
{
    if (__aros_get_ProgCtx())
        __progonly_exit(code);
    else
    {
        char s[50];
        char *taskname = FindTask(NULL)->tc_Node.ln_Name;
        struct CrtIntBase * CrtBase = (struct CrtIntBase *)__aros_getbase_CrtBase();
        sprintf(s, "Program '%s' called exit() without doing program startup.", taskname);
        __deadend(CrtBase, s);
    }
}
