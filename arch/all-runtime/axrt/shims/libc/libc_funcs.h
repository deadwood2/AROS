/*
    Copyright (C) 2020, The AROS Development Team.
*/

#ifndef LIBC_FUNCS_H
#define LIBC_FUNCS_H

#include <stdio.h>

typedef unsigned short mode_t;

struct libcfuncs
{
    FILE *(*fopen64)(const char * restrict path, const char * restrict mode);
    int (*mkdir)(const char * path, mode_t mode);
    int (*open64)(const char * pathname, int oflag);
    int (*__xstat64)(int ver, const char * path, void * buf);
    int (*__lxstat64)(int ver, const char * path, void * buf);
};

extern struct libcfuncs __libcfuncs;

#endif
