#include <dos/bptr.h>
#include <exec/types.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <aros/debug.h>
#include <proto/timer.h>

#include <stdio.h>
#include <string.h>

#include "abiv0/include/exec/functions.h"
#include "abiv0/include/aros/proxy.h"
#include "abiv0/include/aros/cpu.h"
#include "abiv0/include/dos/structures.h"
#include "abiv0/include/timer/structures.h"

BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);

struct DosLibraryV0 *abiv0DOSBase;

struct LibraryV0 *shallow_InitResident32(struct ResidentV0 *resident, BPTR segList, struct ExecBaseV0 *SysBaseV0)
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
        init->init = 0;

        library = abiv0_InitResident(resident, segList, SysBaseV0);
    }
    else
    {
        D(bug("InitResident !RTF_AUTOINIT"));
asm("int3");
    }

    D(bug("InitResident end 0x%p (\"%s\"), result 0x%p", resident, resident->rt_Name, library));

    return library;
} /* shallow_InitResident32 */


APTR abiv0_AllocMem(ULONG byteSize, ULONG requirements, struct ExecBaseV0 *SysBaseV0)
{
    return AllocMem(byteSize, requirements | MEMF_31BIT);
}
MAKE_PROXY_ARG_3(AllocMem)

void abiv0_FreeMem(APTR memoryBlock, ULONG byteSize, struct ExecBaseV0 *SysBaseV0)
{
    return FreeMem(memoryBlock, byteSize);
}
MAKE_PROXY_ARG_3(FreeMem)

APTR abiv0_AllocVec(ULONG byteSize, ULONG requirements, struct ExecBaseV0 *SysBaseV0)
{
    return AllocVec(byteSize, requirements | MEMF_31BIT);
}
MAKE_PROXY_ARG_3(AllocVec)

APTR abiv0_CreatePool(ULONG requirements, ULONG puddleSize, ULONG threshSize, struct ExecBaseV0 *SysBaseV0)
{
    return CreatePool(requirements | MEMF_31BIT, puddleSize, threshSize);
}
MAKE_PROXY_ARG_4(CreatePool)

APTR abiv0_AllocPooled(APTR poolHeader, ULONG memSize, struct ExecBaseV0 *SysBaseV0)
{
    return AllocPooled(poolHeader, memSize);
}
MAKE_PROXY_ARG_3(AllocPooled)

ULONG abiv0_TypeOfMem(APTR address, struct ExecBaseV0 *SysBaseV0)
{
    return TypeOfMem(address);
}
MAKE_PROXY_ARG_2(TypeOfMem)

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

APTR abiv0_DOS_OpenLibrary(CONST_STRPTR name, ULONG version, struct ExecBaseV0 *SysBaseV0)
{
    bug("abiv0_OpenLibrary: %s\n", name);
    TEXT buff[64];
    struct LibraryV0 *_ret;

    /* Special case */
    if (strcmp(name, "dos.library") == 0)
        return abiv0DOSBase;

    /* Call Exec function, maybe the library is already available */
    _ret = abiv0_OpenLibrary(name, version, SysBaseV0);
    if (_ret)
        return _ret;

    /* Try loading from disk */
    sprintf(buff, "SYS:Libs32/%s", name);

    BPTR seglist = LoadSeg32(buff, DOSBase);

    if (seglist == BNULL)
        return NULL;

    struct ResidentV0 *res = findResident(seglist, NULL);

    if (res)
    {
        (bug("[LDInit] Calling InitResident(%p) on %s\n", res, res->rt_Name));
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
        _ret = abiv0_OpenLibrary(name, version, SysBaseV0);
        // Permit();
        (bug("[LDInit] Done calling InitResident(%p) on %s, seg %p, node %p\n", res, res->rt_Name, BADDR(seglist), _ret));

        return _ret;
    }

    return NULL;
}

void proxy_OpenLibrary();
void dummy_OpenLibrary()
{
    EXTER_PROXY(OpenLibrary)
    ENTER64
    COPY_ARG_1
    COPY_ARG_2
    COPY_ARG_3
    CALL_IMPL64(DOS_OpenLibrary)
    ENTER32
    LEAVE_PROXY
}

void abiv0_PutStr(CONST_STRPTR text)
{
    PutStr(text);
}
MAKE_PROXY_ARG_2(PutStr)

void abiv0_CloseLibrary()
{
}
MAKE_PROXY_ARG_2(CloseLibrary)

