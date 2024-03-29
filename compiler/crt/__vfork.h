#ifndef __VFORK_H
#define __VFORK_H

/*
    Copyright � 2008-2013, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <dos/bptr.h>
#include <exec/exec.h>
#include <setjmp.h>
#include <sys/types.h>
#include <aros/startup.h>

#include "__fdesc.h"

struct CrtExtProgCtx;

struct vfork_data
{
    struct vfork_data *prev;                /* pointer to prev data; happens when vfork()ed process calls vfork() again */
    jmp_buf vfork_jmp;                      /* jmp back to place in program code where vfork was called */

    struct Task *parent;
    int *parent_olderrorptr;
    jmp_buf parent_oldexitjmp;
    jmp_buf parent_newexitjmp;
    BYTE parent_signal;
    BYTE parent_state;
    struct CrtProgCtx *parent_progctx;
    BPTR parent_curdir;
    int parent_flags;
    int parent_called_exec_pretending_child;


    struct Task *child;
    int child_error;
    int child_errno;
    BYTE child_signal;
    BYTE child_state;
    struct CrtProgCtx *child_progctx;

    const char *exec_filename;
    char *const *exec_argv;
    char *const *exec_envp;
    APTR ectx;
};

pid_t __vfork(jmp_buf env);
void vfork_longjmp (jmp_buf env, int val);

#define PARENT_STATE_EXIT_CALLED            (1L << 0)
#define PARENT_STATE_EXEC_CALLED            (1L << 1)
#define PARENT_STATE_EXEC_DO_FINISHED       (1L << 2)
#define PARENT_STATE_STOPPED_PRETENDING     (1L << 3)

#define CHILD_STATE_SETUP_FAILED            (1L << 0)
#define CHILD_STATE_SETUP_FINISHED          (1L << 1)
#define CHILD_STATE_EXEC_PREPARE_FINISHED   (1L << 2)
#define CHILD_STATE_UDATA_NOT_USED          (1L << 3)

#define DSTATE(x)   //x

#define PRINTSTATE  \
    DSTATE(bug("[PrintState]: Parent = %d, Child = %d, Line %d\n", udata->parent_state, udata->child_state, __LINE__));

#define SETPARENTSTATE(a) \
    udata->parent_state = a

#define ASSERTPARENTSTATE(a) \
    DSTATE({                                \
        if (!(udata->parent_state & (a)))   \
            bug("Parent state assertion failed, was %d, expected %d\n", udata->parent_state, (a));\
    })

#define SETCHILDSTATE(a) \
    udata->child_state = a

#define ASSERTCHILDSTATE(a) \
    DSTATE({                                \
        if (!(udata->child_state & (a)))    \
            bug("Child state assertion failed, was %d, expected %d\n", udata->parent_state, (a));\
    })

#endif /* __VFORK_H */
