/*
    Copyright (C) 2026, The AROS Development Team. All rights reserved.
*/

#ifndef _INTUITION_WINDOWS
#define _INTUITION_WINDOWS

void Intuition_Windows_init(struct IntuitionBaseV0 *abiv0IntuitionBase);

struct WindowProxy * wmGetByWindow(struct Window *native);
void syncWindowV0(struct WindowProxy *proxy);

#endif
