/*
    Copyright (C) 2021, The AROS Development Team. All rights reserved.

*/
#ifndef __CRTEXT_INTBASE_H
#define __CRTEXT_INTBASE_H

#include <exec/libraries.h>
#include <dos/dos.h>
#include <devices/timer.h>

#include "../stdc/__stdc_intbase.h"
#include "__posixc_intbase.h"

struct CrtExtIntBase
{
    struct Library          lib;
    struct StdCIntBase      *StdCBase;
    struct PosixCIntBase    *PosixCBase;
};

struct Library * __aros_getbase_CrtExtBase();

void __progonly_set_exitjmp(jmp_buf exitjmp, jmp_buf previousjmp);
void __progonly_jmp2exit(int normal, int retcode);
int *__progonly_get_errorptr(void);
int *__progonly_set_errorptr(int *errorptr);
void __progonly_callexitfuncs(void);
void __progonly_program_startup(jmp_buf exitjmp, int *errorptr);

#endif //__CRTEXT_INTBASE_H
