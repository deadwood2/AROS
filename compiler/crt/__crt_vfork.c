/*
    Copyright (C) 2008-2022, The AROS Development Team. All rights reserved.
*/

#include <aros/debug.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <exec/exec.h>
#include <exec/tasks.h>
#include <dos/dos.h>
#include <aros/cpu.h>
#include <aros/startup.h>

#include <sys/types.h>
#include <unistd.h>
#include <setjmp.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "__crt_intbase.h"
#include "__crt_progonly.h"

#include "__fdesc.h"
#include "__vfork.h"
#include "__exec.h"

/* #define VFORK_USE_INLINECOPY */ /* Disabled. Causes crash in native gcc */
#if defined(VFORK_USE_INLINECOPY)
#define _VFORK_COPYENV(a,b) \
({ \
    int _i; \
    for (_i = 0; _i < sizeof(jmp_buf); _i++) \
        *((UBYTE *)((IPTR)(&a) + _i)) = *((UBYTE *)((IPTR)(&b) + _i)); \
})
#else
#define _VFORK_COPYENV(a,b)      *(a) = *(b)
#endif

BOOL __aros_setoffsettable(char *base);

/*****************************************************************************

    NAME
#include <unistd.h>

        pid_t vfork(

    SYNOPSIS
        void)

    FUNCTION
        Function to create a subprocess of the current process.

        This is there to ease porting of software using the fork()/vfork()
        POSIX functions. Due to a different memory and process model, fork()
        is not implemented at the moment in the C library. vfork() is provided
        with some extended functionality. In the POSIX standard the only
        guaranteed functionality for vfork() is to have an exec*() function or
        exit() called right after the vfork() in the child.

        Extra functionality for vfork():
        - The child has its own memory heap; memory allocation/deallocation
          is allowed and the heap will be removed when calling _exit() or will
          be used for the code started by the exec*() functions.
        - The child will have a copy of the file descriptors as specified by
          the POSIX standard for the fork() function. File I/O is possible in
          the child, as is file manipulation with dup() etc.

        Difference with fork():
        - The virtual memory heap is not duplicated as in POSIX but the memory
          is shared between parent and child. AROS lives in one big single
          memory region so changes to memory in the child are also seen by the
          parent.

        Behavior for other resources not described in this doc may not be
        relied on for future compatibility.

    INPUTS
        -

    RESULT
        -1: error, no child is started; errno will be set.
        0: Running in child
        >0: Running in parent, pid of child is return value.

    NOTES
        Current implementation of vfork() will only really start running things
        in parallel on an exec*() call. After vfork(), parent process will run child code
        until _exit() or exec*(). With _exit(), the the parent process will exit child
        code and will continue running parent code; with exec*(), the child process will be
        detached and will continue executing child code from exec*(), while the parent
        procees will stop executing child code and will continue executing parent code.

    EXAMPLE

    BUGS

    SEE ALSO
        execl(), execve(), execlp(), execv(), execvp()

    INTERNALS

******************************************************************************/

/* The following functions are used to update the child's and parent's privdata
   for the parent pretending to be running as child and for the child to take
   over. It is called in the following sequence:
   1) parent_enterpretendchild() is called in vfork() so the parent pretends to be
   running as child. Parent executes child code until the code has exec*() or exit():
      1a) If exit(), execution is stoped and Parent jumps to "else" of "parent_newexitjmp"
      1b) If exec*(), child_takeover() is called by Child so it can continue from
      the Parent state
   2) parent_leavepretendchild() is called by Parent to switch back to be running
   as parent code
*/
static void parent_enterpretendchild(struct vfork_data *udata);
static void child_takeover(struct vfork_data *udata);
static void parent_leavepretendchild(struct vfork_data *udata);
static void parent_createchild(struct vfork_data *udata);

static __attribute__((noinline)) void __vfork_exit_controlled_stack(struct vfork_data *udata);

/* Needed to manage temporary libbase */
void __aros_setbase_CrtBase(struct CrtIntBase *CrtBase);
void __aros_setbase_fake_CrtBase(struct CrtIntBase *fCrtBase);

int __crtext_open(struct CrtIntBase *CrtBase);
int __init_memstuff(struct CrtIntBase *CrtBase);
int __copy_fdarray(fdesc ***__src_fd_array, int fd_slots);
int __init_stdio(struct CrtIntBase *CrtBase);

void __exit_fd(struct CrtIntBase *CrtBase);
void __exit_memstuff(struct CrtIntBase *CrtBase);
void __crtext_close(struct CrtIntBase *CrtBase);
/* End */

