#ifndef __STDC_INTBASE_H
#define __STDC_INTBASE_H

/* This file replaces the header found in compiler/stdc */

#include "__crt_intbase.h"

#include <time.h>

struct StdCBase
{
    /* errno.h */
    int _errno;

    /* math.h */
    int _signgam;

    /* stdlib.h */
    int mb_cur_max;

    // /* signal.h */
    // unsigned char sigrunning, sigpending;
};

struct StdCIntBase
{
    struct StdCBase             lib;

    struct DosLibrary           *StdCDOSBase;           // Needed for strerror->Fault
    /* optional libs */
    struct IntuitionBase        *StdCIntuitionBase;

    /* stdlib.h */
    APTR                        mempool;
    unsigned int                srand_seed;

    /* time.h and it's functions */
    char                        timebuffer[26];
    struct tm                   tmbuffer;
    struct DateStamp            starttime;

    /* signal.c & co. */
    struct signal_func_data     *sigfunc_array;

    /* strerror.c */
    char                        *fault_buf;

    /* strtok.c */
    char                        *last;
};

struct StdCBase *__aros_getbase_StdCBase(void);

#endif
