#include <proto/exec.h>
#include <proto/dos.h>
#include <aros/debug.h>

#include <stdio.h>
#include <string.h>

#include "../include/exec/structures.h"
#include "../include/exec/proxy_structures.h"
#include "../include/exec/functions.h"
#include "../include/aros/cpu.h"
#include "../include/aros/proxy.h"

#include "../include/dos/structures.h"

extern struct DosLibraryV0 *abiv0DOSBase;
extern struct LibraryV0 *abiv0TimerBase;

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

VOID abiv0_AddMemHandler(APTR memHandler, struct ExecBaseV0 *SysBaseV0)
{
    bug("abiv0_AddMemHandler ignored\n");
}
MAKE_PROXY_ARG_2(AddMemHandler)

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

void abiv0_CacheClearE(APTR address, IPTR length, ULONG caches, struct ExecBaseV0 *SysBaseV0)
{
    CacheClearE(address, length, caches);
}
MAKE_PROXY_ARG_4(CacheClearE)

LONG abiv0_OpenDevice(CONST_STRPTR devName, ULONG unitNumber, struct IORequestV0 *iORequest)
{
    if (strcmp(devName, "timer.device") == 0)
    {
        iORequest->io_Device = (APTR32)(IPTR)abiv0TimerBase;
        return 0;
    }

    if (strcmp(devName, "input.device") == 0)
    {
bug("abiv0_OpenDevice: input.device STUB\n");
        iORequest->io_Device = 0;
        return 0;
    }


asm("int3");
    return 0;
}
MAKE_PROXY_ARG_4(OpenDevice)

void abiv0_CloseDevice(struct IORequestV0 *iORequest, struct ExecBaseV0 *SysBaseV0)
{
bug("abiv0_CloseDevice: STUB\n");
}
MAKE_PROXY_ARG_2(CloseDevice)

struct MsgPortV0 * abiv0_CreateMsgPort(struct ExecBaseV0 *SysBaseV0)
{
    struct MsgPortProxy *proxy = abiv0_AllocMem(sizeof(struct MsgPortProxy), MEMF_CLEAR, SysBaseV0);
    struct MsgPort *native = CreateMsgPort();

    proxy->base.mp_SigBit = native->mp_SigBit;
    NEWLISTV0(&proxy->base.mp_MsgList);

    proxy->native = native;

    return (struct MsgPortV0 *)proxy;
}
MAKE_PROXY_ARG_1(CreateMsgPort)

void abiv0_DeleteMsgPort(struct MsgPortV0 * port, struct ExecBaseV0 *SysBaseV0)
{
    struct MsgPortProxy *proxy = (struct MsgPortProxy *)port;
    DeleteMsgPort(proxy->native);
}
MAKE_PROXY_ARG_2(DeleteMsgPort)
struct MessageV0 * abiv0_GetMsg(struct MsgPortV0 *port, struct ExecBaseV0 *SysBaseV0)
{
    struct MsgPortProxy *proxy = (struct MsgPortProxy *)port;
    struct Message *msg = GetMsg(proxy->native);

    if (proxy->translate)
    {
        return proxy->translate(msg);
    }

    if (msg)
    {
bug("abiv0_GetMsg: STUB\n");
asm("int3");
    }

    return NULL;
}
MAKE_PROXY_ARG_2(GetMsg)

void abiv0_ReplyMsg(struct MessageV0 *message, struct ExecBaseV0 *SysBaseV0)
{
    if (message)
    {
        struct Message *native = (struct Message *)*(IPTR*)&message->mn_Node;
        ReplyMsg(native);
    }
}
MAKE_PROXY_ARG_2(ReplyMsg)

ULONG abiv0_Wait(ULONG signalSet, struct ExecBaseV0 *SysBaseV0)
{
    return Wait(signalSet);
}
MAKE_PROXY_ARG_2(Wait)

LONG abiv0_DoIO(struct IORequestV0 *IORequest, struct ExecBaseV0 *SysBaseV0)
{
bug("abiv0_DoIO: STUB\n");
    return 0;
}
MAKE_PROXY_ARG_2(DoIO)

void abiv0_SendIO(struct IORequestV0 *iORequest, struct ExecBaseV0 *SysBaseV0)
{
bug("abiv0_SendIO: STUB\n");
}
MAKE_PROXY_ARG_2(SendIO)

struct IORequestV0 *abiv0_CheckIO(struct IORequestV0 *iORequest, struct ExecBaseV0 *SysBaseV0)
{
bug("abiv0_CheckIO: STUB\n");
    return NULL;
}
MAKE_PROXY_ARG_2(CheckIO)

LONG abiv0_AbortIO(struct IORequestV0 *iORequest, struct ExecBaseV0 *SysBaseV0)
{
bug("abiv0_AbortIO: STUB\n");
    return 0;
}
MAKE_PROXY_ARG_2(AbortIO)

