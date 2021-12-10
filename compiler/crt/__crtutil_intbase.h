/*
    Copyright (C) 2021, The AROS Development Team. All rights reserved.

*/
#ifndef __CRTUTIL_INTBASE_H
#define __CRTUTIL_INTBASE_H

#include <exec/libraries.h>
#include <dos/dos.h>
#include <libraries/crtutil.h>
#include <devices/timer.h>

struct CrtUtilIntBase
{
    struct CrtUtilBase          lib;

    /* optional libs */
    struct LocaleBase           *_LocaleBase;
    struct IntuitionBase        *_IntuitionBase;


    struct Device               *_TimerBase;
    struct timerequest          timereq;
    struct MsgPort              timeport;
};

struct CrtUtilBase * __aros_getbase_CrtUtilBase();

#endif //__CRTUTIL_INTBASE_H
