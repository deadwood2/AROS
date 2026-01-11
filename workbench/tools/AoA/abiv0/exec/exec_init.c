/*
    Copyright (C) 2024-2026, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <aros/debug.h>
#include <exec/rawfmt.h>

#include <string.h>

#include "../include/exec/structures.h"
#include "../include/exec/proxy_structures.h"
#include "../include/exec/functions.h"
#include "../include/aros/cpu.h"
#include "../include/aros/proxy.h"

#include "../include/dos/structures.h"

#include "exec_devices.h"
#include "exec_libraries.h"
#include "exec_ports.h"

BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);
struct ExecBaseV0 *abiv0SysBase;

APTR abiv0_AllocMem(ULONG byteSize, ULONG requirements, struct ExecBaseV0 *SysBaseV0)
{
    return AllocMem(byteSize, requirements | MEMF_31BIT);
}
MAKE_PROXY_ARG_3(AllocMem)

APTR abiv0_AllocAbs(ULONG byteSize, APTR location, struct ExecBaseV0 *SysBaseV0)
{
    return NULL;
}
MAKE_PROXY_ARG_3(AllocAbs)

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

void abiv0_FreeVec(APTR memoryBlock, struct ExecBaseV0 *SysBaseV0)
{
    FreeVec(memoryBlock);
}
MAKE_PROXY_ARG_2(FreeVec)

APTR abiv0_CreatePool(ULONG requirements, ULONG puddleSize, ULONG threshSize, struct ExecBaseV0 *SysBaseV0)
{
    return CreatePool(requirements | MEMF_31BIT, puddleSize, threshSize);
}
MAKE_PROXY_ARG_4(CreatePool)

void abiv0_DeletePool(APTR poolHeader, struct ExecBaseV0 *SysBaseV0)
{
    DeletePool(poolHeader);
}
MAKE_PROXY_ARG_2(DeletePool)

APTR abiv0_AllocPooled(APTR poolHeader, ULONG memSize, struct ExecBaseV0 *SysBaseV0)
{
    return AllocPooled(poolHeader, memSize);
}
MAKE_PROXY_ARG_3(AllocPooled)

void abiv0_FreePooled(APTR poolHeader, APTR memory, ULONG memSize, struct ExecBaseV0 *SysBaseV0)
{
    FreePooled(poolHeader, memory, memSize);
}
MAKE_PROXY_ARG_4(FreePooled)

APTR abiv0_AllocVecPooled(APTR poolHeader, ULONG memSize, struct ExecBaseV0 *SysBaseV0)
{
    return AllocVecPooled(poolHeader, memSize);
}
MAKE_PROXY_ARG_3(AllocVecPooled)

void abiv0_FreeVecPooled(APTR poolHeader, APTR memory, struct ExecBaseV0 *SysBaseV0)
{
    return FreeVecPooled(poolHeader, memory);
}
MAKE_PROXY_ARG_3(FreeVecPooled)

ULONG abiv0_TypeOfMem(APTR address, struct ExecBaseV0 *SysBaseV0)
{
    return TypeOfMem(address);
}
MAKE_PROXY_ARG_2(TypeOfMem)

VOID abiv0_Forbid(struct ExecBaseV0 *SysBaseV0)
{
    Forbid();
}
MAKE_PROXY_ARG_1(Forbid)

VOID abiv0_Permit(struct ExecBaseV0 *SysBaseV0)
{
    Permit();
}
MAKE_PROXY_ARG_1(Permit)

void nss_trampoline();
void dummy_nss_trampoline()
{
    __asm__ volatile(
    "nss_trampoline:\n"
    "   push %%rbp\n" /* Preserve full rbp in case we are called form code working on 64-bit stack */
    "   push %%rbx\n"
    "   movq %%rcx, %%rbx\n" /* ecx is used in ENTER32 */
    ENTER32
    "   push %%ebx\n" /* really ecx */
    "   push %%edx\n"
    "   push %%esi\n"
    "   call *%%edi\n"
    "   add $12, %%esp\n"
    ENTER64
    "   pop %%rbx\n"
    "   pop %%rbp\n"
    "   ret\n"
    :::);
}