ULONG abiv0_AllocTaskStorageSlot()
{
    return AllocTaskStorageSlot();
}
MAKE_PROXY_ARG_2(AllocTaskStorageSlot)

BOOL abiv0_SetTaskStorageSlot(LONG slot, ULONG value)
{
    return SetTaskStorageSlot(slot, value);
}
MAKE_PROXY_ARG_3(SetTaskStorageSlot)

ULONG abiv0_GetTaskStorageSlot(LONG id)
{
    return GetTaskStorageSlot(id);
}
MAKE_PROXY_ARG_2(GetTaskStorageSlot)

APTR abiv0_OpenResource(CONST_STRPTR resName)
{
    if (strcmp(resName, "kernel.resource") == 0)
        return NULL;

asm("int3");
    return (APTR)0x1;
}
MAKE_PROXY_ARG_2(OpenResource)

LONG abiv0_OpenDevice(CONST_STRPTR devName, ULONG unitNumber, struct IORequestV0 *iORequest)
{
    if (strcmp(devName, "timer.device") == 0)
    {
        iORequest->io_Device = (APTR32)0;
        return 0;
    }

asm("int3");
    return 0;
}
MAKE_PROXY_ARG_4(OpenDevice)

MAKE_PROXY_ARG_6(MakeLibrary)

void abiv0_CopyMem(APTR source, APTR dest, ULONG size)
{
    return CopyMem(source, dest, size);
}
MAKE_PROXY_ARG_4(CopyMem)

struct LibraryV0 *abiv0_Intuition_OpenLib(ULONG version, struct LibraryV0* IntuitionBaseV0)
{
    return IntuitionBaseV0;
}
MAKE_PROXY_ARG_2(Intuition_OpenLib)

struct LibraryV0 *abiv0_Gfx_OpenLib(ULONG version, struct LibraryV0* GfxBaseV0)
{
    return GfxBaseV0;
}
MAKE_PROXY_ARG_2(Gfx_OpenLib)

struct TaskV0 *abiv0_FindTask(CONST_STRPTR name, struct ExecBaseV0 *SysBaseV0)
{
    static struct ProcessV0 *dummy = NULL;
    if (dummy == NULL) dummy = abiv0_AllocMem(sizeof(struct ProcessV0), MEMF_CLEAR, SysBaseV0);
    dummy->pr_Task.tc_Node.ln_Type = NT_PROCESS;
    dummy->pr_Task.tc_Node.ln_Name = (APTR32)(IPTR)abiv0_AllocMem(10, MEMF_CLEAR, SysBaseV0);
    strcpy((char *)(IPTR)dummy->pr_Task.tc_Node.ln_Name, "emulator");
    dummy->pr_CLI = 0x1; //fake
    dummy->pr_CIS = 0x1; //fake
    dummy->pr_CES = 0x1; //fake
    dummy->pr_COS = 0x1; //fake

    return (struct TaskV0 *)dummy;
}
MAKE_PROXY_ARG_2(FindTask)

LONG abiv0_SetVBuf()
{
    return 0;
}
MAKE_PROXY_ARG_5(SetVBuf)

void abiv0_GetSysTime(struct timeval *dest, struct LibraryV0 *TimerBaseV0)
{
    return GetSysTime(dest);
}
MAKE_PROXY_ARG_2(GetSysTime)

LONG abiv0_FPutC(BPTR file, LONG character, struct DosLibraryV0 DOSBaseV0)
{
    if ((IPTR)file == 0x1)
    {
        return FPutC(Output(), character);
    }

asm("int3");
}
MAKE_PROXY_ARG_3(FPutC)

ULONG *execfunctable;
ULONG *dosfunctable;
ULONG *dosinitlist;

