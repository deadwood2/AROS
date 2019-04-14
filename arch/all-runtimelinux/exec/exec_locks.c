/*
    Copyright © 2017, The AROS Development Team. All rights reserved.
    $Id$
*/

/*
    Desc:

        Exposes a Public resource on smp systems that allows user space code
        to create (spin) locks, and exposes locking on exec Lists.

        e.g. LDDemon uses this to lock access to the system lists
        when scanning devices/libraries etc - since are not allowed
        to directly call the kernel locking functions or access the exec locks.

    Lang: english
*/

#include <aros/config.h>

#include <asm/cpu.h>
#include <aros/atomic.h>
#include <aros/types/spinlock_s.h>
#include <proto/exec.h>
#include <stdlib.h>
#include "exec_intern.h"

#undef Allocate
#define Allocate(a, b) malloc(b)

void __spinlock_init(spinlock_t *lock, APTR base)
{
    D(bug("[Kernel] %s(0x%p)\n", __func__, lock));

    lock->lock = SPINLOCK_UNLOCKED;
    lock->s_Owner = NULL;

    return;
}

APTR __spinlock_lock(spinlock_t *lock, APTR failhook, ULONG mode, APTR base)
{
    if (mode == SPINLOCK_MODE_WRITE)
    {
        ULONG tmp;
        struct Task *me = NULL;
        /* BYTE old_pri = 0;
        UBYTE priority_changed = 0; */
        me = GET_THIS_TASK;
        /*
        Check if lock->lock equals to SPINLOCK_UNLOCKED. If yes, it will be atomicaly replaced by SPINLOCKF_WRITE and function
        returns 1. Otherwise it copies value of lock->lock into tmp and returns 0.
        */
        while (!compare_and_exchange_long((ULONG*)&lock->lock, SPINLOCK_UNLOCKED, SPINLOCKF_WRITE, &tmp))
        {
            struct Task *t = lock->s_Owner;
            // Tell CPU we are spinning
            asm volatile("pause");

            // Call failhook if there is any
            if (failhook)
            {
               D(bug("[Kernel] %s: lock-held ... calling fail hook @ 0x%p ...\n", __func__, failhook);)
                CALLHOOKPKT(failhook, (APTR)lock, 0);
            }
            D(bug("[Kernel] %s: my name is %s\n", __func__, Kernel_13_KrnIsSuper() ? "--supervisor code--" : me->tc_Node.ln_Name));
            D(bug("[Kernel] %s: spinning on held lock (val=%08x, s_Owner=%p)...\n", __func__, tmp, t));
            if (me && t && (me->tc_Node.ln_Pri > t->tc_Node.ln_Pri))
            {
                D(bug("[Kernel] %s: spinlock owner (%s) has pri %d, lowering ours...\n", __func__, t ? (t->tc_Node.ln_Name) : "--supervisor--", t ? t->tc_Node.ln_Pri : 999));
                // If lock is spinning and the owner of lock has lower priority than ours, we need to reduce
                // tasks priority too, otherwise it might happen that waiting task spins forever
                /* priority_changed = 1; */
                /* old_pri = SetTaskPri(me, t->tc_Node.ln_Pri); FIXME: SetTaskPri not implemented*/
            }
        };
        lock->s_Owner = me;
        /* if (priority_changed)
            SetTaskPri(me, old_pri); */
    }
    else
    {
        /*
        Check if upper 8 bits of lock->lock are all 0, which means spinlock is not in UPDATING state and it is not
        in the WRITE state. If we manage to obtain it, we set the UPDATING flag. Until we release UPDATING state
        we are free to do whatever we want with the spinlock
        */
        while (!compare_and_exchange_byte((UBYTE*)&lock->block[3], 0, SPINLOCKF_UPDATING >> 24, NULL))
        {
            // Tell CPU we are spinning
            asm volatile("pause");

            // Call fail hook if available
            if (failhook)
            {
                D(bug("[Kernel] %s: write-locked .. calling fail hook @ 0x%p ...\n", __func__, failhook);)
                CALLHOOKPKT(failhook, (APTR)lock, 0);
            }
            D(bug("[Kernel] %s: spinning on write lock ...\n", __func__);)
        };

        /*
        At this point we have the spinlock in UPDATING state. So, update readcount field (no worry with atomic add,
        spinlock is for our exclusive use here), and then release it just by setting updating flag to 0
        */
        lock->slock.readcount++;
#if defined(AROS_NO_ATOMIC_OPERATIONS)
        lock->slock.updating = 0;
#else
        __AROS_ATOMIC_AND_L(lock->lock, ~SPINLOCKF_UPDATING);
#endif
    }

    D(bug("[Kernel] %s: lock = %08x\n", __func__, lock->lock));

    return lock;
}

