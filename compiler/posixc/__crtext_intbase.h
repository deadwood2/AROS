/*
    Copyright (C) 2021, The AROS Development Team. All rights reserved.

*/
#ifndef __CRTEXT_INTBASE_H
#define __CRTEXT_INTBASE_H

#include <exec/libraries.h>
#include <dos/dos.h>
#include <devices/timer.h>

#include "../stdc/__stdc_intbase.h"
#include "__posixc_intbase.h"

struct CrtExtIntBase
{
    struct Library          lib;
    struct StdCIntBase      *StdCBase;
    struct PosixCIntBase    *PosixCBase;
};

struct Library * __aros_getbase_CrtExtBase();

#endif //__CRTEXT_INTBASE_H