IPTR abiv0_NewStackSwap(struct StackSwapStructV0 *sss, LONG_FUNC entry, struct StackSwapArgsV0 *args, struct ExecBaseV0 *SysBaseV0)
{
    struct StackSwapStruct sssnative;
    sssnative.stk_Pointer   = (APTR)(IPTR)sss->stk_Upper;
    sssnative.stk_Lower     = (APTR)(IPTR)sss->stk_Lower;
    sssnative.stk_Upper     = (APTR)(IPTR)sss->stk_Upper;

    struct StackSwapArgs argsnative;
    argsnative.Args[0]      = (IPTR)entry;
    argsnative.Args[1]      = args->Args[0];
    argsnative.Args[2]      = args->Args[1];
    argsnative.Args[3]      = args->Args[2];

bug("abiv0_NewStackSwap: STUB\n");

    return NewStackSwap(&sssnative, nss_trampoline, &argsnative);
}
MAKE_PROXY_ARG_4(NewStackSwap)

VOID abiv0_AddMemHandler(APTR memHandler, struct ExecBaseV0 *SysBaseV0)
{
    bug("abiv0_AddMemHandler ignored\n");
}
MAKE_PROXY_ARG_2(AddMemHandler)

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

ULONG abiv0_GetParentTaskStorageSlot(LONG id)
{
    IPTR val = GetParentTaskStorageSlot(id);
    return val;
}
MAKE_PROXY_ARG_2(GetParentTaskStorageSlot)

void abiv0_CacheClearE(APTR address, IPTR length, ULONG caches, struct ExecBaseV0 *SysBaseV0)
{
    CacheClearE(address, length, caches);
}
MAKE_PROXY_ARG_4(CacheClearE)

///

ULONG abiv0_Wait(ULONG signalSet, struct ExecBaseV0 *SysBaseV0)
{
    return Wait(signalSet);
}
MAKE_PROXY_ARG_2(Wait)

MAKE_PROXY_ARG_2(InitSemaphore)

void abiv0_CopyMem(APTR source, APTR dest, ULONG size)
{
    return CopyMem(source, dest, size);
}
MAKE_PROXY_ARG_4(CopyMem)

APTR makeFileHandleProxy(BPTR);
APTR makeFileHandleProxyDetailed(BPTR);

struct TaskV0 *g_v0maintask = NULL;
struct Task *g_nativemaintask = NULL;
extern STRPTR program_name;
extern STRPTR emu_argstr;

void refresh_g_v0maintask()
{
    if (g_v0maintask)
    {
        struct ProcessV0 *dummy = (struct ProcessV0 *)g_v0maintask;
        dummy->pr_HomeDir = (BPTR32)(IPTR)makeFileHandleProxy(GetProgramDir());
    }
}

struct TaskV0 * childprocess_getbynative(struct Task *childnative);

struct TaskV0 *abiv0_FindTask(CONST_STRPTR name, struct ExecBaseV0 *SysBaseV0)
{
    struct Task* native = FindTask(name);

