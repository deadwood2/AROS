#include <dos/bptr.h>
#include <exec/types.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <aros/debug.h>

#include <stdio.h>
#include <string.h>

#include "abiv0/include/exec/functions.h"
#include "abiv0/include/aros/asm.h"
#include "abiv0/include/aros/cpu.h"
#include "abiv0/include/dos/structures.h"

BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);

struct DosLibraryV0 *abiv0DOSBase;



APTR abiv0_AllocMem(ULONG byteSize, ULONG requirements, struct ExecBaseV0 *SysBaseV0)
{
    return AllocMem(byteSize, requirements | MEMF_31BIT);
}

struct ResidentV0 * findResident(BPTR seg)
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

    if (strcmp(name, "stdlib.library") == 0)
        asm("int3");

    /* Call Exec function, maybe the library is already available */
    _ret = abiv0_OpenLibrary(name, version, SysBaseV0);
    if (_ret)
        return _ret;

    /* Try loading from disk */
    sprintf(buff, "SYS:Libs32/%s", name);

    BPTR seglist = LoadSeg32(buff, DOSBase);

    struct ResidentV0 *res = findResident(seglist);

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

void proxy_PutStr();
void dummy_PutStr()
{
    EXTER_PROXY(PutStr)
    ENTER64
    COPY_ARG_1
    CALL_IMPL64(PutStr)
    ENTER32
    LEAVE_PROXY
}

void abiv0_CloseLibrary()
{
}

void proxy_CloseLibrary();
void dummy_CloseLibrary()
{
    EXTER_PROXY(CloseLibrary)
    ENTER64
    COPY_ARG_1
    CALL_IMPL64(CloseLibrary)
    ENTER32
    LEAVE_PROXY
}

ULONG abiv0_AllocTaskStorageSlot()
{
    return AllocTaskStorageSlot();
}

void proxy_AllocTaskStorageSlot();
void dummy_AllocTaskStorageSlot()
{
    EXTER_PROXY(AllocTaskStorageSlot)
    ENTER64
    COPY_ARG_1
    CALL_IMPL64(AllocTaskStorageSlot)
    ENTER32
    LEAVE_PROXY
}

BOOL abiv0_SetTaskStorageSlot(LONG slot, ULONG value)
{
    return SetTaskStorageSlot(slot, value);
}

void proxy_SetTaskStorageSlot();
void dummy_SetTaskStorageSlot()
{
    EXTER_PROXY(SetTaskStorageSlot)
    ENTER64
    COPY_ARG_1
    COPY_ARG_2
    CALL_IMPL64(SetTaskStorageSlot)
    ENTER32
    LEAVE_PROXY
}

APTR abiv0_OpenResource(CONST_STRPTR name)
{
    if (strcmp(name, "kernel.resource") == 0)
        return NULL;

asm("int3");
    return (APTR)0x1;
}

void proxy_OpenResource();
void dummy_OpenResource()
{
    EXTER_PROXY(OpenResource)
    ENTER64
    COPY_ARG_1
    COPY_ARG_2
    CALL_IMPL64(OpenResource)
    ENTER32
    LEAVE_PROXY
}

struct TaskV0 *abiv0_FindTask(CONST_STRPTR name, struct ExecBaseV0 *SysBaseV0)
{
    static struct ProcessV0 *dummy = NULL;
    if (dummy == NULL) dummy = abiv0_AllocMem(sizeof(struct ProcessV0), MEMF_CLEAR, SysBaseV0);
    dummy->pr_CLI = 0x1; //fake
    return (struct TaskV0 *)dummy;
}

void proxy_FindTask();
void dummy_FindTask()
{
    EXTER_PROXY(FindTask)
    ENTER64
    COPY_ARG_1
    COPY_ARG_2
    CALL_IMPL64(FindTask)
    ENTER32
    LEAVE_PROXY
}

LONG_FUNC run_emulation()
{
    BPTR seg = LoadSeg32("SYS:Calculator", DOSBase);
    // BPTR seg = LoadSeg32("SYS:helloabi", DOSBase);
    APTR (*start)() = (APTR)((IPTR)BADDR(seg) + sizeof(BPTR));

    /* Set start at first instruction (skip Seg header) */
    start = (APTR)((IPTR)start + 13);

    APTR tmp;

    tmp = AllocMem(2048, MEMF_31BIT | MEMF_CLEAR);
    struct ExecBaseV0 *sysbase = (tmp + 1024);
    NEWLISTV0(&sysbase->LibList);
    sysbase->LibNode.lib_Version = 51;

    __AROS_SETVECADDRV0(sysbase, 92, (APTR32)(IPTR)proxy_OpenLibrary);
    __AROS_SETVECADDRV0(sysbase, 69, (APTR32)(IPTR)proxy_CloseLibrary);
    __AROS_SETVECADDRV0(sysbase, 49, (APTR32)(IPTR)proxy_FindTask);
    __AROS_SETVECADDRV0(sysbase,180, (APTR32)(IPTR)proxy_AllocTaskStorageSlot);
    __AROS_SETVECADDRV0(sysbase,184, (APTR32)(IPTR)proxy_SetTaskStorageSlot);
    __AROS_SETVECADDRV0(sysbase, 83, (APTR32)(IPTR)proxy_OpenResource);

    tmp = AllocMem(2048, MEMF_31BIT | MEMF_CLEAR);
    abiv0DOSBase = (tmp + 1024);
    abiv0DOSBase->dl_lib.lib_Version = DOSBase->dl_lib.lib_Version;

    __AROS_SETVECADDRV0(abiv0DOSBase, 158, (APTR32)(IPTR)proxy_PutStr);

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

int main()
{
    /* Run emulation code with stack allocated in 31-bit memory */
    APTR stack31bit = AllocMem(64 * 1024, MEMF_CLEAR | MEMF_31BIT);
    struct StackSwapStruct sss;
    sss.stk_Lower = stack31bit;
    sss.stk_Upper = sss.stk_Lower + 64 * 1024;
    sss.stk_Pointer = sss.stk_Upper;

    NewStackSwap(&sss, run_emulation, NULL);

    return 0;
}

