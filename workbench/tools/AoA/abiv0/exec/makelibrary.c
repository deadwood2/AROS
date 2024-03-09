/*
    Copyright (C) 1995-2011, The AROS Development Team. All rights reserved.

    Desc: Make a library ready for use.
*/

#include <aros/debug.h>

#include "../include/exec/functions.h"
#include "../include/aros/cpu.h"

struct LibraryV0 * abiv0_MakeLibrary(APTR32 funcInit, APTR32 structInit, APTR32 libInit,
    ULONG dataSize, BPTR segList, struct ExecBaseV0 * SysBaseV0)

{
    struct LibraryV0 *library;
    ULONG negsize;
    ULONG count = 0;

    D(bug("MakeLibrary: functions table at 0x%p, data size is %lu", funcInit, dataSize));

    /* Can't do the following check on machines like x86, because 0x????ffff is
       a valid address for code there! */
    
#if defined(__mc68000) || defined(__ppc__) || defined(__powerpc__)
    /* Calculate the jumptable's size */
    if (*(WORD *)funcInit==-1)
    {
        /* Count offsets */
        WORD *fp=(WORD *)funcInit+1;

        while(*fp++!=-1)
            count++;
        D(bug("Table contains %lu relative offsets", count));
    }
    else
#endif
    {
        /* Count function pointers */
        APTR32 *fp=(APTR32*)(IPTR)funcInit;

        while(*fp++!=(APTR32)-1)
            count++;

        D(bug("Table contains %lu absolute pointers", count));
    }

    /* Align library base */
    negsize=AROS_ALIGN(count * LIB_VECTSIZEV0);

    /* Allocate memory */
    library=(struct LibraryV0 *)abiv0_AllocMem(dataSize+negsize,MEMF_PUBLIC|MEMF_CLEAR,SysBaseV0);
    D(bug("Allocated vector table at 0x%p, size is %lu", library, negsize));

    /* And initilize the library */
    if (library!=NULL)
    {
        /* Get real library base */
        library=(struct LibraryV0 *)((char *)library+negsize);

        /* Build jumptable */
    #if defined(__mc68000) || defined(__ppc__) || defined(__powerpc__)
        if(*(WORD *)funcInit==-1)
            /* offsets */
            MakeFunctions(library,(WORD *)funcInit+1,(WORD *)funcInit);
        else
    #endif
            /* function pointers */
            abiv0_MakeFunctions(library,funcInit,(APTR32)(IPTR)NULL,SysBaseV0);

        /* Write sizes */
        library->lib_NegSize=negsize;
        library->lib_PosSize=dataSize;

        /* Create structure, do not clear struct Library */
        if(structInit!=(APTR32)(IPTR)NULL)
asm("int3");
            // InitStruct(structInit,library,0);

        /* Call init vector */
        if (libInit!=(APTR32)(IPTR)NULL)
        {
            D(bug("Calling init function 0x%p", libInit));

asm("int3");
            // library=AROS_UFC3(struct Library *, libInit,
            //     AROS_UFCA(struct Library *,  library, D0),
            //     AROS_UFCA(BPTR,              segList, A0),
            //     AROS_UFCA(struct ExecBase *, SysBase, A6)
            // );
        }
    }

    /* All done */
    D(bug("Created library 0x%p", library));
    return library;

} /* MakeLibrary */

