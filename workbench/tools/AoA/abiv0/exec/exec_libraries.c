/*
    Copyright (C) 2024-2026, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <aros/debug.h>
#include <exec/rawfmt.h>

#include <string.h>

#include "../include/aros/cpu.h"
#include "../include/aros/proxy.h"
#include "../include/aros/call32.h"
#include "../include/exec/functions.h"
#include "../include/exec/structures.h"

extern ULONG *execfunctable;
extern struct DosLibraryV0 *abiv0DOSBase;
BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);

struct ResidentV0 * findResident(BPTR seg, CONST_STRPTR name)
{
    /* we may not have any extension fields */
    const int sizeofresident = offsetof(struct ResidentV0, rt_Init) + sizeof(APTR);

    while(seg)
    {
        STRPTR addr = (STRPTR)((IPTR)BADDR(seg) - sizeof(ULONG));
        ULONG size = *(ULONG *)addr;

        for(
            addr += sizeof(BPTR) + sizeof(ULONG),
                size -= sizeof(BPTR) + sizeof(ULONG);
            size >= sizeofresident;
            size -= 2, addr += 2
        )
        {
            struct ResidentV0 *res = (struct ResidentV0 *)addr;
            if(    res->rt_MatchWord == RTC_MATCHWORD
                && res->rt_MatchTag == (APTR32)(IPTR)res )
            {
                if ((name != NULL) && (strcmp((char *)(IPTR)res->rt_Name, name) != 0))
                    continue;

                return res;
            }
        }
        seg = *(BPTR *)BADDR(seg);
    }
}

static void int_Enqueue(struct ListV0 *list, struct NodeV0 *node, struct ExecBaseV0 *SysBaseV0)
{
    struct NodeV0 * next;

    ASSERT(list != NULL);
    ASSERT(node != NULL);

    /* Look through the list */
    ForeachNodeV0 (list, next)
    {
        /*
            Look for the first node with a lower pri as the node
            we have to insert into the list.
        */
        if (node->ln_Pri > next->ln_Pri)
            break;
    }

    /* Insert the node before(!) next. The situation looks like this:

            A<->next<->B *<-node->*

        ie. next->ln_Pred points to A, A->ln_Succ points to next,
        next->ln_Succ points to B, B->ln_Pred points to next.
        ln_Succ and ln_Pred of node contain illegal pointers.
    */

    /* Let node point to A: A<-node */
    node->ln_Pred          = next->ln_Pred;

    /* Make node point to next: A<->node->next<->B */
    node->ln_Succ          = (APTR32)(IPTR)next;

    /* Let A point to node: A->node */
    ((struct NodeV0 *)(IPTR)next->ln_Pred)->ln_Succ = (APTR32)(IPTR)node;

    /* Make next point to node: A<->node<-next<->B */
    next->ln_Pred          = (APTR32)(IPTR)node;
} /* Enqueue */

static struct NodeV0 * int_FindName(struct ListV0 *list, CONST_STRPTR name, struct ExecBaseV0 *SysBaseV0)
{
    struct NodeV0 * node;
/* FIX !
        FindName supplied with a NULL list defaults to the exec port list
        Changed in lists.c as well....
*/
    if( !list )
    {
#if defined(__AROSEXEC_SMP__)
        bug("[EXEC] %s: called with NULL list!\n", __func__);
#endif
        list = &SysBaseV0->PortList;
    }

/*    ASSERT(list != NULL); */
    ASSERT(name);

    /* Look through the list */
    for (node=GetHeadV0(list); node; node=GetSuccV0(node))
    {
        /* Only compare the names if this node has one. */
        if(node->ln_Name)
        {
            /* Check the node. If we found it, stop. */
            if (!strcmp ((APTR)(IPTR)node->ln_Name, name))
                break;
        }
    }

    /*
        If we found a node, this will contain the pointer to it. If we
        didn't, this will be NULL (either because the list was
        empty or because we tried all nodes in the list)
    */
    return node;
} /* FindName */

