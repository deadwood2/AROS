/*
    Copyright (C) 2021, The AROS Development Team. All rights reserved.

*/

#include <aros/symbolsets.h>
#include <proto/crt.h>

AROS_IMPORT_ASM_SYM(int, dummy, __includecrtprogramstartuphandling);

void _Exit(int code)
{
    __progonly__Exit(code);
}

void _exit(int code)
{
    _Exit(code);
}
