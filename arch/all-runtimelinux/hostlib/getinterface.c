/*
    Copyright (C) 1995-2024, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>

#include "hostinterface.h"
#include "hostlib_intern.h"

#include <dlfcn.h>

/*****************************************************************************

    NAME */
#include <proto/hostlib.h>

        AROS_LH3(APTR *, HostLib_GetInterface,

/*  SYNOPSIS */
        AROS_LHA(void *, handle, A0),
        AROS_LHA(const char **, symtable, A1),
        AROS_LHA(ULONG *, unresolved, A2),

/*  LOCATION */
        struct HostLibBase *, HostLibBase, 5, HostLib)

/*  FUNCTION
        Resolve array of symbols in the host operating system library.
        The result is a pointer to a dynamically allocated array of
        symbol values.

    INPUTS
        handle     - An opaque library handle provided by HostLib_Open()
        symbable   - A pointer to a NULL-terminated array of symbol names
        unresolved - An optional location where count of unresolved symbols
                        will be placed. Can be set to NULL to ignore it.

    RESULT
        A pointer to a dynamically allocated array of symbol values or NULL if
        empty symbol table was given.

    NOTES
        Note that the resulting array will always have as many entries as there
        are in symbol names array. It some symbols (or even all of them) fail
        to resolve, corresponding entries will be set to NULL. You may supply
        a valid unresolved pointer if you want to get unresolved symbols count.
        
        Even incomplete interface needs to be freed using HostLib_DropInterface().

        Resulting values are valid as long as the library is open. For portability
        sake it's advised to free interfaces before closing corresponding libraries.

        This function appeared in v2 of hostlib.resource.

    EXAMPLE

    BUGS

    SEE ALSO
        HostLib_GetPointer()

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    const char **c;
    ULONG cnt = 0;
    APTR *iface = NULL;

    for (c = symtable; *c; c++)
        cnt++;

    if (cnt)
    {
/* Workaround */
/* Trigger: call from battclock.resource -> HostLib_GetInterface with two symbols when loading gmplayer causes
    memory crash on host side when next resident (gfx.hidd) allocates next chunk of memory. Reason is unknown, but
    incresing memory to 32 bytes makes the problem go away. Note that AllocVec is doing other 16 bytes allocations
    and they don't seem to be causing issues */

        ULONG memsize = cnt * sizeof(APTR);
        if (memsize < 32) memsize = 32;
        iface = AllocVec(memsize, MEMF_ANY);
/* End */
        if (iface)
        {
            ULONG bad = 0;
            ULONG i;

            HOSTLIB_LOCK();
        
            for (i = 0; i < cnt; i++)
            {
                iface[i] = dlsym(handle, symtable[i]);
                AROS_HOST_BARRIER

                if (!iface[i])
                    bad++;
            }

            HOSTLIB_UNLOCK();

            if (unresolved)
                *unresolved = bad;
        }
    }

    return iface;

    AROS_LIBFUNC_EXIT
}