APTR abiv0_OpenResource(CONST_STRPTR resName)
{
    if (strcmp(resName, "kernel.resource") == 0)
        return NULL;

asm("int3");
    return (APTR)0x1;
}
MAKE_PROXY_ARG_2(OpenResource)

void  abiv0_AddResource(APTR resource, struct ExecBaseV0 *SysBaseV0)
{
    ASSERT_VALID_PTR(resource);

    /* Just in case the user forgot them */
    ((struct NodeV0 *)resource)->ln_Type=NT_RESOURCE;

    /* Arbitrate for the resource list */
    // EXEC_LOCK_LIST_WRITE_AND_FORBID(&SysBase->ResourceList);

    /* And add the resource */
    int_Enqueue(&SysBaseV0->ResourceList,(struct NodeV0 *)resource, SysBaseV0);

    /* All done. */
    // EXEC_UNLOCK_LIST_AND_PERMIT(&SysBase->ResourceList);

    /*
     * A tricky part.
     * kernel.resource is the first one to get initialized. After that
     * some more things can wake up (hostlib.resource for example).
     * They might want to use AllocMem() and even AllocPooled().
     * Here we switch off boot mode of the memory manager. Currently we
     * only set up page size for pool manager. When memory protection
     * is implemented, more things will be done here.
     * This allows to use exec pools even in kernel.resource itself.
     * To do this its startup code needs to be changed to call AddResource()
     * itself. Right after this exec's pool manager will be up and running.
     */
    // if (!strcmp(((struct Node *)resource)->ln_Name, "kernel.resource"))
    // {
    //     KernelBase = resource;
    //     DINIT("Post-kernel init");

    //     /* If there's no MMU support, PageSize will stay zero */
    //     KrnStatMemory(0, KMS_PageSize, &PrivExecBase(SysBase)->PageSize, TAG_DONE);

    //     /*
    //      * On MMU-less hardware kernel.resource will report zero page size.
    //      * In this case we use MEMCHUNK_TOTAL as allocation granularity.
    //      * This is because our Allocate() relies on the fact that all chunks
    //      * are at least MemChunk-aligned, otherwise we end up in
    //      * "Corrupt memory list" alert.
    //      */
    //     if (!PrivExecBase(SysBase)->PageSize)
    //         PrivExecBase(SysBase)->PageSize = MEMCHUNK_TOTAL;

    //     DINIT("Memory page size: %lu", PrivExecBase(SysBase)->PageSize);

    //     /* We print the notice here because kprintf() works only after KernelBase is set up */
    //     if (PrivExecBase(SysBase)->IntFlags & EXECF_MungWall)
    //         RawDoFmt("[exec] Mungwall enabled\n", NULL, (VOID_FUNC)RAWFMTFUNC_SERIAL, NULL);
    // }

} /* AddResource */
MAKE_PROXY_ARG_2(AddResource)

static void int_AddLibrary(struct LibraryV0 *library, struct ExecBaseV0 *SysBaseV0)
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
    int_Enqueue(&SysBaseV0->LibList,&library->lib_Node,SysBaseV0);
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

AROS_LD3(void, CacheClearE,
         AROS_LHA(APTR, address, A0),
         AROS_LHA(ULONG, length, D0),
         AROS_LHA(ULONG, caches, D1),
         struct ExecBase *, SysBase, 107, Exec);

void _aros_not_implemented(char *lvo)
{
    asm("int3");
}

static ULONG  int_MakeFunctions(APTR target, APTR32 functionArray, APTR32 funcDispBase, struct ExecBaseV0 *SysBaseV0)
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
            int_MakeFunctions(library,funcInit,(APTR32)(IPTR)NULL,SysBaseV0);

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
MAKE_PROXY_ARG_6(MakeLibrary)

