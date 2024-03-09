/*
    Copyright (C) 1995-2021, The AROS Development Team. All rights reserved.

    Desc: Create the jumptable for a shared library or a device.
*/

#include <aros/debug.h>

#include "../include/exec/structures.h"
#include "../include/aros/cpu.h"

AROS_LD3(void, CacheClearE,
         AROS_LHA(APTR, address, A0),
         AROS_LHA(ULONG, length, D0),
         AROS_LHA(ULONG, caches, D1),
         struct ExecBase *, SysBase, 107, Exec);

void _aros_not_implemented(char *lvo)
{
    asm("int3");
}

ULONG  abiv0_MakeFunctions(APTR target, APTR32 functionArray, APTR32 funcDispBase, struct ExecBaseV0 *SysBaseV0)
{
    long n;
    APTR lastvec;

    D(bug("MakeFunctions(0x%p, 0x%p, 0x%p)", target, functionArray, funcDispBase));

    n = 1;

    if (funcDispBase!=(APTR32)(IPTR)NULL)
    {
asm("int3");
        // /* If FuncDispBase is non-NULL it's an array of relative offsets */
        // WORD *fp=(WORD *)functionArray;

        // /* -1 terminates the array */
        // while(*fp!=-1)
        // {
        //     /* Decrement vector pointer by one and install vector */
        //     __AROS_INITVEC(target,n);
        //     if (*fp)
        //         __AROS_SETVECADDR(target,n,(APTR)((IPTR)funcDispBase + *fp));

        //     /* Use next array entry */
        //     fp++;
        //     n++;
        // }
    }
    else
    {
        /* If FuncDispBase is NULL it's an array of function pointers */
        APTR32 *fp=(APTR32 *)(IPTR)functionArray;

        /* -1 terminates the array */
        while(*fp!=(APTR32)-1)
        {
            /* Decrement vector pointer by one and install vector */
            __AROS_INITVECV0(target,n);
            if (*fp)
                __AROS_SETVECADDRV0(target,n,*fp);

            /* Use next array entry */
            fp++;
            n++;
        }
    }

    lastvec = __AROS_GETJUMPVECV0(target,n);
    n = (IPTR)target-(IPTR)lastvec;

#ifdef __AROS_USE_FULLJMP
    /* Clear instruction cache for the whole jumptable. We need to do it only if
       the jumptable actually contains executable code. __AROS_USE_FULLJMP must
       be defined in cpu.h in this case.

       Note that we call this function directly because MakeFunctions() is also
       used for building ExecBase itself. */
    if (SysBase->LibNode.lib_Node.ln_Type != NT_LIBRARY) {
        AROS_CALL3NR(void, AROS_SLIB_ENTRY(CacheClearE, Exec, 107),
              AROS_UFCA(APTR, lastvec, A0),
              AROS_UFCA(ULONG, n, D0),
              AROS_UFCA(ULONG, CACRF_ClearI|CACRF_ClearD, D1),
              struct ExecBase *, SysBase);
    } else {
        /* call CacheClearE() indirectly if SysBase is already valid.
         * CacheClearE may have been SetFunction()'d for specific CPU type.
         */
        CacheClearE(lastvec, n, CACRF_ClearI|CACRF_ClearD);
    }
#endif

    /* Return size of jumptable */
    D(bug("Created %lu vectors", n));
    return n;

} /* MakeFunctions */

