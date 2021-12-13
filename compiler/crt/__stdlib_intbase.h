/*
    Copyright (C) 2021, The AROS Development Team. All rights reserved.

*/
#ifndef __STDLIB_INTBASE_H
#define __STDLIB_INTBASE_H

#include <exec/libraries.h>
#include <dos/dos.h>
#include <libraries/stdlib.h>
#include <devices/timer.h>

struct StdlibIntBase
{
    struct StdlibBase          lib;

    /* optional libs */
    struct LocaleBase           *_LocaleBase;
    struct IntuitionBase        *_IntuitionBase;


    struct Device               *_TimerBase;
    struct timerequest          timereq;
    struct MsgPort              timeport;
};

struct StdlibBase * __aros_getbase_StdlibBase();

#endif //__STDLIB_INTBASE_H