LONG launcher()
{
    D(bug("launcher: Entered child launcher, ThisTask=%p\n", FindTask(NULL)));

    struct Task *this = FindTask(NULL);
    struct vfork_data *udata = this->tc_UserData;
    BYTE child_signal;
    struct CrtProgCtx *ProgCtx = NULL;
    jmp_buf exec_exitjmp; /* jmp_buf for when calling __exec_do */
    int exec_error; /* errno for when calling __exec_do */
    LONG ret = 0;

    /* Allocate signal for parent->child communication */
    child_signal = udata->child_signal = AllocSignal(-1);
    D(bug("launcher: Allocated child signal: %d\n", udata->child_signal));
    if (udata->child_signal == -1)
    {
        /* Lie */
        udata->child_errno = ENOMEM;
        SETCHILDSTATE(CHILD_STATE_SETUP_FAILED);
        Signal(udata->parent, 1 << udata->parent_signal);
        return -1;
    }

    /*
        Create temporary context for this child

        If child eventually __exec_do() into another binary, this contenxt will
        be overwritten. It is created however to carry over informtion into
        process of opening library by child (see __fdesc/__init_fd)
    */

    ProgCtx = __aros_create_ProgCtx();
    udata->child_progctx = ProgCtx;

    /*
        Child has not opened C library on its own. It will only
        do this when __exec_do() into the binary. ProgCtx->libbase
        is NULL. We create fake libbase here to carry over information
        as well as be used for child code prior to exec*(). (child code
        can for example manipulate file descriptors, example "gcc -pipe")
    */

    ProgCtx->libbase = (struct CrtIntBase *)AllocMem(sizeof(struct CrtIntBase), MEMF_PUBLIC);
    /* "register" temp base with child process. */
    __aros_setoffsettable((char *)ProgCtx->libbase);

    /* Initialize */
    __crtext_open(ProgCtx->libbase);
    __init_memstuff(ProgCtx->libbase);

    /* Parent process will copy remaining information into this libbase in parent_enterpretenchild */

    if (setjmp(exec_exitjmp) == 0)
    {
        /* Setup complete, signal parent */
        D(bug("launcher: Signaling parent that we finished setup\n"));
        SETCHILDSTATE(CHILD_STATE_SETUP_FINISHED);
        Signal(udata->parent, 1 << udata->parent_signal);

        D(bug("launcher: Child waiting for exec or exit\n"));
        Wait(1 << udata->child_signal);
        ASSERTPARENTSTATE(PARENT_STATE_EXEC_CALLED | PARENT_STATE_EXIT_CALLED);
        PRINTSTATE;

        if (udata->parent_called_exec_pretending_child)
        {
            APTR ectx;

            D(bug("launcher: child called exec()\n"));

            child_takeover(udata);

            /* Filenames passed from parent obey parent's doupath */
            ectx = udata->ectx = __progonly_exec_prepare(
                udata->exec_filename,
                0,
                udata->exec_argv,
                udata->exec_envp
            );

            udata->child_errno = errno;

            D(bug("launcher: informing parent that we have run __exec_prepare\n"));
            /* Inform parent that we have run __exec_prepare */
            SETCHILDSTATE(CHILD_STATE_EXEC_PREPARE_FINISHED);
            Signal(udata->parent, 1 << udata->parent_signal);

            /* Wait 'till __exec_do() is called on parent process */
            D(bug("launcher: Waiting parent to get the result\n"));
            Wait(1 << udata->child_signal);
            ASSERTPARENTSTATE(PARENT_STATE_EXEC_DO_FINISHED);
            PRINTSTATE;

            D(bug("launcher: informing parent that we won't use udata anymore\n"));

            /* !!! No usage of any parent data beyond this point !!! */

            /*
                Once the signal is sent, parent will start leaving "pretend
                child" state and will eventually jump to parent code which
                can mean then exit from program.
            */

            /* Inform parent that we won't use udata anymore */
            SETCHILDSTATE(CHILD_STATE_UDATA_NOT_USED);
            Signal(udata->parent, 1 << udata->parent_signal);

            D(bug("launcher: waiting for parent to be after _exit()\n"));
            Wait(1 << udata->child_signal);
            ASSERTPARENTSTATE(PARENT_STATE_STOPPED_PRETENDING);
            PRINTSTATE;

            if (ectx)
            {
                /* Flush whatever parent pretending as child wrote */
                fflush(NULL);

                D(bug("launcher: prepare to catch _exit()\n"));
                /* Part 2 of "program_startup" for child. */
                __progonly_program_startup_internal(ProgCtx, exec_exitjmp, &exec_error);

                D(bug("launcher: executing command\n"));
                __progonly_exec_do(ectx);

                assert(0); /* Should not be reached */
            }
            else
            {
                D(bug("launcher: __exec_prepare returned with an error\n"));
                ret = -1;
            }
        }
        else /* !udata->child_called_exec */
        {
            D(bug("launcher: child did not call exec()\n"));

            D(bug("launcher: informing parent that we won't use udata anymore\n"));
            /* Inform parent that we won't use udata anymore */
            SETCHILDSTATE(CHILD_STATE_UDATA_NOT_USED);
            Signal(udata->parent, 1 << udata->parent_signal);
        }
    }
    else
    {
        /* child exited */
        D(bug("launcher: catched _exit(), errno=%d\n", exec_error));
    }

    fflush(NULL);

    D(bug("launcher: freeing child_signal\n"));
    FreeSignal(child_signal);

    D(bug("launcher: freing resources\n"));
    __exit_fd(ProgCtx->libbase);
    __exit_memstuff(ProgCtx->libbase);
    __crtext_close(ProgCtx->libbase);
    FreeMem(ProgCtx->libbase, sizeof(struct CrtIntBase));
    FreeMem(ProgCtx, sizeof(struct CrtProgCtx));
    
    D(bug("Child Done\n"));

    return ret;
}