void __spinlock_unlock(spinlock_t *lock, APTR base)
{
    D(bug("[Kernel] %s(0x%p)\n", __func__, lock));

    lock->s_Owner = NULL;
    /*
    use cmpxchg - expect SPINLOCKF_WRITE and replace it with 0 (unlocking the spinlock), if that succeeded, the lock
    was in WRITE mode and is now free. If that was not the case, continue with unlocking READ mode spinlock
    */
    if (!compare_and_exchange_long((ULONG*)&lock->lock, SPINLOCKF_WRITE, 0, NULL))
    {
        /*
        Unlocking READ mode spinlock means we need to put it into UDPATING state, decrement counter and unlock from
        UPDATING state.
        */
        while (!compare_and_exchange_byte((UBYTE*)&lock->block[3], 0, SPINLOCKF_UPDATING >> 24, NULL))
        {
            // Tell CPU we are spinning
            asm volatile("pause");
        }

        // Just in case someone tries to unlock already unlocked stuff
        if (lock->slock.readcount != 0)
        {
            lock->slock.readcount--;
        }
#if defined(AROS_NO_ATOMIC_OPERATIONS)
        lock->slock.updating = 0;
#else
        __AROS_ATOMIC_AND_L(lock->lock, ~SPINLOCKF_UPDATING);
#endif
    }

    D(bug("[Kernel] %s: lock = %08x\n", __func__, lock->lock));

    return;
}

#if defined(__AROSEXEC_SMP__)

#define DEBUG 0

#include <aros/debug.h>
#include <proto/exec.h>

#include "exec_debug.h"
#include "exec_intern.h"
#include "exec_locks.h"

int ExecLock__InternObtainSystemLock(struct List *systemList, ULONG mode, ULONG flags)
{
    spinlock_t *sysListLock = NULL;
    D(char *name = "??");

    if (systemList)
    {
        if (&SysBase->ResourceList == systemList) {
            D(name="ResourceList");
            sysListLock = &PrivExecBase(SysBase)->ResourceListSpinLock;
        }
        else if (&SysBase->DeviceList == systemList) {
            D(name="DeviceList");
            sysListLock = &PrivExecBase(SysBase)->DeviceListSpinLock;
        }
        else if (&SysBase->IntrList == systemList) {
            D(name="IntrList");
            sysListLock = &PrivExecBase(SysBase)->IntrListSpinLock;
        }
        else if (&SysBase->LibList == systemList) {
            D(name="LibList");
            sysListLock = &PrivExecBase(SysBase)->LibListSpinLock;
        }
        else if (&SysBase->PortList == systemList) {
            D(name="PortList");
            sysListLock = &PrivExecBase(SysBase)->PortListSpinLock;
        }
        else if (&SysBase->SemaphoreList == systemList) {
            D(name="SemaphoreList");
            sysListLock = &PrivExecBase(SysBase)->SemListSpinLock;
        }
        else if (&SysBase->MemList == systemList) {
            D(name="MemList");
            sysListLock = &PrivExecBase(SysBase)->MemListSpinLock;
        }
        else if (&SysBase->TaskReady == systemList) {
            D(name="TaskReady");
            sysListLock = &PrivExecBase(SysBase)->TaskReadySpinLock;
        }
        else if (&SysBase->TaskWait == systemList) {
            D(name="TaskWait");
            sysListLock = &PrivExecBase(SysBase)->TaskWaitSpinLock;
        }
        else if (&PrivExecBase(SysBase)->TaskRunning == systemList) {
            D(name="TaskRunning");
            sysListLock = &PrivExecBase(SysBase)->TaskRunningSpinLock;
        }
        else if (&PrivExecBase(SysBase)->TaskSpinning == systemList) {
            D(name="TaskSpinning");
            sysListLock = &PrivExecBase(SysBase)->TaskSpinningLock;
        }
    }
    D(bug("[Exec:Lock] %s(), List='%s' (%p), mode=%s, flags=%d\n", __func__, name, systemList, mode == SPINLOCK_MODE_WRITE ? "write":"read", flags));
    if (sysListLock)
    {
        if (flags && LOCKF_DISABLE)
            Disable();
        if (flags && LOCKF_FORBID)
            Forbid();

        EXEC_SPINLOCK_LOCK(sysListLock, NULL, mode);

        return TRUE;
    }
    return FALSE;
}

