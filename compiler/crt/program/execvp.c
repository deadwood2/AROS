/*
    Copyright (C) 2021, The AROS Development Team. All rights reserved.

*/

#include <aros/symbolsets.h>
#include <proto/crt.h>

AROS_IMPORT_ASM_SYM(int, dummy, __includecrtprogramstartuphandling);

int execvp(const char *path, char *const argv[])
{
    return __progonly_execvp(path, argv);
}
