/*
    Copyright (C) 2021, The AROS Development Team. All rights reserved.

*/

#include <aros/symbolsets.h>
#include <proto/crt.h>

AROS_IMPORT_ASM_SYM(int, dummy, __includecrtprogramstartuphandling);

int execlp(const char *file, const char *arg, ...)
{
    va_list args;
    va_start(args, arg);
    int r = __progonly_execlp(file, arg, args);
    va_end(args);
    return r;
}