    if (native == g_nativemaintask)
    {
        if (g_v0maintask == NULL)
        {
            struct ProcessV0 *dummy = NULL;
            struct CommandLineInterfaceV0 *cli = NULL;

            if (dummy == NULL) dummy = abiv0_AllocMem(sizeof(struct ProcessV0), MEMF_CLEAR, SysBaseV0);
            dummy->pr_Task.tc_Node.ln_Type = NT_PROCESS;
            dummy->pr_Task.tc_Node.ln_Name = (APTR32)(IPTR)abiv0_AllocMem(10, MEMF_CLEAR, SysBaseV0);
            strcpy((char *)(IPTR)dummy->pr_Task.tc_Node.ln_Name, "emulator");
            dummy->pr_CIS = (BPTR32)(IPTR)makeFileHandleProxyDetailed(Input()); // MEMLEAK
            dummy->pr_CES = (BPTR32)(IPTR)makeFileHandleProxy(((struct Process *)FindTask(NULL))->pr_CES); // MEMLEAK
            dummy->pr_COS = (BPTR32)(IPTR)makeFileHandleProxy(Output()); // MEMLEAK
            dummy->pr_HomeDir = (BPTR32)(IPTR)makeFileHandleProxy(GetProgramDir());
            dummy->pr_Arguments = (APTR32)(IPTR)emu_argstr;

            cli = abiv0_AllocMem(sizeof(struct CommandLineInterfaceV0), MEMF_CLEAR, SysBaseV0);
            cli->cli_CommandName = (APTR32)(IPTR)abiv0_AllocMem(strlen(program_name) + 1, MEMF_CLEAR, SysBaseV0);
            strcpy((char *)(IPTR)cli->cli_CommandName, program_name);

            dummy->pr_CLI = (BPTR32)(IPTR)cli;

            dummy->pr_Task.tc_Flags |= TF_ETASK;
            dummy->pr_Task.tc_UnionETask.tc_ETask = (APTR32)(IPTR)abiv0_AllocMem(sizeof(struct ETaskV0), MEMF_CLEAR, SysBaseV0);

            g_v0maintask = (struct TaskV0 *)dummy; // MEMLEAK

            /* Needed for GET_THIS_TASK; TODO: change for multithreaded applications */
            SysBaseV0->ThisTask = (APTR32)(IPTR)dummy;
        }
        return g_v0maintask;
    }

    /* Handle child processes */
    struct TaskV0 *child = childprocess_getbynative(native);
    if (child)
        return child;

bug("abiv0_FindTask:STUB\n");

    return NULL;
}
MAKE_PROXY_ARG_2(FindTask)

BYTE abiv0_AllocSignal(LONG signalNum, struct ExecBaseV0 *SysBaseV0)
{
    return AllocSignal(signalNum);
}
MAKE_PROXY_ARG_2(AllocSignal)

void abiv0_FreeSignal(LONG signalNum, struct ExecBaseV0 *SysBaseV0)
{
    FreeSignal(signalNum);
}
MAKE_PROXY_ARG_2(FreeSignal)

ULONG abiv0_SetSignal(ULONG newSignals, ULONG signalSet, struct ExecBaseV0 *SysBaseV0)
{
    return SetSignal(newSignals, signalSet);
}
MAKE_PROXY_ARG_3(SetSignal)

struct Task * childprocess_getbyv0(struct TaskV0 *childv0);

BYTE abiv0_SetTaskPri(struct TaskV0 *task, LONG priority, struct ExecBaseV0 *SysBaseV0)
{
    if (task == g_v0maintask)
        return SetTaskPri(g_nativemaintask, priority);

    struct Task *child = childprocess_getbyv0(task);
    if (child)
        return SetTaskPri(child, priority);

asm("int3");
    return 0;
}
MAKE_PROXY_ARG_3(SetTaskPri)

void  abiv0_Signal(struct TaskV0 *task, ULONG signalSet, struct ExecBaseV0 *SysBaseV0)
{
    if (task == g_v0maintask)
    {
        Signal(g_nativemaintask, signalSet);
        return;
    }

    struct Task *child = childprocess_getbyv0(task);
    if (child)
    {
        Signal(child, signalSet);
        return;
    }

asm("int3");
    return;
}
MAKE_PROXY_ARG_3(Signal)

void abiv0_RawPutChar(UBYTE chr, struct ExecBaseV0 *SysBaseV0)
{
    RawPutChar(chr);
}
MAKE_PROXY_ARG_2(RawPutChar)

IPTR abiv0_AvailMem(ULONG attributes, struct ExecBaseV0 *SysBaseV0)
{
    return AvailMem(attributes);
}
MAKE_PROXY_ARG_2(AvailMem)