LONG_FUNC run_emulation()
{
    BPTR seg = LoadSeg32("SYS:Calculator", DOSBase);
    // BPTR seg = LoadSeg32("SYS:helloabi", DOSBase);
    APTR (*start)() = (APTR)((IPTR)BADDR(seg) + sizeof(BPTR));

    /* Set start at first instruction (skip Seg header) */
    start = (APTR)((IPTR)start + 13);

    APTR tmp;

    /* Keep it! This fills global variable */
    LoadSeg32("SYS:Libs32/exec/kernel", DOSBase);

    tmp = AllocMem(2048, MEMF_31BIT | MEMF_CLEAR);
    struct ExecBaseV0 *sysbase = (tmp + 1024);
    NEWLISTV0(&sysbase->LibList);
    sysbase->LibNode.lib_Version = 51;

    __AROS_SETVECADDRV0(sysbase, 92, (APTR32)(IPTR)proxy_OpenLibrary);
    __AROS_SETVECADDRV0(sysbase, 69, (APTR32)(IPTR)proxy_CloseLibrary);
    __AROS_SETVECADDRV0(sysbase, 49, (APTR32)(IPTR)proxy_FindTask);
    __AROS_SETVECADDRV0(sysbase,180, (APTR32)(IPTR)proxy_AllocTaskStorageSlot);
    __AROS_SETVECADDRV0(sysbase,184, (APTR32)(IPTR)proxy_SetTaskStorageSlot);
    __AROS_SETVECADDRV0(sysbase,185, (APTR32)(IPTR)proxy_GetTaskStorageSlot);
    __AROS_SETVECADDRV0(sysbase, 83, (APTR32)(IPTR)proxy_OpenResource);
    __AROS_SETVECADDRV0(sysbase, 93, execfunctable[92]);    // InitSemaphore
    __AROS_SETVECADDRV0(sysbase, 33, (APTR32)(IPTR)proxy_AllocMem);
    __AROS_SETVECADDRV0(sysbase, 14, (APTR32)(IPTR)proxy_MakeLibrary);
    __AROS_SETVECADDRV0(sysbase,104, (APTR32)(IPTR)proxy_CopyMem);
    __AROS_SETVECADDRV0(sysbase,116, (APTR32)(IPTR)proxy_CreatePool);
    __AROS_SETVECADDRV0(sysbase, 74, (APTR32)(IPTR)proxy_OpenDevice);
    __AROS_SETVECADDRV0(sysbase,118, (APTR32)(IPTR)proxy_AllocPooled);
    __AROS_SETVECADDRV0(sysbase,114, (APTR32)(IPTR)proxy_AllocVec);
    __AROS_SETVECADDRV0(sysbase, 46, execfunctable[45]);    // FindName
    __AROS_SETVECADDRV0(sysbase,135, execfunctable[134]);   // TaggedOpenLibrary
    __AROS_SETVECADDRV0(sysbase, 89, (APTR32)(IPTR)proxy_TypeOfMem);
    __AROS_SETVECADDRV0(sysbase, 41, execfunctable[40]);   // AddTail
    __AROS_SETVECADDRV0(sysbase, 87, execfunctable[86]);   // RawDoFmt
    __AROS_SETVECADDRV0(sysbase, 35, (APTR32)(IPTR)proxy_FreeMem);

    tmp = AllocMem(1024, MEMF_31BIT | MEMF_CLEAR);
    struct LibraryV0 *abiv0TimerBase = (tmp + 512);
    __AROS_SETVECADDRV0(abiv0TimerBase, 11, (APTR32)(IPTR)proxy_GetSysTime);


    /* Keep it! This fills global variable */
    LoadSeg32("SYS:Libs32/dos.library", DOSBase);

    tmp = AllocMem(2048, MEMF_31BIT | MEMF_CLEAR);
    abiv0DOSBase = (tmp + 1024);
    abiv0DOSBase->dl_lib.lib_Version = DOSBase->dl_lib.lib_Version;
    abiv0DOSBase->dl_TimeReq = (APTR32)(IPTR)AllocMem(sizeof(struct timerequestV0), MEMF_31BIT | MEMF_CLEAR);
    ((struct timerequestV0 *)(IPTR)abiv0DOSBase->dl_TimeReq)->tr_node.io_Device = (APTR32)(IPTR)abiv0TimerBase;

    /* Call SysBase_autoinit */
    __asm__ volatile (
        "subq $4, %%rsp\n"
        "movl %0, %%eax\n"
        "movl %%eax, (%%rsp)\n"
        "movl %1, %%eax\n"
        ENTER32
        "call *%%eax\n"
        ENTER64
        "addq $4, %%rsp\n"
        ::"m"(sysbase), "m"(dosinitlist[1]) : "%rax", "%rcx");

    __AROS_SETVECADDRV0(abiv0DOSBase, 158, (APTR32)(IPTR)proxy_PutStr);
    __AROS_SETVECADDRV0(abiv0DOSBase,   9, dosfunctable[8]);    // Input
    __AROS_SETVECADDRV0(abiv0DOSBase,  10, dosfunctable[9]);    // Output
    __AROS_SETVECADDRV0(abiv0DOSBase,  61, (APTR32)(IPTR)proxy_SetVBuf);
    __AROS_SETVECADDRV0(abiv0DOSBase,  32, dosfunctable[31]);   // DateStamp
    __AROS_SETVECADDRV0(abiv0DOSBase,  82, dosfunctable[81]);   // Cli
    __AROS_SETVECADDRV0(abiv0DOSBase, 159, dosfunctable[158]);  // VPrintf
    __AROS_SETVECADDRV0(abiv0DOSBase,  52, (APTR32)(IPTR)proxy_FPutC);


    BPTR cgfxseg = LoadSeg32("SYS:Libs32/cybergraphics.library", DOSBase);
    struct ResidentV0 *cgfxres = findResident(cgfxseg, NULL);
    struct LibraryV0 *abiv0CyberGfxBase = shallow_InitResident32(cgfxres, cgfxseg, sysbase);
    /* Remove all vectors for now (leave LibOpen) */
    for (int i = 5; i <= 38; i++) __AROS_SETVECADDRV0(abiv0CyberGfxBase, i, 0);

    BPTR intuitionseg = LoadSeg32("SYS:Libs32/intuition.library", DOSBase);
    struct ResidentV0 *intuitionres = findResident(intuitionseg, NULL);
    struct LibraryV0 *abiv0IntuitionBase = shallow_InitResident32(intuitionres, intuitionseg, sysbase);
    /* Remove all vectors for now */
    for (int i = 1; i <= 164; i++) __AROS_SETVECADDRV0(abiv0IntuitionBase, i, 0);

    __AROS_SETVECADDRV0(abiv0IntuitionBase,   1, (APTR32)(IPTR)proxy_Intuition_OpenLib);

    BPTR graphicsseg = LoadSeg32("SYS:Libs32/graphics.library", DOSBase);
    struct ResidentV0 *graphicsres = findResident(graphicsseg, NULL);
    struct LibraryV0 *abiv0GfxBase = shallow_InitResident32(graphicsres, graphicsseg, sysbase);
    /* Remove all vectors for now */
    for (int i = 1; i <= 201; i++) __AROS_SETVECADDRV0(abiv0GfxBase, i, 0);
    __AROS_SETVECADDRV0(abiv0GfxBase,   1, (APTR32)(IPTR)proxy_Gfx_OpenLib);


    /*  Switch to CS = 0x23 during FAR call. This switches 32-bit emulation mode.
        Next, load 0x2B to DS (needed under 32-bit) and NEAR jump to 32-bit code */
    __asm__ volatile(
    "   movl %0, %%ecx\n"
    "   movl %1, %%edx\n"
    "   subq $8, %%rsp\n"
    "   movl $0x23, 4(%%rsp)\n" // Jump to 32-bit mode
    "   lea  tramp, %%rax\n"
    "   movl %%eax, (%%rsp)\n"
    "   lret\n"
    "tramp:\n"
    "   .code32\n"
    "   push $0x2b\n"
    "   pop %%ds\n"
    "   mov $0x0, %%eax\n"
    "   push %%edx\n" //SysBase
    "   push %%eax\n" //argsize
    "   push %%eax\n" //argstr
    "   call *%%ecx\n"
    "   pop %%eax\n" // Clean up stack
    "   pop %%eax\n"
    "   pop %%eax\n"
    "   push $0x33\n" // Jump back to 64-bit mode
    "   lea finished, %%eax\n"
    "   push %%eax\n"
    "   lret\n"
    "   .code64\n"
    "finished:"
        :: "m"(start), "m" (sysbase) :);
}

struct timerequest tr;
struct Device *TimerBase;

int main()
{
    OpenDevice("timer.device", UNIT_VBLANK, &tr.tr_node, 0);
    TimerBase = tr.tr_node.io_Device;

    /* Run emulation code with stack allocated in 31-bit memory */
    APTR stack31bit = AllocMem(64 * 1024, MEMF_CLEAR | MEMF_31BIT);
    struct StackSwapStruct sss;
    sss.stk_Lower = stack31bit;
    sss.stk_Upper = sss.stk_Lower + 64 * 1024;
    sss.stk_Pointer = sss.stk_Upper;

    NewStackSwap(&sss, run_emulation, NULL);

    return 0;
}

