/*
    Copyright (C) 2026, The AROS Development Team. All rights reserved.
*/

#ifndef _TIMER_INIT
#define _TIMER_INIT

#include "../../include/exec/proxy_structures.h"

extern struct DeviceProxy *abiv0TimerBase;

void init_timer(struct ExecBaseV0 *SysBaseV0);
void exit_timer(struct ExecBaseV0 *SysBaseV0);

#endif
