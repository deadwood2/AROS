/*
    Copyright (C) 1995-2001, The AROS Development Team. All rights reserved.

    Desc: Initialize a SignalSemaphore
*/

#include "../include/exec/structures.h"

#define NT_SIGNALSEM    15

void abiv0_InitSemaphore(struct SignalSemaphoreV0 *sigSem, struct ExecBaseV0 *SysBaseV0)
{

    /* Clear list of wait messages */
    sigSem->ss_WaitQueue.mlh_Head     = (APTR32)(IPTR)&sigSem->ss_WaitQueue.mlh_Tail;
    sigSem->ss_WaitQueue.mlh_Tail     = NULL;
    sigSem->ss_WaitQueue.mlh_TailPred = (APTR32)(IPTR)&sigSem->ss_WaitQueue.mlh_Head;

    /* Set type of Semaphore */
    sigSem->ss_Link.ln_Type = NT_SIGNALSEM;

    /* Semaphore is currently unused */
    sigSem->ss_NestCount = 0;

    /* Semaphore has no owner yet */
    sigSem->ss_Owner = 0;

    /* Semaphore has no queue */
    sigSem->ss_QueueCount = -1;

} /* InitSemaphore */