/* This can be good for debugging */
#ifdef __arm__
#define SP 8
#define ALT 27
#endif

#ifndef SP
#define SP  _JMPLEN - 1
#define ALT _JMPLEN - 1
#endif

pid_t __vfork(jmp_buf env)
{
    struct CrtProgCtx *ProgCtx = __aros_get_ProgCtx();
    struct Task *this = FindTask(NULL);
    struct ETask *etask = NULL;
    struct vfork_data *udata = AllocMem(sizeof(struct vfork_data), MEMF_ANY | MEMF_CLEAR);

    /*
        Remember that vfork() can be called recursivelly, that is a child of parent,
        can create new child and so on. This is supported via vfork_data->prev chain.
        if (vfork() == 0)
            if (vfork() == 0)
                child of child code
    /*
        vfork() is being called by a direct call without passing a library base
        Use library base registered with context. We can do this, because vfork()
        is progonly, meaning registered library base is one opened by program
        binary.
    */
    __aros_setoffsettable((char *)ProgCtx->libbase);

    if (udata == NULL)
    {
        errno = ENOMEM;
        vfork_longjmp(env, -1);
    }
    D(bug("__vfork: Parent: ThisTask=%s, ProgCtx=%p\n", this->tc_Node.ln_Name, ProgCtx));
    D(bug("__vfork: Parent: allocated udata %p, jmp_buf %p\n", udata, udata->vfork_jmp));
    _VFORK_COPYENV(udata->vfork_jmp,env);

    D(bug("__vfork: Parent: initial jmp_buf %p\n", env));
    D(bug("__vfork: Parent: ip: %p, stack: %p, alt: 0x%p\n", env->retaddr, env->regs[SP], env->regs[ALT]));
    D(bug("__vfork: Parent: Current altstack 0x%p\n", *((void **) this->tc_SPLower)));
    D(hexdump(env, 0, sizeof(jmp_buf) + sizeof(void *) * 4));

    udata->parent = this;
    udata->prev = ProgCtx->vfork_data;

    D(bug("__vfork: Parent: Saved old parent's vfork_data: %p\n", udata->prev));
    udata->parent_progctx = ProgCtx;

    parent_createchild(udata);
    /* After this point parent can assume memory pointed udata->child_progctx and
       udata->child_progctx->libbase contains active objects */

    D(bug("__vfork: Parent: Setting jmp_buf at %p\n", udata->parent_newexitjmp));
    if (setjmp(udata->parent_newexitjmp) == 0)
    {
        udata->parent_olderrorptr = __progonly_set_errorptr(&udata->child_error);
        udata->child_error = *udata->parent_olderrorptr;
        __progonly_set_exitjmp(udata->parent_newexitjmp, udata->parent_oldexitjmp);

        parent_enterpretendchild(udata);

        D(bug("__vfork: ParentPretendingChild(%p) jumping to jmp_buf %p\n",
              udata->parent, &udata->vfork_jmp
        ));
        D(bug("__vfork: ip: %p, stack: %p alt: %p\n",
              udata->vfork_jmp[0].retaddr, udata->vfork_jmp[0].regs[SP],
              udata->vfork_jmp[0].regs[ALT]
        ));

        /* This equals to "returning" from vfork() into child code (if(vfork() == 0)) */
        vfork_longjmp(udata->vfork_jmp, 0);
        assert(0); /* not reached */
        return (pid_t) 0;
    }
    else /* _exit() was called either by Parent in child code or by Parent in __exec_do_pretend_child */
    {
        D(bug("__vfork: ParentPretendingChild: child called exit() or exec()\n"));

        /* Stack may have been overwritten when we return here,
         * we jump to here from a function lower in the call chain
         */
        this = FindTask(NULL);
        ProgCtx = __aros_get_ProgCtx();
        udata = ProgCtx->vfork_data;

        D(bug("__vfork: ParentPretendingChild: ThisTask=%p, ProgCtx=%p, vfork_data = %x\n", this, ProgCtx, udata));

        if (!udata->parent_called_exec_pretending_child)
        {
            D(bug("__vfork: ParentPretendingChild: child did not call exec()\n"));

            /* et_Result is normally set in startup code but no exec was performed
               so we have to mimic the startup code
             */
            etask = GetETask(udata->child);
            if (etask)
                etask->et_Result1 = udata->child_error;

            D(bug("__vfork: ParentPretendingChild: Signaling child %p, signal %d\n", udata->child, udata->child_signal));
            SETPARENTSTATE(PARENT_STATE_EXIT_CALLED);
            Signal(udata->child, 1 << udata->child_signal);
            /* After this point parent must assume memory pointed udata->child_progctx and
               udata->child_progctx->libbase might have been freed. Parent cannot call any
               C library functions which need child fake PosixCBase to be returned via
               standard __aros_getbase_PosixCBase()
            */
        }

        D(bug("__vfork: ParentPretendingChild: Waiting for child to finish using udata, me=%p, signal %d\n", FindTask(NULL),
                udata->parent_signal));
        /* Wait for child to finish using udata */
        Wait(1 << udata->parent_signal);
        ASSERTCHILDSTATE(CHILD_STATE_UDATA_NOT_USED);
        PRINTSTATE;

        __vfork_exit_controlled_stack(udata);

        assert(0); /* not reached */
        return (pid_t) 1;
    }
}