extern ULONG *execfunctable;
APTR32 global_SysBaseV0Ptr;

// #define SEMA_DEBUG

#if defined(SEMA_DEBUG)
void abiv0_ObtainSemaphore(struct SignalSemaphoreV0 *sem, struct ExecBaseV0 *SysBaseV0)
{
    ULONG ret;

if (sem->ss_Link.ln_Type != NT_SIGNALSEM) asm("int3");
else
    __asm__ volatile (
        "subq $16, %%rsp\n"
        "movl %3, %%eax\n"
        "movl %%eax, 4(%%rsp)\n"
        "movl %2, %%eax\n"
        "movl %%eax, (%%rsp)\n"
        "movl %1, %%eax\n"
        ENTER32
        "call *%%eax\n"
        ENTER64
        "addq $16, %%rsp\n"
        "movl %%eax, %0\n"
        :"=m"(ret):"m"(execfunctable[93]), "m"(sem), "m"(SysBaseV0)
        : SCRATCH_REGS_64_TO_32 );
}
MAKE_PROXY_ARG_2(ObtainSemaphore)

void abiv0_ObtainSemaphoreShared(struct SignalSemaphoreV0 *sem, struct ExecBaseV0 *SysBaseV0)
{
    ULONG ret;

if (sem->ss_Link.ln_Type != NT_SIGNALSEM) asm("int3");
else
    __asm__ volatile (
        "subq $16, %%rsp\n"
        "movl %3, %%eax\n"
        "movl %%eax, 4(%%rsp)\n"
        "movl %2, %%eax\n"
        "movl %%eax, (%%rsp)\n"
        "movl %1, %%eax\n"
        ENTER32
        "call *%%eax\n"
        ENTER64
        "addq $16, %%rsp\n"
        "movl %%eax, %0\n"
        :"=m"(ret):"m"(execfunctable[112]), "m"(sem), "m"(SysBaseV0)
        : SCRATCH_REGS_64_TO_32 );
}
MAKE_PROXY_ARG_2(ObtainSemaphoreShared)
#endif


void abiv0_Kprintf(STRPTR format, APTR32 arg1, APTR32 arg2, APTR32 arg3, APTR32 arg4)
{
    bug(format, arg1, arg2, arg3, arg4);
}
MAKE_PROXY_ARG_12(Kprintf)

struct AROSSupportBaseV0
{
    ULONG   _pad;
    APTR32  kprintfptr;
    APTR32  rkprintfptr;
    APTR32  vkprintfptr;
};

struct IntExecBaseV0
{
    struct ExecBaseV0           pub;
    struct ListV0               ResetHandlers;                  /* Reset handlers list                                          */
    struct InterruptV0          ColdResetHandler;               /* Reset handler that causes cold reboot                        */
    struct InterruptV0          WarmResetHandler;               /* Reset handler that causes warm reboot                        */
    struct InterruptV0          ShutdownHandler;                /* Reset handler that halts CPU                                 */
    struct MinListV0            AllocMemList;                   /* Mungwall allocations list                                    */
    struct SignalSemaphoreV0    LowMemSem;                      /* Lock for single-threading low memory handlers                */
    APTR32                      KernelBase;                     /* kernel.resource base                                         */
};

struct ExecBaseV0 *init_exec()
{
    APTR tmp;
    TEXT path[64];

    tmp = AllocMem(1024 + sizeof(struct IntExecBaseV0), MEMF_31BIT | MEMF_CLEAR);
    abiv0SysBase = (tmp + 1024);
    global_SysBaseV0Ptr = (APTR32)(IPTR)&abiv0SysBase; /* Needed for LoadSeg32 to resolve SysBase in kernel */

    /* Keep it! This fills global variable */
    NewRawDoFmt("LIBSv0:partial/kernel", RAWFMTFUNC_STRING, path);
    LoadSeg32(path, DOSBase);