LONG abiv0_WaitIO(struct IORequestV0 *iORequest, struct ExecBaseV0 *SysBaseV0)
{
bug("abiv0_WaitIO: STUB\n");
    return 0;
}
MAKE_PROXY_ARG_2(WaitIO)

MAKE_PROXY_ARG_6(MakeLibrary)
MAKE_PROXY_ARG_2(AddResource)
MAKE_PROXY_ARG_2(InitSemaphore)

void abiv0_CopyMem(APTR source, APTR dest, ULONG size)
{
    return CopyMem(source, dest, size);
}
MAKE_PROXY_ARG_4(CopyMem)

APTR makeFileHandleProxy(BPTR);

struct TaskV0 *abiv0_FindTask(CONST_STRPTR name, struct ExecBaseV0 *SysBaseV0)
{
    static struct ProcessV0 *dummy = NULL;
    if (dummy == NULL) dummy = abiv0_AllocMem(sizeof(struct ProcessV0), MEMF_CLEAR, SysBaseV0);
    dummy->pr_Task.tc_Node.ln_Type = NT_PROCESS;
    dummy->pr_Task.tc_Node.ln_Name = (APTR32)(IPTR)abiv0_AllocMem(10, MEMF_CLEAR, SysBaseV0);
    strcpy((char *)(IPTR)dummy->pr_Task.tc_Node.ln_Name, "emulator");
    dummy->pr_CLI = (BPTR32)(IPTR)abiv0_AllocMem(sizeof(struct CommandLineInterfaceV0), MEMF_CLEAR, SysBaseV0);
    dummy->pr_CIS = (BPTR32)(IPTR)makeFileHandleProxy(Input());
    dummy->pr_CES = 0x2; //fake
    dummy->pr_COS = (BPTR32)(IPTR)makeFileHandleProxy(Output());
    dummy->pr_HomeDir = (BPTR32)(IPTR)makeFileHandleProxy(GetProgramDir());


    return (struct TaskV0 *)dummy;
}
MAKE_PROXY_ARG_2(FindTask)

extern ULONG *execfunctable;
APTR32 global_SysBaseV0Ptr;
struct ExecBaseV0 *abiv0SysBase;

struct ExecBaseV0 *init_exec()
{
    APTR tmp;

    tmp = AllocMem(2048, MEMF_31BIT | MEMF_CLEAR);
    abiv0SysBase = (tmp + 1024);
    global_SysBaseV0Ptr = (APTR32)(IPTR)&abiv0SysBase; /* Needed for LoadSeg32 to resolve SysBase in kernel */

    /* Keep it! This fills global variable */
    LoadSeg32("SYS:Libs32/partial/kernel", DOSBase);

    NEWLISTV0(&abiv0SysBase->LibList);
    NEWLISTV0(&abiv0SysBase->ResourceList);
    abiv0SysBase->LibNode.lib_Version = 51;

