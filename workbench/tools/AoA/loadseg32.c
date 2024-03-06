/*
    Copyright (C) 1995-2011, The AROS Development Team. All rights reserved.

    Desc: DOS function LoadSeg()
*/

#include <aros/asmcall.h>
#include <dos/dos.h>
#include <dos/stdio.h>
#include <proto/dos.h>
#include <aros/debug.h>
#include "dos_intern.h"

static AROS_UFH4(LONG, ReadFunc,
        AROS_UFHA(BPTR, file,   D1),
        AROS_UFHA(APTR, buffer, D2),
        AROS_UFHA(LONG, length, D3),
        AROS_UFHA(struct DosLibrary *, DOSBase, A6)
)
{
    AROS_USERFUNC_INIT

    return FRead(file, buffer, 1, length);

    AROS_USERFUNC_EXIT
}

static AROS_UFH4(LONG, SeekFunc,
        AROS_UFHA(BPTR, file,  D1),
        AROS_UFHA(LONG,   pos, D2),
        AROS_UFHA(LONG,  mode, D3),
        AROS_UFHA(struct DosLibrary *, DOSBase, A6)
)
{
    AROS_USERFUNC_INIT

    return Seek(file, pos, mode);

    AROS_USERFUNC_EXIT
}


static AROS_UFH3(APTR, AllocFunc,
        AROS_UFHA(ULONG, length, D0),
        AROS_UFHA(ULONG, flags,  D1),
        AROS_UFHA(struct ExecBase *, SysBase, A6)
)
{
    AROS_USERFUNC_INIT

    return AllocMem(length, flags);

    AROS_USERFUNC_EXIT
}

static AROS_UFH3(void, FreeFunc,
        AROS_UFHA(APTR, buffer, A1),
        AROS_UFHA(ULONG, length, D0),
        AROS_UFHA(struct ExecBase *, SysBase, A6)
)
{
    AROS_USERFUNC_INIT

    FreeMem(buffer, length);

    AROS_USERFUNC_EXIT
}

BPTR InternalLoadSeg32(BPTR fh, BPTR table, LONG_FUNC *funcarray, LONG *stack, struct DosLibrary *DOSBase);

BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase)
{
    BPTR file, segs=0;
    SIPTR err;
    LONG_FUNC FunctionArray[] = {
        (LONG_FUNC)ReadFunc,
        (LONG_FUNC)AllocFunc,
        (LONG_FUNC)FreeFunc,
        (LONG_FUNC)SeekFunc,    /* Only needed for ELF */
    };

    /* Open the file */
    D(bug("[LoadSeg] Opening '%s'...\n", name));
    file = Open (name, MODE_OLDFILE);

    if (file)
    {
        D(bug("[LoadSeg] Loading '%s'...\n", name));

        SetVBuf(file, NULL, BUF_FULL, 4096);
        segs = InternalLoadSeg32(file, BNULL, FunctionArray, NULL, DOSBase);
        /* We cache the IoErr(), since Close() will alter it */
        err = IoErr();

        D(if (segs == BNULL)
            bug("[LoadSeg] Failed to load '%s'\n", name));
#if 0
        /* overlayed executables return -segs and handle must not be closed */
        if ((LONG)segs > 0)
            Close(file);
        else
            segs = (BPTR)-((LONG)segs);
#else
        Close(file);
#endif
        SetIoErr(err);
    }

    D(bug("[LoadSeg] Returns '%p', IoErr=%d\n", segs, IoErr()));
  

    /* And return */
    return segs;

} /* LoadSeg */
