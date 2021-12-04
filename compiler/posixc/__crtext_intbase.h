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

    /* vfork/exec */
    int                         vforkflags;

    /* atexit.c */
    struct MinList              atexit_list;

    /* __vfork.c */
    struct vfork_data *vfork_data;
};

struct CrtExtProgCtx * __aros_get_ProgCtx();
struct CrtExtProgCtx * __aros_create_ProgCtx();
void __aros_delete_ProgCtx();

void __progonly_set_exitjmp(jmp_buf exitjmp, jmp_buf previousjmp);
void __progonly_jmp2exit(int normal, int retcode);
int *__progonly_get_errorptr(void);
int *__progonly_set_errorptr(int *errorptr);
void __progonly_callexitfuncs(void);
void __progonly_program_startup_internal(struct CrtExtProgCtx *ProgCtx, jmp_buf exitjmp, int *errorptr);

int __progonly_init_atexit(struct CrtExtProgCtx *ProgCtx);
struct AtExitNode;
int __progonly_addexitfunc(struct AtExitNode *aen);

void __progonly__Exit (int code);

/* Make a distinction between exit() and abort() */
#define ABNORMAL_EXIT 0x00000001


/* flags; values of flags are power of two so they can be ORed together */

/* When a program is started with the exec functions and from vfork,
   this is indicated in the flags of the library.
   This way the child can use the parent posixc.library during its
   initialization phase */
#define EXEC_PARENT 0x00000001
#define VFORK_PARENT 0x00000002

/* This flag is set by vfork() to correctly report child process ID during
   execution of child code, even though that it's actually executed by parent
   process until execve() is called. */
#define PRETEND_CHILD 0x00000004

#endif //__CRTEXT_INTBASE_H
