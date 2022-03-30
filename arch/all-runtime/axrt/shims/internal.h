/*
    Copyright (C) 2020, The AROS Development Team.
*/

#ifndef SHIMS_INTERNAL_H
#define SHIMS_INTERNAL_H

#include <exec/types.h>
#include <exec/exec.h>
#include <dos/dos.h>

#include "shimsbase.h"

#define SysBase SB.sysBase
#define DOSBase SB.dosBase

LONG __shims_amiga2host(const char *func, const char *amigapath, char *hostpath);

#endif