    NEWLISTV0(&abiv0SysBase->LibList);
    NEWLISTV0(&abiv0SysBase->ResourceList);
    abiv0SysBase->LibNode.lib_Node.ln_Name = 0x0000E0EC;
    abiv0SysBase->LibNode.lib_Version = 51;
    abiv0SysBase->DebugAROSBase = (APTR32)(IPTR)AllocMem(sizeof(struct AROSSupportBaseV0), MEMF_31BIT | MEMF_CLEAR);
    ((struct AROSSupportBaseV0 *)(IPTR)abiv0SysBase->DebugAROSBase)->kprintfptr =
        (APTR32)(IPTR)proxy_Kprintf;

    /* Needed to prevent certain original code from running */
    ((struct IntExecBaseV0 *)(IPTR)abiv0SysBase)->KernelBase = (APTR32)(IPTR)NULL;

    g_nativemaintask = FindTask(NULL);

    /* Set all LVO addresses to their number so that code jumps to "number" of the LVO and crashes */
    for (LONG i = 5; i <= 186; i++)
        __AROS_SETVECADDRV0(abiv0SysBase, i, (APTR32)(IPTR)i);

    /* Set all working LVOs */
    __AROS_SETVECADDRV0(abiv0SysBase, 49, (APTR32)(IPTR)proxy_FindTask);
    __AROS_SETVECADDRV0(abiv0SysBase,180, (APTR32)(IPTR)proxy_AllocTaskStorageSlot);
    __AROS_SETVECADDRV0(abiv0SysBase,184, (APTR32)(IPTR)proxy_SetTaskStorageSlot);
    __AROS_SETVECADDRV0(abiv0SysBase,185, (APTR32)(IPTR)proxy_GetTaskStorageSlot);
    __AROS_SETVECADDRV0(abiv0SysBase, 93, (APTR32)(IPTR)proxy_InitSemaphore);
    __AROS_SETVECADDRV0(abiv0SysBase, 33, (APTR32)(IPTR)proxy_AllocMem);
    __AROS_SETVECADDRV0(abiv0SysBase,104, (APTR32)(IPTR)proxy_CopyMem);
    __AROS_SETVECADDRV0(abiv0SysBase,116, (APTR32)(IPTR)proxy_CreatePool);
    __AROS_SETVECADDRV0(abiv0SysBase,118, (APTR32)(IPTR)proxy_AllocPooled);
    __AROS_SETVECADDRV0(abiv0SysBase,114, (APTR32)(IPTR)proxy_AllocVec);
    __AROS_SETVECADDRV0(abiv0SysBase, 46, execfunctable[45]);    // FindName
    __AROS_SETVECADDRV0(abiv0SysBase, 89, (APTR32)(IPTR)proxy_TypeOfMem);
    __AROS_SETVECADDRV0(abiv0SysBase, 41, execfunctable[40]);    // AddTail
    __AROS_SETVECADDRV0(abiv0SysBase, 87, execfunctable[86]);    // RawDoFmt
    __AROS_SETVECADDRV0(abiv0SysBase, 35, (APTR32)(IPTR)proxy_FreeMem);
#if defined(SEMA_DEBUG)
    __AROS_SETVECADDRV0(abiv0SysBase,113, (APTR32)(IPTR)proxy_ObtainSemaphoreShared);
    __AROS_SETVECADDRV0(abiv0SysBase, 94, (APTR32)(IPTR)proxy_ObtainSemaphore);
#else
    __AROS_SETVECADDRV0(abiv0SysBase,113, execfunctable[112]);   // ObtainSemaphoreShared
    __AROS_SETVECADDRV0(abiv0SysBase, 94, execfunctable[93]);    // ObtainSemaphore
#endif
    __AROS_SETVECADDRV0(abiv0SysBase, 40, execfunctable[39]);    // AddHead
    __AROS_SETVECADDRV0(abiv0SysBase, 95, execfunctable[94]);    // ReleaseSemaphore
    __AROS_SETVECADDRV0(abiv0SysBase, 22, (APTR32)(IPTR)proxy_Forbid);
    __AROS_SETVECADDRV0(abiv0SysBase, 23, (APTR32)(IPTR)proxy_Permit);
    __AROS_SETVECADDRV0(abiv0SysBase,129, (APTR32)(IPTR)proxy_AddMemHandler);
    __AROS_SETVECADDRV0(abiv0SysBase, 45, execfunctable[44]);    // Enqueue
    __AROS_SETVECADDRV0(abiv0SysBase, 34, (APTR32)(IPTR)proxy_AllocAbs);
    __AROS_SETVECADDRV0(abiv0SysBase,115, (APTR32)(IPTR)proxy_FreeVec);
    __AROS_SETVECADDRV0(abiv0SysBase,109, execfunctable[108]);  // CreateIORequest
    __AROS_SETVECADDRV0(abiv0SysBase, 42, execfunctable[41]);   // Remove
    __AROS_SETVECADDRV0(abiv0SysBase,149, (APTR32)(IPTR)proxy_AllocVecPooled);
    __AROS_SETVECADDRV0(abiv0SysBase,119, (APTR32)(IPTR)proxy_FreePooled);
    __AROS_SETVECADDRV0(abiv0SysBase,107, (APTR32)(IPTR)proxy_CacheClearE);
    __AROS_SETVECADDRV0(abiv0SysBase, 39, execfunctable[38]);   // Insert
    __AROS_SETVECADDRV0(abiv0SysBase, 44, execfunctable[43]);   // RemTail
    __AROS_SETVECADDRV0(abiv0SysBase, 43, execfunctable[42]);   // RemHead
    __AROS_SETVECADDRV0(abiv0SysBase, 53, (APTR32)(IPTR)proxy_Wait);
    __AROS_SETVECADDRV0(abiv0SysBase,117, (APTR32)(IPTR)proxy_DeletePool);
    __AROS_SETVECADDRV0(abiv0SysBase,110, execfunctable[109]);  // DeleteIORequest
    __AROS_SETVECADDRV0(abiv0SysBase,186, (APTR32)(IPTR)proxy_GetParentTaskStorageSlot);
    __AROS_SETVECADDRV0(abiv0SysBase,134, (APTR32)(IPTR)proxy_NewStackSwap);
    __AROS_SETVECADDRV0(abiv0SysBase,150, (APTR32)(IPTR)proxy_FreeVecPooled);
    __AROS_SETVECADDRV0(abiv0SysBase, 55, (APTR32)(IPTR)proxy_AllocSignal);
    __AROS_SETVECADDRV0(abiv0SysBase, 56, (APTR32)(IPTR)proxy_FreeSignal);
    __AROS_SETVECADDRV0(abiv0SysBase, 96, execfunctable[95]);    // AttemptSemaphore
    __AROS_SETVECADDRV0(abiv0SysBase, 51, (APTR32)(IPTR)proxy_SetSignal);
    __AROS_SETVECADDRV0(abiv0SysBase, 50, (APTR32)(IPTR)proxy_SetTaskPri);
    __AROS_SETVECADDRV0(abiv0SysBase, 54, (APTR32)(IPTR)proxy_Signal);
    __AROS_SETVECADDRV0(abiv0SysBase, 38, execfunctable[37]);    // FreeEntry
    __AROS_SETVECADDRV0(abiv0SysBase, 86, (APTR32)(IPTR)proxy_RawPutChar);
    __AROS_SETVECADDRV0(abiv0SysBase, 36, (APTR32)(IPTR)proxy_AvailMem);
    __AROS_SETVECADDRV0(abiv0SysBase,105, execfunctable[104]);   // CopyMemQuick
    Exec_Devices_init(abiv0SysBase);
    Exec_Libraries_init(abiv0SysBase);
    Exec_Ports_init(abiv0SysBase);

    return abiv0SysBase;
}
