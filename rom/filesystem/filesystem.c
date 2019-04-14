/*
    Copyright © 1995-2013, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
*/

#include <aros/symbolsets.h>
#include <resources/filesysres.h>
#include <proto/alib.h>
#include <proto/exec.h>

const LONG __aros_libreq_SysBase = 0L;

static int FileSystemInit(struct FileSysResource *FileSystemBase)
{
    FileSystemBase->fsr_Creator = "AROS Development Team";
    NewList(&FileSystemBase->fsr_FileSysEntries);

    return TRUE;
}

ADD2INITLIB(FileSystemInit, 0);