/*
 * The sole purpose of this function is to enable control over allocation of dummy and env.
 * Previously they were allocated in the ending code of __vfork function. On ARM however
 * this was causing immediate allocation of space at entry to the __vfork function. Moreover
 * the jmp_buf is aligned(16) and as such is represented on the stack as a pointer to stack
 * region instead of offset from stack base.
 *
 * The exit block of __vfork function represents code that underwent a number of longjumps. The
 * stack there is not guaranteed to be preserved, thus the on-stack pointer representing dummy
 * and env were also damaged. Extracting the code below allows to control when the variables
 * are allocated (as long as the function remains not inlined).
 */
static __attribute__((noinline)) void __vfork_exit_controlled_stack(struct vfork_data *udata)
{
    jmp_buf dummy;
    jmp_buf env;
    ULONG childid;

    D(bug("__vfork: ParentPretendingChild: freeing parent signal\n"));
    FreeSignal(udata->parent_signal);

    errno = udata->child_errno;

    parent_leavepretendchild(udata);

    if(udata->parent_called_exec_pretending_child)
    {
        D(bug("__vfork: Inform child that we are after _exit()\n"));
        SETPARENTSTATE(PARENT_STATE_STOPPED_PRETENDING);
        Signal(udata->child, 1 << udata->child_signal);
    }

    D(bug("__vfork: Parent: restoring startup buffer\n"));
    /* Restore parent errorptr and startup buffer */
    __progonly_set_errorptr(udata->parent_olderrorptr);
    __progonly_set_exitjmp(udata->parent_oldexitjmp, dummy);

    /* Save some data from udata before udata is being freed */
    _VFORK_COPYENV(env,udata->vfork_jmp);
    childid = GetETaskID(udata->child);

    D(bug("__vfork: Parent: freeing udata\n"));
    FreeMem(udata, sizeof(struct vfork_data));

    D(bug("__vfork: Parent jumping to jmp_buf %p\n", env));
    D(bug("__vfork: ip: %p, stack: %p\n", env->retaddr, env->regs[SP]));

    /* This equals to "returning" from vfork() into parent code (if(vfork() > 0)) */
    vfork_longjmp(env, childid);
}

