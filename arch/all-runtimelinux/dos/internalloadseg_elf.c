/*
    Copyright (C) 1995-2019, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Code to dynamically load ELF executables
    Lang: english
*/
#include <proto/dos.h>
#include <aros/debug.h>

#include <stdlib.h>
#include <dlfcn.h>
#define BUFFSIZE    (512)
static void strreplace(STRPTR target, CONST_STRPTR from, CONST_STRPTR to)
{
    STRPTR pos = strstr(target, from);
    if (pos != NULL)
    {
        TEXT buff[BUFFSIZE * 2] = {0};
        ULONG targetlen = strlen(target);
        ULONG headlen = pos - target;
        ULONG taillen = targetlen - strlen(from) - headlen;

        // add head
        strncat(buff, target, headlen);

        // add to
        strncat(buff, to, strlen(to));

        // add tail
        strncat(buff, pos + strlen(from), taillen);

        // copy back
        strncpy(target, buff, strlen(buff));
        target[strlen(buff)] = 0;
    }
}

BPTR InternalLoadSeg_ELF
(
    BPTR               file,
    BPTR               table __unused,
    SIPTR             *funcarray,
    LONG              *stack __unused,
    struct DosLibrary *DOSBase
)
{
    TEXT path[BUFFSIZE] = {0};
    void *__so_handle = NULL;

    NameFromFH(file, path, BUFFSIZE);

    D(bug("LoadSeg: %s -> ", path));

    /* FIXME: HACK: Executables now load as well, and each executable re-initializes kickstart */
    if (strstr(path, "ExecuteStartup") != NULL) return BNULL;

    /* FIXME: Won't work with volumes other than ROOT */
    strreplace(path, "ROOT:", "/");
    D(bug("%s", path));

    __so_handle = dlopen(path, RTLD_LAZY);

    D(bug("\n"));

    return (BPTR)__so_handle;
}
