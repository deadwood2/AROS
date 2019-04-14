/*
    Copyright © 1995-2017, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Send some signal to a given task
    Lang: english
*/

#define DEBUG 0
#include <aros/debug.h>

#include <exec/execbase.h>
#include <aros/libcall.h>
#include <proto/exec.h>

#define __AROS_KERNEL__
#include "exec_intern.h"

#include <aros/atomic.h>
#include "etask.h"
#include "exec_locks.h"

/*****************************************************************************

    NAME */

        AROS_LH2(void, Signal,

/*  SYNOPSIS */
        AROS_LHA(struct Task *,     task,      A1),
        AROS_LHA(ULONG,             signalSet, D0),

/*  LOCATION */
        struct ExecBase *, SysBase, 54, Exec)

/*  FUNCTION
        Send some signals to a given task. If the task is currently waiting
        on these signals, has a higher priority as the current one and if
        taskswitches are allowed the new task begins to run immediately.

    INPUTS
        task      - Pointer to task structure.
        signalSet - The set of signals to send to the task.

    RESULT

    NOTES
        This function may be used from interrupts.

    EXAMPLE

    BUGS

    SEE ALSO
        AllocSignal(), FreeSignal(), Wait(), SetSignal(), SetExcept()

    INTERNALS

    HISTORY

******************************************************************************/
{
    AROS_LIBFUNC_INIT

    struct Task *thisTask = GET_THIS_TASK;

        D(bug("[Exec] Signal(0x%p, %08lX)\n", task, signalSet);)

        D(
            bug("[Exec] Signal: Signaling '%s', state = %08x\n", task->tc_Node.ln_Name, task->tc_State);
            if (((struct KernelBase *)KernelBase)->kb_ICFlags & KERNBASEF_IRQPROCESSING)
                bug("[Exec] Signal: (Called from Interrupt)\n");
            else
                bug("[Exec] Signal: (Called from '%s')\n", thisTask->tc_Node.ln_Name);
        )

        (void)thisTask;
        struct IntETask *etask = GetIntETask(task);
        pthread_mutex_lock(&etask->iet_SignalMutex);
        D(bug("[Exec] Signal: Target signal flags : %08x ->", task->tc_SigRecvd);)
        /* Set the signals in the task structure. */
#if defined(__AROSEXEC_SMP__)
        __AROS_ATOMIC_OR_L(task->tc_SigRecvd, signalSet);
#else
        task->tc_SigRecvd |= signalSet;
#endif
        D(bug(" %08x\n", task->tc_SigRecvd);)

        /* Do those bits raise exceptions? */
        if (task->tc_SigRecvd & task->tc_SigExcept)
        {
            /* Yes. Set the exception flag. */
#if defined(__AROSEXEC_SMP__)
            __AROS_ATOMIC_OR_B(task->tc_Flags, TF_EXCEPT);
#else
            task->tc_Flags |= TF_EXCEPT;
#endif
            D(bug("[Exec] Signal: TF_EXCEPT set\n");)

            /* 
                    if the target task is running (called from within interrupt handler, or from another processor),
                    raise the exception or defer it for later.
                */
            if (task->tc_State == TS_RUN)
            {
                D(bug("[Exec] Signal: Raising Exception for 'running' Task\n");)
                /* Order a reschedule */
                Enable();

                /* All done. */
                return;
            }
        }
        /*
            Is the task receiving the signals waiting on them
            (or on a exception) ?
        */
        if ((task->tc_State == TS_WAIT) &&
        (task->tc_SigRecvd & (task->tc_SigWait | task->tc_SigExcept)))
        {
            D(bug("[Exec] Signal: Signaling 'waiting' Task\n");)

            /* Yes. Move it to the ready list. */
            EXEC_LOCK_LIST_WRITE(&SysBase->TaskWait);
            Remove(&task->tc_Node);
            EXEC_UNLOCK_LIST(&SysBase->TaskWait);
            task->tc_State = TS_READY;
            EXEC_LOCK_LIST_WRITE(&SysBase->TaskReady);
            Enqueue(&SysBase->TaskReady, &task->tc_Node);
            EXEC_UNLOCK_LIST(&SysBase->TaskReady);
            pthread_cond_signal(&etask->iet_SignalCond);
        }

        pthread_mutex_unlock(&etask->iet_SignalMutex);

        D(bug("[Exec] Signal: 0x%p finished signal processing\n", task);)

    AROS_LIBFUNC_EXIT
}
