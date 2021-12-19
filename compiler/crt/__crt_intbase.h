/*
    Copyright (C) 2021, The AROS Development Team. All rights reserved.

*/
#ifndef __CRT_INTBASE_H
#define __CRT_INTBASE_H

#include <exec/libraries.h>
#include <dos/dos.h>
#include <devices/timer.h>

#include <setjmp.h>

struct StdCIntBase;
struct PosixCIntBase;

struct CrtIntBase
{
    struct Library          lib;
    struct StdCIntBase      *StdCBase;
    struct PosixCIntBase    *PosixCBase;

    struct CrtIntBase       *fakevforkbase;
};

struct Library * __aros_getbase_CrtBase();

struct CrtProgCtx
{
    /* start program */
    int                         *startup_errorptr;
    jmp_buf                     exit_jmpbuf;

    /* end program */
    int                         exitflags;

    /* vfork/exec */
    int                         vforkflags;
    /* __vfork.c */
    struct vfork_data           *vfork_data;

    /* atexit.c */
    struct MinList              atexit_list;

    /* Libbase that was in use when this context was created */
    /* Treat this field as readonly. Never assign/substitute its value */
    struct CrtIntBase           *libbase;
};

struct CrtProgCtx * __aros_get_ProgCtx();
struct CrtProgCtx * __aros_create_ProgCtx();
void __aros_delete_ProgCtx();

void __modonly_abort(void);
char ***__posixc_get_environptr(void);

/* Make a distinction between exit() and abort() */
#define ABNORMAL_EXIT 0x00000001


/* flags; values of flags are power of two so they can be ORed together */

/* When a program is started with the exec functions and from vfork,
   this is indicated in the flags of the library.
   This way the child can use the parent crt.library during its
   initialization phase */
#define EXEC_PARENT 0x00000001
#define VFORK_PARENT 0x00000002

/* This flag is set by vfork() to correctly report child process ID during
   execution of child code, even though that it's actually executed by parent
   process until execve() is called. */
#define PRETEND_CHILD 0x00000004

#endif //__CRT_INTBASE_H
