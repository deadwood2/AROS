/*
    Copyright (C) 2021, The AROS Development Team. All rights reserved.

*/

#include <aros/symbolsets.h>
#include <proto/crt.h>

AROS_IMPORT_ASM_SYM(int, dummy, __includecrtprogramstartuphandling);

void __assert(const char * expr, const char * file, unsigned int line)
{
    __progonly_assert(expr, file, line);
}
