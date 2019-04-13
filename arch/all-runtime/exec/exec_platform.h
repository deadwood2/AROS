#ifndef __EXEC_PLATFORM_H
#define __EXEC_PLATFORM_H

struct Exec_PlatformData
{
    APTR HostLibBase;
};

#include <aros/types/spinlock_s.h>
#include <pthread.h>
extern void __spinlock_init(spinlock_t *, APTR);
extern APTR __spinlock_lock(spinlock_t *, APTR, ULONG, APTR);
extern void __spinlock_unlock(spinlock_t *, APTR);
#define EXEC_SPINLOCK_INIT(a)           __spinlock_init((a), NULL)
#define EXEC_SPINLOCK_LOCK(a,b,c)       __spinlock_lock((a), (b), (c), NULL)
#define EXEC_SPINLOCK_UNLOCK(a)         __spinlock_unlock((a), NULL)

#define IDNESTCOUNT_INC
#define IDNESTCOUNT_DEC
#define TDNESTCOUNT_INC
#define TDNESTCOUNT_DEC
#define FLAG_SCHEDQUANTUM_CLEAR         AROS_ATOMIC_AND(SysBase->SysFlags, ~SFF_QuantumOver)
#define FLAG_SCHEDQUANTUM_SET           AROS_ATOMIC_OR(SysBase->SysFlags, SFF_QuantumOver)
#define FLAG_SCHEDSWITCH_CLEAR          AROS_ATOMIC_AND(SysBase->AttnResched, ~ARF_AttnSwitch)
#define FLAG_SCHEDSWITCH_SET            AROS_ATOMIC_OR(SysBase->AttnResched, ARF_AttnSwitch)
#define FLAG_SCHEDDISPATCH_CLEAR        AROS_ATOMIC_AND(SysBase->AttnResched, ~ARF_AttnDispatch)
#define FLAG_SCHEDDISPATCH_SET          AROS_ATOMIC_OR(SysBase->AttnResched, ARF_AttnDispatch)

#define SCHEDQUANTUM_SET(val)           ((void)(val))
#define SCHEDQUANTUM_GET                (0)
#define SCHEDELAPSED_SET(val)           ((void)(val))
#define SCHEDELAPSED_GET                (0)
#define IDNESTCOUNT_GET                 (-1)
#define IDNESTCOUNT_SET(val)            ((void)val)
#define TDNESTCOUNT_GET                 (-1)
#define TDNESTCOUNT_SET(val)            ((void)(val))
#define FLAG_SCHEDQUANTUM_ISSET         (SysBase->SysFlags & SFF_QuantumOver)
#define FLAG_SCHEDSWITCH_ISSET          (SysBase->AttnResched & ARF_AttnSwitch)
#define FLAG_SCHEDDISPATCH_ISSET        (SysBase->AttnResched & ARF_AttnDispatch)

extern pthread_key_t KEY_THISTASK;
#define GET_THIS_TASK                   ((struct Task *)pthread_getspecific(KEY_THISTASK))
#define SET_THIS_TASK(x)                (pthread_setspecific(KEY_THISTASK, x))

#endif /* __EXEC_PLATFORM_H */