APTR abiv0_InitResident(struct ResidentV0 *resident, BPTR segList, struct ExecBaseV0 *SysBaseV0)
{
    struct LibraryV0 *library = NULL;

    D(bug("InitResident begin 0x%p (\"%s\")", resident, resident->rt_Name));

    /* Check for validity */
    if(resident->rt_MatchWord != RTC_MATCHWORD ||
       resident->rt_MatchTag != (APTR32)(IPTR)resident)
        return NULL;

    /* Depending on the autoinit flag... */
    if(resident->rt_Flags & RTF_AUTOINIT)
    {
        /* ...initialize automatically... */
        struct init
        {
            ULONG dSize;
            APTR32 vectors;
            APTR32 structure;
            APTR32 init;
        };
        struct init *init = (struct init *)(IPTR)resident->rt_Init;

        D(bug("InitResident RTF_AUTOINIT"));

        /*
            Make the library. Don't call the Init routine yet, but delay
            that until we can copy stuff from the tag to the libbase.
        */
        library = abiv0_MakeLibrary(init->vectors, init->structure,
                              (APTR32)(IPTR)NULL, init->dSize, segList, SysBaseV0);

        if(library != NULL)
        {
            /*
                Copy over the interesting stuff from the ROMtag, and set the
                library state to indicate that this lib has changed and
                should be checksummed at the next opportunity.

                Don't copy the priority, because a tag's priority doesn't
                mean the same as a lib's priority.
            */
            library->lib_Node.ln_Type = resident->rt_Type;
            library->lib_Node.ln_Name = resident->rt_Name;
/*          Even if this is a resource, it was created using MakeLibrary(), this assumes
            that it has struct Library in the beginning - sonic
            if (resident->rt_Type != NT_RESOURCE)
            {*/
                library->lib_Version      = resident->rt_Version;
                library->lib_IdString     = resident->rt_IdString;
                library->lib_Flags        = LIBF_SUMUSED|LIBF_CHANGED;

                if (resident->rt_Flags & RTF_EXTENDED)
                {
                    library->lib_Revision = resident->rt_Revision;
                }
/*          }*/

            /*
                Call the library init vector, if set.
            */
            if(init->init)
            {
                __asm__ volatile (
                    "subq $12, %%rsp\n"
                    "movl %2, %%eax\n"
                    "movl %%eax, 8(%%rsp)\n"
                    "movl %3, %%eax\n"
                    "movl %%eax, 4(%%rsp)\n"
                    "movl %0, %%eax\n"
                    "movl %%eax, (%%rsp)\n"
                    "movl %1, %%eax\n"
                    ENTER32
                    "call *%%eax\n"
                    ENTER64
                    "addq $12, %%rsp\n"
                    "movl %%eax, %0\n"
                :"+m" (library):"m"(init->init), "m"(SysBaseV0), "m"(segList)
                : SCRATCH_REGS_64_TO_32 );
            }

            /*
                Test the library base, in case the init routine failed in
                some way.
            */
            if(library != NULL)
            {
                /*
                    Add the initialized module to the system.
                */
                switch(resident->rt_Type)
                {
                    case NT_DEVICE:
                    asm("int3");
                        // AddDevice((struct Device *)library);
                        break;
                    case NT_LIBRARY:
                    case NT_HIDD:   /* XXX Remove when new Hidd system ok. */
                        int_AddLibrary(library, SysBaseV0);
                        break;
                    case NT_RESOURCE:
                    asm("int3");
                        // AddResource(library);
                        break;
                }
            }
        }
    }
    else
    {
        D(bug("InitResident !RTF_AUTOINIT"));
        /* ...or let the library do it. */
        if (resident->rt_Init) {
            __asm__ volatile (
                "subq $12, %%rsp\n"
                "movl %2, %%eax\n"
                "movl %%eax, 8(%%rsp)\n"
                "movl %3, %%eax\n"
                "movl %%eax, 4(%%rsp)\n"
                "movl $0, %%eax\n"
                "movl %%eax, (%%rsp)\n"
                "movl %1, %%eax\n"
                ENTER32
                "call *%%eax\n"
                ENTER64
                "addq $12, %%rsp\n"
                "movl %%eax, %0\n"
            :"=m" (library):"m"(resident->rt_Init), "m"(SysBaseV0), "m"(segList)
            : SCRATCH_REGS_64_TO_32 );
        }
    }

    D(bug("InitResident end 0x%p (\"%s\"), result 0x%p", resident, resident->rt_Name, library));

    return library;
} /* InitResident */

