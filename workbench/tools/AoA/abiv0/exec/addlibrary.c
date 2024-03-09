/*
    Copyright (C) 1995-2017, The AROS Development Team. All rights reserved.

    Desc: Add a library to the public list of libraries.
*/
#include <aros/debug.h>
#include "../include/exec/functions.h"

void abiv0_AddLibrary(struct LibraryV0 *library, struct ExecBaseV0 *SysBaseV0)
{
    ASSERT_VALID_PTR(library);

    /* Just in case the user forgot them */
    library->lib_Node.ln_Type=NT_LIBRARY;
    library->lib_Flags|=LIBF_CHANGED;

    /* Build checksum for library vectors */
    // SumLibrary(library);

    /* Arbitrate for the library list */
    // EXEC_LOCK_LIST_WRITE_AND_FORBID(&SysBase->LibList);
    /* And add the library */
    abiv0_Enqueue(&SysBaseV0->LibList,&library->lib_Node,SysBaseV0);
    /* We're done with midifying the LibList */
    // EXEC_UNLOCK_LIST_AND_PERMIT(&SysBase->LibList);
    /*
     * When debug.library is added, open it and cache its base instantly.
     * We do it because symbol lookup routines can be called in a system crash
     * condition, where calling OpenLibrary() can be dangerous.
     */
    // if (!strcmp(library->lib_Node.ln_Name, "debug.library"))
    // {
    //     /* Don't bother searching for just added library, just call open vector */
    //     DebugBase = AROS_LVO_CALL1(struct Library *,
    //                                AROS_LCA(ULONG, 0, D0),
    //                                struct Library *, library, 1, lib);

    //     DINIT("%s added, base 0x%p", library->lib_Node.ln_Name, DebugBase);
    // }

    // AROS_COMPAT_SETD0(library);
} /* AddLibrary */
