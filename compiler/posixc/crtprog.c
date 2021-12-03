/*
    Copyright (C) 2021, The AROS Development Team. All rights reserved.

*/

#include <aros/debug.h>

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