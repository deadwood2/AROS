/*
    Copyright (C) 1995-2017, The AROS Development Team. All rights reserved.

    Desc: Open a library.
*/

#include <aros/debug.h>

#include "../include/exec/functions.h"
#include "../include/aros/asm.h"
#include "../include/aros/cpu.h"

struct LibraryV0 * abiv0_OpenLibrary(CONST_STRPTR libName, ULONG version, struct ExecBaseV0 *SysBaseV0)
{
    struct LibraryV0 * library;

    D(bug("OpenLibrary(\"%s\", %ld)", libName, version));

    /* Arbitrate for the library list */
    // EXEC_LOCK_LIST_READ_AND_FORBID(&SysBase->LibList);
    
    /* Look for the library in our list */
    library = (struct LibraryV0 *) abiv0_FindName (&SysBaseV0->LibList, libName, SysBaseV0);

    // EXEC_UNLOCK_LIST(&SysBase->LibList);

    /* Something found ? */
    if(library!=NULL)
    {
        /* Check version */
        if(library->lib_Version>=version)
        {
            /* Call Open vector */
            __asm__ volatile (
                "subq $8, %%rsp\n"
                "movl %0, %%eax\n"
                "movl %%eax, 4(%%rsp)\n"
                "movl %2, %%eax\n"
                "movl %%eax, (%%rsp)\n"
                "movl %1, %%eax\n"
                ENTER32
                "call *%%eax\n"
                ENTER64
                "addq $8, %%rsp\n"
                "movl %%eax, %0\n"
            :"+m" (library):"m"(__AROS_GETVECADDRV0(library, 1)), "m"(version): "%rax", "%rcx");
        }
        else
        {
            D(bug("Version mismatch (have %ld, wanted %ld)", library->lib_Version, version));
            library = NULL;
        }
    }

    /*
     *  We cannot handle loading libraries from disk. But this is taken
     *  care of by dos.library (well lddemon really) replacing this
     *  function with a SetFunction() call.
     */

    /* All done. */
    // Permit();

    D(bug("OpenLibrary(\"%s\", %ld) = %p", libName, version, library));
    return library;

} /* OpenLibrary */
