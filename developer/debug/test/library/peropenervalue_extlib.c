/*
    Copyright (C) 2023, The AROS Development Team. All rights reserved.

    This file simulates third party code that in general is a library which uses global variables. When this code is
    compiled to shared objects on other systems, the global values are per-process and are not shared. When using
    this code directly under AROS by compiling it into a .library, global values are shared between all openers of
    library. To overcome this, this code is linked into program that is LoadSeg(ed) each time the library is opened.
    This means global value become isolated per opener, but also unfortunatelly code is duplicated per each opener.
*/

int globalValue = 8;

void SetGlobalValue(int val)
{
    globalValue = val;
}

int GetGlobalValue()
{
    return globalValue;
}
