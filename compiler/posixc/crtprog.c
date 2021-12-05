/*
    Copyright (C) 2021, The AROS Development Team. All rights reserved.

*/

#include <aros/debug.h>

#include <aros/startup.h>
#include <proto/exec.h>
#include <aros/symbolsets.h>

#include <proto/crtext.h>


void abort (void)
{
    __progonly_abort();
}

int atexit(void (*func)(void))
{
    __progonly_atexit(func);
}

void exit (int code)
{
    __progonly_exit(code);
}

void _Exit(int code)
{
    __progonly__Exit(code);
}

void _exit(int code)
{
    _Exit(code);
}

int execl(const char *path, const char *arg, ...)
{
    va_list args;
    va_start(args, arg);
    int r = __progonly_execl(path, arg, args);
    va_end(args);
    return r;
}

int execv(const char *path, char *const argv[])
{
    return __progonly_execv(path, argv);
}

int execve(const char *path, char *const argv[], char *const envp[])
{
    return __progonly_execve(path, argv, envp);
}

int execvp(const char *path, char *const argv[])
{
    return __progonly_execvp(path, argv);
}

static void __progonly_startup(struct ExecBase *SysBase)
{
    jmp_buf exitjmp;

    if (setjmp(exitjmp) == 0)
    {
        D(bug("%s: setjmp() called\n", __func__));

        /* Tell library a program using it has started */
        __progonly_program_startup(exitjmp, (int *)&__startup_error);
        D(bug("%s: Library startup called\n", __func__));

        __startup_entries_next();
    }
    else
    {
        D(bug("%s: setjmp() return from longjmp\n", __func__));
    }

    /* Tell library program has reached the end */
    __progonly_program_end();

    D(bug("%s: Leave\n", __func__));
}

ADD2SET(__progonly_startup, PROGRAM_ENTRIES, 0);