    __AROS_SETVECADDRV0(abiv0SysBase, 92, (APTR32)(IPTR)proxy_OpenLibrary);
    __AROS_SETVECADDRV0(abiv0SysBase, 69, (APTR32)(IPTR)proxy_CloseLibrary);
    __AROS_SETVECADDRV0(abiv0SysBase, 49, (APTR32)(IPTR)proxy_FindTask);
    __AROS_SETVECADDRV0(abiv0SysBase,180, (APTR32)(IPTR)proxy_AllocTaskStorageSlot);
    __AROS_SETVECADDRV0(abiv0SysBase,184, (APTR32)(IPTR)proxy_SetTaskStorageSlot);
    __AROS_SETVECADDRV0(abiv0SysBase,185, (APTR32)(IPTR)proxy_GetTaskStorageSlot);
    __AROS_SETVECADDRV0(abiv0SysBase, 83, (APTR32)(IPTR)proxy_OpenResource);
    __AROS_SETVECADDRV0(abiv0SysBase, 93, (APTR32)(IPTR)proxy_InitSemaphore);
    __AROS_SETVECADDRV0(abiv0SysBase, 33, (APTR32)(IPTR)proxy_AllocMem);
    __AROS_SETVECADDRV0(abiv0SysBase, 14, (APTR32)(IPTR)proxy_MakeLibrary);
    __AROS_SETVECADDRV0(abiv0SysBase,104, (APTR32)(IPTR)proxy_CopyMem);
    __AROS_SETVECADDRV0(abiv0SysBase,116, (APTR32)(IPTR)proxy_CreatePool);
    __AROS_SETVECADDRV0(abiv0SysBase, 74, (APTR32)(IPTR)proxy_OpenDevice);
    __AROS_SETVECADDRV0(abiv0SysBase,118, (APTR32)(IPTR)proxy_AllocPooled);
    __AROS_SETVECADDRV0(abiv0SysBase,114, (APTR32)(IPTR)proxy_AllocVec);
    __AROS_SETVECADDRV0(abiv0SysBase, 46, execfunctable[45]);    // FindName
    __AROS_SETVECADDRV0(abiv0SysBase,135, execfunctable[134]);   // TaggedOpenLibrary
    __AROS_SETVECADDRV0(abiv0SysBase, 89, (APTR32)(IPTR)proxy_TypeOfMem);
    __AROS_SETVECADDRV0(abiv0SysBase, 41, execfunctable[40]);    // AddTail
    __AROS_SETVECADDRV0(abiv0SysBase, 87, execfunctable[86]);    // RawDoFmt
    __AROS_SETVECADDRV0(abiv0SysBase, 35, (APTR32)(IPTR)proxy_FreeMem);
    __AROS_SETVECADDRV0(abiv0SysBase,113, execfunctable[112]);   // ObtainSemaphoreShared
    __AROS_SETVECADDRV0(abiv0SysBase, 94, execfunctable[93]);    // ObtainSemaphore
    __AROS_SETVECADDRV0(abiv0SysBase, 40, execfunctable[39]);    // AddHead
    __AROS_SETVECADDRV0(abiv0SysBase, 95, execfunctable[94]);    // ReleaseSemaphore
    __AROS_SETVECADDRV0(abiv0SysBase, 81, (APTR32)(IPTR)proxy_AddResource);
    __AROS_SETVECADDRV0(abiv0SysBase, 22, (APTR32)(IPTR)proxy_Forbid);
    __AROS_SETVECADDRV0(abiv0SysBase, 23, (APTR32)(IPTR)proxy_Permit);
    __AROS_SETVECADDRV0(abiv0SysBase,129, (APTR32)(IPTR)proxy_AddMemHandler);
    __AROS_SETVECADDRV0(abiv0SysBase, 70, execfunctable[69]);    // SetFunction
    __AROS_SETVECADDRV0(abiv0SysBase, 71, execfunctable[70]);    // SumLibrary
    __AROS_SETVECADDRV0(abiv0SysBase, 45, execfunctable[44]);    // Enqueue
    __AROS_SETVECADDRV0(abiv0SysBase, 34, (APTR32)(IPTR)proxy_AllocAbs);
    __AROS_SETVECADDRV0(abiv0SysBase,115, (APTR32)(IPTR)proxy_FreeVec);
    __AROS_SETVECADDRV0(abiv0SysBase,111, (APTR32)(IPTR)proxy_CreateMsgPort);
    __AROS_SETVECADDRV0(abiv0SysBase,109, execfunctable[108]);  // CreateIORequest
    __AROS_SETVECADDRV0(abiv0SysBase, 42, execfunctable[41]);   // Remove
    __AROS_SETVECADDRV0(abiv0SysBase,149, (APTR32)(IPTR)proxy_AllocVecPooled);
    __AROS_SETVECADDRV0(abiv0SysBase, 76, (APTR32)(IPTR)proxy_DoIO);
    __AROS_SETVECADDRV0(abiv0SysBase,119, (APTR32)(IPTR)proxy_FreePooled);
    __AROS_SETVECADDRV0(abiv0SysBase,107, (APTR32)(IPTR)proxy_CacheClearE);
    __AROS_SETVECADDRV0(abiv0SysBase, 39, execfunctable[38]);   // Insert
    __AROS_SETVECADDRV0(abiv0SysBase, 44, execfunctable[43]);   // RemTail
    __AROS_SETVECADDRV0(abiv0SysBase, 43, execfunctable[42]);   // RemHead
    __AROS_SETVECADDRV0(abiv0SysBase, 62, (APTR32)(IPTR)proxy_GetMsg);
    __AROS_SETVECADDRV0(abiv0SysBase, 53, (APTR32)(IPTR)proxy_Wait);
    __AROS_SETVECADDRV0(abiv0SysBase, 63, (APTR32)(IPTR)proxy_ReplyMsg);
    __AROS_SETVECADDRV0(abiv0SysBase,117, (APTR32)(IPTR)proxy_DeletePool);
    __AROS_SETVECADDRV0(abiv0SysBase,112, (APTR32)(IPTR)proxy_DeleteMsgPort);
    __AROS_SETVECADDRV0(abiv0SysBase, 75, (APTR32)(IPTR)proxy_CloseDevice);
    __AROS_SETVECADDRV0(abiv0SysBase,110, execfunctable[109]);  // DeleteIORequest
    __AROS_SETVECADDRV0(abiv0SysBase, 77, (APTR32)(IPTR)proxy_SendIO);
    __AROS_SETVECADDRV0(abiv0SysBase, 78, (APTR32)(IPTR)proxy_CheckIO);
    __AROS_SETVECADDRV0(abiv0SysBase, 80, (APTR32)(IPTR)proxy_AbortIO);
    __AROS_SETVECADDRV0(abiv0SysBase, 79, (APTR32)(IPTR)proxy_WaitIO);

    return abiv0SysBase;
}
