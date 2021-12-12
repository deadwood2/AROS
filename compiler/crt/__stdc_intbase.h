#ifndef __STDC_INTBASE_H
#define __STDC_INTBASE_H

/* This file replaces the header found in compiler/stdc */

#include "__crt_intbase.h"

#include <time.h>

struct StdCBase
{
    struct Library lib;

    /* ctype.h */
    const unsigned short int * __ctype_b;
    const unsigned char * __ctype_toupper;
    const unsigned char * __ctype_tolower;

    /* errno.h */
    int _errno;

    /* math.h */
    int _signgam;

    /* stdlib.h */
    int mb_cur_max;

    /* signal.h */
    unsigned char sigrunning, sigpending;
};

struct StdCIntBase
{
    struct StdCBase             StdCBase;
    struct StdCIntBase          *StdCRootBase;
    BPTR                        StdCSegList;

    struct DosLibrary           *StdCDOSBase;           // Needed for strerror->Fault
    /* optional libs */
    struct LocaleBase           *StdCLocaleBase;
    struct IntuitionBase        *StdCIntuitionBase;
    struct Device               *StdCTimerBase;

    /* stdlib.h */
    APTR                        mempool;
    unsigned int                srand_seed;

    /* time.h and it's functions */
    struct timerequest          timereq;
    struct MsgPort              timeport;
    char                        timebuffer[26];
    struct tm                   tmbuffer;
    struct DateStamp            starttime;

    /* signal.c & co. */
    struct signal_func_data     *sigfunc_array;

    /* strerror.c */
    char                        *fault_buf;

    /* strtok.c */
    char                        *last;

    /* private for fenv.c/softfloat implementations */
    APTR                        __fe_private;
    APTR                        __fe_dfl_env;
    APTR                        __fe_nomask_env;
    int                         __fe_round;
};

struct StdCBase *__aros_getbase_StdCBase(void);

#endif
