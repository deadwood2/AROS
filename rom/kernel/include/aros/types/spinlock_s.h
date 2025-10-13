#ifndef _AROS_TYPES_SPINLOCK_S_H_
#define _AROS_TYPES_SPINLOCK_S_H_

#include <aros/cpu.h>
#include <exec/types.h>

/* Comment why spinlock_t had alignment of 128 - note that aligment of 128 is
 * not a requirement according to below, only that two spin locks don't reside
 * in the same 128 byte cache line - in other words, it is enought for every
 * structure that embeded a spinlock to be padded to be at least 128 bytes in
 * size. */

/* Align spinlock_t to 128 bytes so that each lock occupies a full cache line.
 * This avoids false sharing between CPUs, since otherwise multiple locks could
 * reside in the same line and contend unnecessarily. On x86_64 the natural
 * alignment would only be 16 bytes, so the attribute enforces cache-line isolation.
 */

typedef struct {
    union
    {
        volatile struct {
            unsigned int        readcount : 24;
            unsigned int        _pad2 : 3;
            unsigned int        write : 1;
            unsigned int        _pad1 : 3;
            unsigned int        updating : 1;
        } slock;
        volatile unsigned char  block[4];
        volatile unsigned int   lock;
    };
    unsigned int initialized;
    // The field s_Owner is set either to task owning the lock,
    // or NULL if the lock is free/read mode or was acquired in interrupt/supervisor mode
    void * s_Owner;
} spinlock_t;

#define SPINLOCK_UNLOCKED               0
#define SPINLOCKB_WRITE                 27
#define SPINLOCKB_UPDATING              31
#define SPINLOCKF_WRITE                 (1 << SPINLOCKB_WRITE)
#define SPINLOCKF_UPDATING              (1 << SPINLOCKB_UPDATING)

#define SPINLOCK_INIT_UNLOCKED          { SPINLOCK_UNLOCKED }
#define SPINLOCK_INIT_WRITE_LOCKED      { SPINLOCKF_WRITE }
#define SPINLOCK_INIT_READ_LOCKED(n)    { n }

#define SPINLOCK_MODE_READ              0
#define SPINLOCK_MODE_WRITE             1

#endif /* ! _AROS_TYPES_SPINLOCK_S_H_ */