void ExecLock__InternReleaseSystemLock(struct List *systemList, ULONG flags)
{
    spinlock_t *sysListLock = NULL;
    D(char *name = "??");

    if (systemList)
    {
        if (&SysBase->ResourceList == systemList) {
            D(name="ResourceList");
            sysListLock = &PrivExecBase(SysBase)->ResourceListSpinLock;
        }
        else if (&SysBase->DeviceList == systemList) {
            D(name="DeviceList");
            sysListLock = &PrivExecBase(SysBase)->DeviceListSpinLock;
        }
        else if (&SysBase->IntrList == systemList) {
            D(name="IntrList");
            sysListLock = &PrivExecBase(SysBase)->IntrListSpinLock;
        }
        else if (&SysBase->LibList == systemList) {
            D(name="LibList");
            sysListLock = &PrivExecBase(SysBase)->LibListSpinLock;
        }
        else if (&SysBase->PortList == systemList) {
            D(name="PortList");
            sysListLock = &PrivExecBase(SysBase)->PortListSpinLock;
        }
        else if (&SysBase->SemaphoreList == systemList) {
            D(name="SemaphoreList");
            sysListLock = &PrivExecBase(SysBase)->SemListSpinLock;
        }
        else if (&SysBase->MemList == systemList) {
            D(name="MemList");
            sysListLock = &PrivExecBase(SysBase)->MemListSpinLock;
        }
        else if (&SysBase->TaskReady == systemList) {
            D(name="TaskReady");
            sysListLock = &PrivExecBase(SysBase)->TaskReadySpinLock;
        }
        else if (&SysBase->TaskWait == systemList) {
            D(name="TaskWait");
            sysListLock = &PrivExecBase(SysBase)->TaskWaitSpinLock;
        }
        else if (&PrivExecBase(SysBase)->TaskRunning == systemList) {
            D(name="TaskRunning");
            sysListLock = &PrivExecBase(SysBase)->TaskRunningSpinLock;
        }
        else if (&PrivExecBase(SysBase)->TaskSpinning == systemList) {
            D(name="TaskSpinning");
            sysListLock = &PrivExecBase(SysBase)->TaskSpinningLock;
        }
    }
    D(bug("[Exec:Lock] %s(), list='%s' (%p), flags=%d\n", __func__, name, systemList, flags));
    if (sysListLock)
    {
        EXEC_SPINLOCK_UNLOCK(sysListLock);

        if (flags & LOCKF_FORBID)
            Permit();
        if (flags & LOCKF_DISABLE)
            Enable();
    }
}

/* Locking mechanism for userspace */
void * ExecLock__AllocLock()
{
    spinlock_t *publicLock;

    D(bug("[Exec:Lock] %s()\n", __func__));

    if ((publicLock = (spinlock_t *)AllocMem(sizeof(spinlock_t), MEMF_ANY | MEMF_CLEAR)) != NULL)
    {
        EXEC_SPINLOCK_INIT(publicLock);
    }
    return publicLock;
}

int ExecLock__ObtainLock(void * lock, ULONG mode, ULONG flags)
{
    D(bug("[Exec:Lock] %s()\n", __func__));

    if (lock)
    {
        if (flags && LOCKF_DISABLE)
            Disable();
        if (flags && LOCKF_FORBID)
            Forbid();

        EXEC_SPINLOCK_LOCK(lock, NULL, mode);
        return TRUE;
    }
    return FALSE;
}

void ExecLock__ReleaseLock(void *lock, ULONG flags)
{
    D(bug("[Exec:Lock] %s()\n", __func__));

    if (lock)
    {
        EXEC_SPINLOCK_UNLOCK(lock);
        
        if (flags & LOCKF_FORBID)
            Permit();
        if (flags & LOCKF_DISABLE)
            Enable();
    }
}

void ExecLock__FreeLock(void *lock)
{
    D(bug("[Exec:Lock] %s()\n", __func__));

    if (lock)
    {
        FreeMem(lock, sizeof(spinlock_t));
    }
}

