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

struct CrtExtProgCtx
{
    /* start program */
    int                         *startup_errorptr;
    jmp_buf                     exit_jmpbuf;

    /* end program */
    int                         exitflags;

    /* atexit.c */
    struct MinList              atexit_list;
};

struct CrtExtProgCtx * __aros_get_ProgCtx();
struct CrtExtProgCtx * __aros_create_ProgCtx();
void __aros_delete_ProgCtx();

void __progonly_set_exitjmp(jmp_buf exitjmp, jmp_buf previousjmp);
void __progonly_jmp2exit(int normal, int retcode);
int *__progonly_get_errorptr(void);
int *__progonly_set_errorptr(int *errorptr);
void __progonly_callexitfuncs(void);
void __progonly_program_startup(jmp_buf exitjmp, int *errorptr);

int __progonly_init_atexit(struct CrtExtProgCtx *ProgCtx);
struct AtExitNode;
int __progonly_addexitfunc(struct AtExitNode *aen);

/* Make a distinction between exit() and abort() */
#define ABNORMAL_EXIT 0x00000001

#endif //__CRTEXT_INTBASE_H
