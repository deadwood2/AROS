/*
    Copyright (C) 2021, The AROS Development Team. All rights reserved.

*/

#include <aros/symbolsets.h>
#include <proto/crt.h>

AROS_IMPORT_ASM_SYM(int, dummy, __includecrtprogramstartuphandling);

int execve(const char *path, char *const argv[], char *const envp[])
{
    return __progonly_execve(path, argv, envp);
}