static struct LibraryV0 * exec_OpenLibrary(CONST_STRPTR libName, ULONG version, struct ExecBaseV0 *SysBaseV0)
{
    struct LibraryV0 * library;

    D(bug("OpenLibrary(\"%s\", %ld)", libName, version));

    /* Arbitrate for the library list */
    // EXEC_LOCK_LIST_READ_AND_FORBID(&SysBase->LibList);

    /* Look for the library in our list */
    library = (struct LibraryV0 *) int_FindName (&SysBaseV0->LibList, libName, SysBaseV0);

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
            :"+m" (library):"m"(__AROS_GETVECADDRV0(library, 1)), "m"(version)
            : SCRATCH_REGS_64_TO_32 );
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

APTR abiv0_DOS_OpenLibrary(CONST_STRPTR name, ULONG version, struct ExecBaseV0 *SysBaseV0)
{
    D(bug("abiv0_OpenLibrary: %s\n", name));
    TEXT buff[64];
    struct LibraryV0 *_ret;
    STRPTR stripped_name = FilePart(name);

    /* Special case */
    if (strcmp(stripped_name, "dos.library") == 0)
        return abiv0DOSBase;

    /* Workaround for compiled-in absolute paths */
    if (strcmp(name, "SYS:Classes/datatypes/picture.datatype") == 0)
        name = "datatypes/picture.datatype";
    if (strcmp(name, "SYS:Classes/datatypes/png.datatype") == 0)
        name = "datatypes/png.datatype";

    /* Call Exec function, maybe the library is already available */
    _ret = exec_OpenLibrary(stripped_name, version, SysBaseV0);
    if (_ret)
        return _ret;

    /* Try loading from disk */
    NewRawDoFmt("LIBSV0:%s", RAWFMTFUNC_STRING, buff, name);

    BPTR seglist = LoadSeg32(buff, DOSBase);

    if (seglist == BNULL)
        return NULL;

    struct ResidentV0 *res = findResident(seglist, NULL);

    if (res)
    {
        D(bug("[LDInit] Calling InitResident(%p) on %s\n", res, res->rt_Name));
        /* AOS compatibility requirement.
            * Ramlib ignores InitResident() return code.
            * After InitResident() it checks if lib/dev appeared
            * in Exec lib/dev list via FindName().
            *
            * Evidently InitResident()'s return code was not
            * reliable for some early AOS libraries.
            */
        // Forbid();
        abiv0_InitResident(res, seglist, SysBaseV0);
        _ret = exec_OpenLibrary(stripped_name, version, SysBaseV0);
        // Permit();
        D(bug("[LDInit] Done calling InitResident(%p) on %s, seg %p, node %p\n", res, res->rt_Name, BADDR(seglist), _ret));

        return _ret;
    }

    return NULL;
}

void proxy_OpenLibrary();
void dummy_OpenLibrary()
{
    EXTER_PROXY(OpenLibrary)
    STORE_ESI_EDI
    ENTER64
    ALIGN_STACK64
    SET_ARG64__3_FROM32
    SET_ARG64__2_FROM32
    SET_ARG64__1_FROM32
    CALL_IMPL64(DOS_OpenLibrary)
    RESTORE_STACK64
    ENTER32
    RESTORE_ESI_EDI
    LEAVE_PROXY
}

