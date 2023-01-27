/*
    Copyright (C) 2023, The AROS Development Team. All rights reserved.

    This is a program that is wrapping the external library and is LoadSeg(ed) at each open of .library. The program
    needs to be compiled without standard AROS startup code as it provides it's own below. The only function of below
    code is to fill in jump table that is then used by .library to proxy the calls.
*/

#include <aros/system.h>

#include "peropenervalue_extlib.h"
#include "peropenervalue_extlibseg.h"

struct ExtLibSegJumpTable jt;

__startup struct ExtLibSegJumpTable * Init()
{
    jt.SetGlobalValue = SetGlobalValue;
    jt.GetGlobalValue = GetGlobalValue;
    return &jt;
}
