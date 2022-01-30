#ifndef _AROS_TYPES_TIMEVAL_S_H_
#define _AROS_TYPES_TIMEVAL_S_H_

#include <aros/cpu.h>
#include <bits/types.h>

/* Version of timeval structure that guarantees 32-bit sizes and can be used
    then 32-bit is enough */
struct timeval32
{
    unsigned AROS_32BIT_TYPE tv_secs;
    unsigned AROS_32BIT_TYPE tv_micro;
};

/* Note: after moving to 64-bit time, "struct timeval" will always be 64-bit
   for compability with POSIX-style structure. All new compiled code will be
   using 64-bit wide fields for both AROS-style and POSIX-style functions */

/* The following structure is combines two requirements:
        Linux-side POSIX requires 64-bit fields
        AROS-side is still operating on 32-bit fields
   In future AROS-will migrate to 64-bit fields and this structure will
   return to initial setup of two unions */

#ifndef NO_AROS_TIMEVAL

#if (__WORDSIZE == 64) && (!AROS_BIG_ENDIAN)
__extension__ struct timeval
{
    union
    {
        struct
        {
        unsigned AROS_32BIT_TYPE tv_secs;   /* AROS field */
        unsigned AROS_32BIT_TYPE tv_micro;  /* AROS field */
        };
        /* Seconds passed. */
        __time_t tv_sec;	                /* POSIX field */
    };
    /* Microseconds passed in the current second. */
    __suseconds_t tv_usec;                  /* POSIX field */
};
#else
#error Adjust struct timeval to your architecture
#endif

#endif /* !NO_AROS_TIMEVAL */

#endif /* ! _AROS_TYPES_TIMEVAL_S_H_ */
