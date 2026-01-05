/*
    Copyright (C) 2026, The AROS Development Team. All rights reserved.
*/

#ifndef ABIV0_INTUITION_PROXY_STRUCTURES_H
#define ABIV0_INTUITION_PROXY_STRUCTURES_H

struct WindowProxy
{
    struct WindowV0 base;
    struct Window   *native;
};

#endif