void abiv0_CloseLibrary(struct LibraryV0 *library, struct ExecBaseV0 *SysBaseV0)
{
    BPTR seglist = BNULL;

    if (library != NULL)
    {
        /* Skip dos.library */
        if (library->lib_Node.ln_Name == 0x0000D0FF) return;

        // Forbid();
        CALL32_ARG_1(seglist, __AROS_GETVECADDRV0(library, 2), library);
        if (seglist)
        {
            /* Safe to call from a Task */
            UnLoadSeg(seglist);
        }
        // Permit();
    }
}
MAKE_PROXY_ARG_2(CloseLibrary)

static BOOL int_expunged = FALSE;
static void int_RemLibrary(struct LibraryV0 *library, struct ExecBaseV0 *SysBaseV0)
{
    BPTR seglist = BNULL;

    // Forbid();
    /* calling ExpungeLib: library ends up in D0 and A6 for compatibility */
    CALL32_ARG_2(seglist, __AROS_GETVECADDRV0(library, 3), library, library);
    if (seglist)
    {
        int_expunged = TRUE;
        UnLoadSeg(seglist);
    }
    // Permit();
}

static LONG int_exec_expunge_libraries(struct ExecBaseV0 *SysBaseV0)
{
    struct LibraryV0 *library;
    LONG expunged = 0;

    /* Forbid() is already done, but I don't want to rely on it. */
//     Forbid();

    /* Follow the linked list of shared libraries. */
    library = (struct LibraryV0 *)(IPTR)SysBaseV0->LibList.lh_Head;
    while (library->lib_Node.ln_Succ != (APTR32)(IPTR)NULL)
    {
        /* Flush libraries with a 0 open count */
        if (library->lib_OpenCnt == 0)
        {
            /* the library list node will be wiped from memory */
            struct LibraryV0 *nextLib = (struct LibraryV0 *)(IPTR)library->lib_Node.ln_Succ;
            int_expunged = FALSE;
            int_RemLibrary(library, SysBaseV0);
            if (int_expunged) expunged++;

            library = nextLib;
        }
        else
        {
            /* Go on to next library. */
            library = (struct LibraryV0 *)(IPTR)library->lib_Node.ln_Succ;
        }
    }

    return expunged;
}

void exec_expunge_libraries(struct ExecBaseV0 *SysBaseV0)
{
    while (int_exec_expunge_libraries(SysBaseV0) > 0);
}

void exec_force_expunge(struct ExecBaseV0 *SysBaseV0, STRPTR libname)
{
    struct LibraryV0 *library;

    /* Follow the linked list of shared libraries. */
    library = (struct LibraryV0 *)(IPTR)SysBaseV0->LibList.lh_Head;
    while (library->lib_Node.ln_Succ != (APTR32)(IPTR)NULL)
    {
        if (strcmp((char *)(IPTR)library->lib_Node.ln_Name, libname) == 0)
        {
            library->lib_OpenCnt = 0;
            exec_expunge_libraries(SysBaseV0);
            return;
        }

        /* Go on to next library. */
        library = (struct LibraryV0 *)(IPTR)library->lib_Node.ln_Succ;
    }
}

void Exec_Libraries_init(struct ExecBaseV0 *abiv0SysBase)
{
    __AROS_SETVECADDRV0(abiv0SysBase, 92, (APTR32)(IPTR)proxy_OpenLibrary);
    __AROS_SETVECADDRV0(abiv0SysBase, 69, (APTR32)(IPTR)proxy_CloseLibrary);
    __AROS_SETVECADDRV0(abiv0SysBase, 14, (APTR32)(IPTR)proxy_MakeLibrary);
    __AROS_SETVECADDRV0(abiv0SysBase,135, execfunctable[134]);   // TaggedOpenLibrary
    __AROS_SETVECADDRV0(abiv0SysBase, 70, execfunctable[69]);    // SetFunction
    __AROS_SETVECADDRV0(abiv0SysBase, 71, execfunctable[70]);    // SumLibrary
    __AROS_SETVECADDRV0(abiv0SysBase, 83, (APTR32)(IPTR)proxy_OpenResource);
    __AROS_SETVECADDRV0(abiv0SysBase, 81, (APTR32)(IPTR)proxy_AddResource);
}