AROS_LH3 (int, ObtainSystemLock,
    AROS_LHA(struct List *, systemList, A0),
    AROS_LHA(ULONG, mode, D0),
    AROS_LHA(ULONG, flags, D1),
    struct ExecLockBase *, ExecLockBase, 1, ExecLock
)
{
    AROS_LIBFUNC_INIT

    D(bug("[Exec:Lock] %s()\n", __func__));

    return ExecLock__InternObtainSystemLock(systemList, mode, flags);

    AROS_LIBFUNC_EXIT
}

AROS_LH2 (void, ReleaseSystemLock,
    AROS_LHA(struct List *, systemList, A0),
    AROS_LHA(ULONG, flags, D1),
    struct ExecLockBase *, ExecLockBase, 2, ExecLock
)
{
    AROS_LIBFUNC_INIT

    D(bug("[Exec:Lock] %s()\n", __func__));

    ExecLock__InternReleaseSystemLock(systemList, flags);

    AROS_LIBFUNC_EXIT
}

AROS_LH0 (APTR, AllocLock,
    struct ExecLockBase *, ExecLockBase, 3, ExecLock
)
{
    AROS_LIBFUNC_INIT

    D(bug("[Exec:Lock] %s()\n", __func__));

    return ExecLock__AllocLock();

    AROS_LIBFUNC_EXIT
}

AROS_LH1 (void, FreeLock,
    AROS_LHA(APTR, lock, A0),
    struct ExecLockBase *, ExecLockBase, 4, ExecLock
)
{
    AROS_LIBFUNC_INIT

    D(bug("[Exec:Lock] %s()\n", __func__));

    ExecLock__FreeLock(lock);

    AROS_LIBFUNC_EXIT
}

AROS_LH3 (int, ObtainLock,
    AROS_LHA(APTR, lock, A0),
    AROS_LHA(ULONG, mode, D0),
    AROS_LHA(ULONG, flags, D1),
    struct ExecLockBase *, ExecLockBase, 5, ExecLock
)
{
    AROS_LIBFUNC_INIT

    D(bug("[Exec:Lock] %s()\n", __func__));

    return ExecLock__ObtainLock(lock, mode, flags);

    AROS_LIBFUNC_EXIT
}

AROS_LH2 (void, ReleaseLock,
    AROS_LHA(APTR, lock, A0),
    AROS_LHA(ULONG, flags, D1),
    struct ExecLockBase *, ExecLockBase, 6, ExecLock
)
{
    AROS_LIBFUNC_INIT

    D(bug("[Exec:Lock] %s()\n", __func__));

    ExecLock__ReleaseLock(lock, flags);

    AROS_LIBFUNC_EXIT
}

const APTR ExecLock__FuncTable[]=
{
    &AROS_SLIB_ENTRY(ObtainSystemLock,ExecLock,1),
    &AROS_SLIB_ENTRY(ReleaseSystemLock,ExecLock,2),
    &AROS_SLIB_ENTRY(AllocLock,ExecLock,3),
    &AROS_SLIB_ENTRY(FreeLock,ExecLock,4),
    &AROS_SLIB_ENTRY(ObtainLock,ExecLock,5),
    &AROS_SLIB_ENTRY(ReleaseLock,ExecLock,6),
    (void *)-1
};


APTR ExecLock__PrepareBase(struct MemHeader *mh)
{
    APTR ExecLockResBase;
    struct ExecLockBase *ExecLockBase;

    D(bug("[Exec:Lock] %s()\n", __func__));

    ExecLockResBase = Allocate(mh, sizeof(struct ExecLockBase) + sizeof(ExecLock__FuncTable));
    ExecLockBase = (struct ExecLockBase *)((IPTR)ExecLockResBase + sizeof(ExecLock__FuncTable));

    MakeFunctions(ExecLockBase, ExecLock__FuncTable, NULL);

    ExecLockBase->el_Node.ln_Name = "execlock.resource";
    ExecLockBase->el_Node.ln_Type = NT_RESOURCE;
    ExecLockBase->ObtainSystemLock = ExecLock__InternObtainSystemLock;
    ExecLockBase->ReleaseSystemLock = ExecLock__InternReleaseSystemLock;
    ExecLockBase->AllocLock = ExecLock__AllocLock;
    ExecLockBase->FreeLock = ExecLock__FreeLock;
    ExecLockBase->ObtainLock = ExecLock__ObtainLock;
    ExecLockBase->ReleaseLock = ExecLock__ReleaseLock;

    AddResource(ExecLockBase);

    return ExecLockBase;
}

#endif
