/*
    Copyright (C) 2022, The AROS Development Team.
*/

#ifndef RT_STARTUP_H
#define RT_STARTUP_H

#include <exec/types.h>

typedef struct
{
    int version;
    LONG (*axrtentry)(STRPTR, LONG , struct ExecBase *);
    int argc;
    char **argv;
} __kick_start_arg_t;

typedef struct
{
    int version;
    int enhpathmode;
} __set_runtime_env_arg_t;


void __set_runtime_env(int __version, __set_runtime_env_arg_t *arg);
int __kick_start(int __version, __kick_start_arg_t *arg);

#endif
