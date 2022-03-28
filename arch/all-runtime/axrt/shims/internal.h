/*
    Copyright (C) 2020, The AROS Development Team.
*/

#ifndef SHIMS_INTERNAL_H
#define SHIMS_INTERNAL_H

#include <exec/types.h>
#include <exec/exec.h>
#include <dos/dos.h>

struct ShimsBase
{
    struct Library base;
    struct ExecBase *sysBase;
    struct Library *dosBase;
};

extern struct ShimsBase SB;

#define SysBase SB.sysBase
#define DOSBase SB.dosBase

LONG __shims_amiga2host(const char *func, const char *amigapath, char *hostpath);

#endif
