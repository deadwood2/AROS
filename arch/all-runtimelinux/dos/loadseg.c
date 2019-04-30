/*
    Copyright © 1995-2011, The AROS Development Team. All rights reserved.
    $Id$

    Desc: DOS function LoadSeg()
    Lang: english
*/

#include <aros/asmcall.h>
#include <aros/config.h>
#include <dos/dos.h>
#include <dos/stdio.h>
#include <proto/dos.h>
#include <aros/debug.h>
#include "dos_intern.h"

#include <stdlib.h>
#include <dlfcn.h>
static void strreplace(STRPTR target, CONST_STRPTR from, CONST_STRPTR to)
{
    STRPTR pos = strstr(target, from);
    if (pos != NULL)
    {
        TEXT buff[512] = {0};
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

/*****************************************************************************

    NAME */
#include <proto/dos.h>

        AROS_LH1(BPTR, LoadSeg,

/*  SYNOPSIS */
        AROS_LHA(CONST_STRPTR, name, D1),

/*  LOCATION */
        struct DosLibrary *, DOSBase, 25, Dos)

/*  FUNCTION
        Loads an executable file into memory. Each hunk of the loadfile
        is loaded into its own memory section and a handle on all of them
        is returned. The segments can be freed with UnLoadSeg().

    INPUTS
        name - NUL terminated name of the file.

    RESULT
        Handle to the loaded executable or NULL if the load failed.
        IoErr() gives additional information in that case.

    NOTES
        This function is built on top of InternalLoadSeg()

    EXAMPLE

    BUGS

    SEE ALSO
        UnLoadSeg()

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    TEXT path[512] = {0};
    STRPTR root = getenv("AROSRUNTIME_ROOT");
    if (root) strncat(path, root, strlen(root));
    strncat(path, name, strlen(name));
    strreplace(path, "SYS:", "");
    strreplace(path, "libs:datatypes", "Classes/DataTypes");
    strreplace(path, "/libs/", "/Libs/");
    strreplace(path, "/datatypes/", "/DataTypes/");
    strreplace(path, "/gadgets/","/Classes/Gadgets/");
    // TODO: rethink, should LoadSeg accept only AROS paths and convert them to ROOT: absolute paths via Lock/NameFromLock?
    // and then to Linux paths? This would remove needs to certain hacks on the path variable
    // above
    void *__so_handle = dlopen(path, RTLD_LAZY);
    return (BPTR)__so_handle;

    AROS_LIBFUNC_EXIT
} /* LoadSeg */
