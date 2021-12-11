/*
    Copyright (C) 2008-2009, The AROS Development Team. All rights reserved.
*/
#ifndef __EXEC_H
#define __EXEC_H

#include <stdarg.h>

struct __exec_context       /* Allocated from selfpool */
{
    APTR selfpool;

    char *exec_args;        /* Allocated form selfpool */
    char *exec_taskname;    /* Allocated from selfpool */
    BPTR exec_seglist;
};

struct __exec_context *__progonly_exec_prepare(const char *filename, int searchpath, char *const argv[], char *const envp[]);
void __progonly_exec_do(struct __exec_context *ectx);
char *const *__progonly_exec_valist2array(const char *arg1, va_list list);
void __progonly_exec_cleanup_array();

#endif //__EXEC_H