static void parent_createchild(struct vfork_data *udata)
{
    struct CrtProgCtx *ProgCtx = __aros_get_ProgCtx();
    jmp_buf vfork_jmp;

    _VFORK_COPYENV(vfork_jmp,udata->vfork_jmp);

    struct TagItem tags[] =
    {
        { NP_Entry,         (IPTR) launcher },
        { NP_CloseInput,    (IPTR) FALSE },
        { NP_CloseOutput,   (IPTR) FALSE },
        { NP_CloseError,    (IPTR) FALSE },
        { NP_Cli,           (IPTR) TRUE },
        { NP_Name,          (IPTR) "vfork()" },
        { NP_UserData,      (IPTR) udata },
        { NP_NotifyOnDeath, (IPTR) TRUE },
        { TAG_DONE, 0 }
    };

    D(bug("__vfork: Parent: Allocating parent signal\n"));
    /* Allocate signal for child->parent communication */
    udata->parent_signal = AllocSignal(-1);
    if (udata->parent_signal == -1)
    {
        /* Couldn't allocate the signal, return -1 */
        FreeMem(udata, sizeof(struct vfork_data));
        errno = ENOMEM;
        vfork_longjmp(vfork_jmp, -1);
    }

    ProgCtx->vforkflags |= VFORK_PARENT;

    D(bug("__vfork: Parent: Creating child\n"));
    udata->child = (struct Task*) CreateNewProc(tags);

    if (udata->child == NULL)
    {
        D(bug("__vfork: Child could not be created\n"));
        /* Something went wrong, return -1 */
        FreeMem(udata, sizeof(struct vfork_data));
        errno = ENOMEM; /* Most likely */
        vfork_longjmp(vfork_jmp, -1);
    }
    D(bug("__vfork: Parent: Child created %p, waiting to finish setup\n", udata->child));

    /* Wait for child to finish setup */
    Wait(1 << udata->parent_signal);
    ASSERTCHILDSTATE(CHILD_STATE_SETUP_FAILED | CHILD_STATE_SETUP_FINISHED);
    PRINTSTATE;

    if (udata->child_errno)
    {
        /* An error occured during child setup */
        D(bug("__vfork: Child returned an error (%d)\n", udata->child_errno));
        errno = udata->child_errno;
        vfork_longjmp(vfork_jmp, -1);
    }

    ProgCtx->vforkflags &= ~VFORK_PARENT;
}

static void parent_enterpretendchild(struct vfork_data *udata)
{
    struct CrtProgCtx *ProgCtx = __aros_get_ProgCtx();
    struct PosixCIntBase *pPosixCBase =
        (struct PosixCIntBase *)__aros_getbase_PosixCBase();
    D(bug("parent_enterpretendchild(%x): entered\n", udata));

    ProgCtx->vfork_data = udata;

    /* Register child fake libbase as my own. This will start returning child fake
       PosixCBase via standard __aros_getbase_PosixCBase() in parent code */
    __aros_setbase_fake_CrtBase(udata->child_progctx->libbase);

    /* Copy necessary information into fake child libbase */
    __copy_fdarray(pPosixCBase->fd_array, pPosixCBase->fd_slots);
    __init_stdio(udata->child_progctx->libbase);
    udata->child_progctx->libbase->PosixCBase->doupath = pPosixCBase->doupath;

    /* Note: it is possible that Parent should replace its ProgCtx with the
       created by Child for the durection of "pretending". I have not come
       across a test case so far that show that this is required
    */

    /* Remember and switch chdir fields */
    udata->parent_curdir = CurrentDir(((struct Process *)udata->child)->pr_CurrentDir);

    /* Pretend to be running as the child created by vfork */
    udata->parent_flags = ProgCtx->vforkflags;
    ProgCtx->vforkflags |= PRETEND_CHILD;

    D(bug("parent_enterpretendchild: leaving\n"));
}

static void child_takeover(struct vfork_data *udata)
{
    D(bug("child_takeover(%x): entered\n", udata));

    /* Set current dir to parent's current dir */
    CurrentDir(((struct Process *)udata->parent)->pr_CurrentDir);

    D(bug("child_takeover(): leaving\n"));
}

static void parent_leavepretendchild(struct vfork_data *udata)
{
    struct CrtProgCtx *ProgCtx = __aros_get_ProgCtx();
    D(bug("parent_leavepretendchild(%x): entered\n", udata));

    /* Switch to currentdir from before vfork() call */
    CurrentDir(udata->parent_curdir);

    /* Switch to previous vfork_data */
    ProgCtx->vfork_data = udata->prev;
    ProgCtx->vforkflags = udata->parent_flags;

    if (ProgCtx->vfork_data)
        __aros_setbase_fake_CrtBase(ProgCtx->vfork_data->child_progctx->libbase);
    else
        __aros_setbase_fake_CrtBase(NULL);

    D(bug("parent_leavepretendchild: leaving\n"));
}
