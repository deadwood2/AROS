/*
    Copyright (C) 2020-2022, The AROS Development Team.
*/

#ifndef SHIMS_SHIMSBASE_H
#define SHIMS_SHIMSBASE_H

#include <exec/types.h>
#include <exec/exec.h>
#include <dos/dos.h>

struct ShimsBase
{
    struct Library base;
    struct ExecBase *sysBase;
    struct Library *dosBase;

    BOOL            sb_debugpath;
    BOOL            sb_EnhPathMode;
};

extern struct ShimsBase SB;

#endif
