/*
    Copyright (C) 1995-2020, The AROS Development Team. All rights reserved.

    Desc: Code to dynamically load ELF executables
*/

#include <aros/debug.h>

#include <proto/dos.h>

#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "internalloadseg.h"
#include "include/loadseg.h"

struct hunk
{
    ULONG size;
    BPTR  next;
    struct FullJumpVec data;
    ULONG segmagic;
    APTR __so_handle;
} __attribute__((packed));

#define BPTR2HUNK(bptr) ((struct hunk *)((void *)bptr - offsetof(struct hunk, next)))
#define HUNK2BPTR(hunk) MKBADDR(&hunk->next)

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

BPTR InternalLoadSeg_ELF_DYN
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
    struct hunk *hunk = NULL;

    NameFromFH(file, path, BUFFSIZE);

    D(bug("LoadSeg: %s -> ", path));

    /* FIXME: Won't work with volumes other than ROOT */
    strreplace(path, "ROOT:", "/");
    D(bug("%s", path));

    __so_handle = dlopen(path, RTLD_LAZY);
    if (!__so_handle)
    {
        bug("%s\n", dlerror());
        return BNULL;
    }

    hunk = ilsAllocMem(sizeof(struct hunk), MEMF_PUBLIC | MEMF_CLEAR);
    if (hunk)
    {
        hunk->next = 0;
        hunk->size = sizeof(struct hunk);
        hunk->__so_handle = __so_handle;
        hunk->segmagic = SEGMAGIC_DYN;

        void * __startup_entry = dlsym(__so_handle, "__startup_entry");

        __AROS_SET_FULLJMP(&hunk->data, __startup_entry);

         register_elf(file, HUNK2BPTR(hunk), NULL, NULL, DOSBase);
    }

    D(bug("\n"));

    return HUNK2BPTR(hunk);
}
