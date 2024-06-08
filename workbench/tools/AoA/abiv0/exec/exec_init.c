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

extern struct DosLibraryV0 *abiv0DOSBase;
extern struct LibraryV0 *abiv0TimerBase;
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
    "   push %%rbx\n"
    ENTER32
    "   push %%esi\n"
    "   call *%%edi\n"
    "   add $4, %%esp\n"
    ENTER64
    "   pop %%rbx\n"
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

bug("abiv0_NewStackSwap: STUB\n");

    return NewStackSwap(&sssnative, nss_trampoline, &argsnative);
}
MAKE_PROXY_ARG_4(NewStackSwap)

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
    STRPTR stripped_name = FilePart(name);

    /* Special case */
    if (strcmp(stripped_name, "dos.library") == 0)
        return abiv0DOSBase;

    /* Workaround for compiled-in absolute paths */
    if (strcmp(name, "SYS:Classes/datatypes/picture.datatype") == 0)
        name = "datatypes/picture.datatype";

    /* Call Exec function, maybe the library is already available */
    _ret = abiv0_OpenLibrary(stripped_name, version, SysBaseV0);
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
        _ret = abiv0_OpenLibrary(stripped_name, version, SysBaseV0);
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

ULONG abiv0_GetParentTaskStorageSlot(LONG id)
{
    IPTR val = GetParentTaskStorageSlot(id);
    return val;
}
MAKE_PROXY_ARG_2(GetParentTaskStorageSlot)

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

LONG abiv0_OpenDevice(CONST_STRPTR devName, ULONG unitNumber, struct IORequestV0 *iORequest, ULONG flags, struct ExecBaseV0 *SysBaseV0)
{
    if (strcmp(devName, "timer.device") == 0)
    {
        iORequest->io_Device = (APTR32)(IPTR)abiv0TimerBase;
        return 0;
    }

    if (strcmp(devName, "input.device") == 0)
    {
        struct DeviceProxy *proxy = abiv0_AllocMem(sizeof(struct DeviceProxy), MEMF_CLEAR, SysBaseV0);
        proxy->mp = CreateMsgPort();
        proxy->io = (struct IOStdReq *)CreateIORequest(proxy->mp, sizeof(struct IOStdReq));
        proxy->type = DEVPROXY_TYPE_INPUT;

        OpenDevice(devName, unitNumber, (struct IORequest *)proxy->io, flags);

        iORequest->io_Device = (APTR32)(IPTR)proxy;
        return 0;
    }

    if (strcmp(devName, "console.device") == 0)
    {
bug("abiv0_OpenDevice: console.device STUB\n");
        iORequest->io_Device = 0;
        return 0;
    }


asm("int3");
    return 0;
}
MAKE_PROXY_ARG_5(OpenDevice)

void abiv0_CloseDevice(struct IORequestV0 *iORequest, struct ExecBaseV0 *SysBaseV0)
{
    struct DeviceProxy *proxy = (struct DeviceProxy *)(IPTR)iORequest->io_Device;
    if (proxy->type == DEVPROXY_TYPE_INPUT)
    {
        CloseDevice((struct IORequest *)proxy->io);
        DeleteIORequest((struct IORequest *)proxy->io);
        DeleteMsgPort(proxy->mp);
        FreeMem(proxy, sizeof(struct DeviceProxy));
        return;
    }
    if (proxy->type == DEVPROXY_TYPE_TIMER)
    {
        /* No Op */
        return;
    }
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

/// hack just for NList
struct MsgPortV0 *rport;

void abiv0_PutMsg(struct MsgPortV0 *port, struct MessageV0 *message, struct ExecBaseV0 *SysBaseV0)
{
    rport = (struct MsgPortV0 *)(IPTR)message->mn_ReplyPort;
    message->mn_Node.ln_Name = 0x1;
bug("abiv0_PutMsg: STUB\n");
}
MAKE_PROXY_ARG_3(PutMsg)

struct MessageV0 *abiv0_WaitPort(struct MsgPortV0 *port, struct ExecBaseV0 *SysBaseV0)
{
bug("abiv0_WaitPort: STUB\n");
    if (rport != NULL && port == rport)
    {
        rport = NULL;
        struct MessageV0 *dummy = abiv0_AllocMem(sizeof(struct MessageV0 *), MEMF_CLEAR, SysBaseV0);
        ADDHEADV0(&port->mp_MsgList, &dummy->mn_Node);
        return (struct MessageV0 *)(IPTR)port->mp_MsgList.lh_Head;
    }

    struct MsgPortProxy *proxy = (struct MsgPortProxy *)port;
    WaitPort(proxy->native);
    return NULL;
}
MAKE_PROXY_ARG_2(WaitPort)
///

ULONG abiv0_Wait(ULONG signalSet, struct ExecBaseV0 *SysBaseV0)
{
    return Wait(signalSet);
}
MAKE_PROXY_ARG_2(Wait)

#include "../include/input/structures.h"

void call_handler_on_31bit_stack(struct InterruptV0 *v0handler, APTR v0chain)
{
    __asm__ volatile (
        "pushq %%rbx\n"
        "subq $8, %%rsp\n"
        "movl %2, %%eax\n"
        "movl %%eax, 4(%%rsp)\n"
        "movl %1, %%eax\n"
        "movl %%eax, (%%rsp)\n"
        "movl %0, %%eax\n"
        ENTER32
        "call *%%eax\n"
        ENTER64
        "addq $8, %%rsp\n"
        "popq %%rbx\n"
        "leave\n"
        "ret\n"
        ::"m"(v0handler->is_Code), "m"(v0chain), "m"(v0handler->is_Data)
        : SCRATCH_REGS_64_TO_32 );
}

AROS_UFH2(struct InputEvent *, EmulatorInputHandler,
          AROS_UFHA(struct InputEvent *,      oldchain,       A0),
          AROS_UFHA(struct DeviceProxy *,       proxy,        A1)
         )
{
    AROS_USERFUNC_INIT
 
    struct InputEvent *next_ie = oldchain;

    ULONG count = 0;

    while(next_ie) { count++; next_ie = next_ie->ie_NextEvent; }
    struct InputEventV0 *v0chain = abiv0_AllocMem(count * sizeof(struct InputEventV0), MEMF_CLEAR, abiv0SysBase);

    next_ie = oldchain;

    ULONG pos = 0;
    struct InputEventV0 *v0event = NULL;
    while (next_ie)
    {
        v0event = &v0chain[pos];
        v0event->ie_NextEvent   = (APTR32)(IPTR)&v0chain[pos + 1];

        v0event->ie_Class       = next_ie->ie_Class;
        v0event->ie_SubClass    = next_ie->ie_SubClass;
        v0event->ie_Code        = next_ie->ie_Code;
        v0event->ie_Qualifier   = next_ie->ie_Qualifier;

        v0event->ie_position.ie_xy.ie_x = next_ie->ie_position.ie_xy.ie_x;
        v0event->ie_position.ie_xy.ie_y = next_ie->ie_position.ie_xy.ie_y;

        v0event->ie_TimeStamp   = next_ie->ie_TimeStamp;

        next_ie = next_ie->ie_NextEvent;
        pos++;
    }
    v0event->ie_NextEvent = (APTR32)(IPTR)NULL; /* Last event */

    /* Switch to 31-bit stack, because handler is in 32-bit code */
    static APTR stack31bit = NULL;
    struct StackSwapStruct sss;
    struct StackSwapArgs ssa;

    if (stack31bit == NULL) stack31bit = AllocMem(64 * 1024, MEMF_CLEAR | MEMF_31BIT);

    sss.stk_Lower = stack31bit;
    sss.stk_Upper = sss.stk_Lower + 64 * 1024;
    sss.stk_Pointer = sss.stk_Upper;

    struct InterruptV0 *v0handler = (struct InterruptV0 *)proxy->user1;

    ssa.Args[0] = (IPTR)v0handler;
    ssa.Args[1] = (IPTR)v0chain;

    NewStackSwap(&sss, call_handler_on_31bit_stack, &ssa);

    FreeMem(v0chain, count * sizeof(struct InputEventV0));

    return oldchain;

    AROS_USERFUNC_EXIT
}

#include <proto/timer.h>
#include "../include/timer/structures.h"

LONG abiv0_DoIO(struct IORequestV0 *IORequest, struct ExecBaseV0 *SysBaseV0)
{
    struct DeviceProxy *proxy = (struct DeviceProxy *)(IPTR)IORequest->io_Device;

    if (proxy->type == DEVPROXY_TYPE_INPUT)
    {
bug("abiv0_DoIO: STUB\n");
return 0;
//         if (IORequest->io_Command == 9 /* IND_ADDHANDLER */)
//         {
//             struct InterruptV0 *v0handler = (APTR)(IPTR)((struct IOStdReqV0 *)IORequest)->io_Data;
//             if (((char *)(IPTR)v0handler->is_Node.ln_Name)[0] == 'c')
//             {
// bug("abiv0_DoIO: STUB\n");
//                 return 0; /* don't install commodities handler yet, it generates crash */
//             }

//             struct Interrupt *inputhandler = AllocMem(sizeof (struct Interrupt), MEMF_PUBLIC | MEMF_CLEAR);
//             inputhandler->is_Code = (VOID_FUNC)AROS_ASMSYMNAME(EmulatorInputHandler);
//             inputhandler->is_Data = proxy;
//             inputhandler->is_Node.ln_Pri = 0;
//             inputhandler->is_Node.ln_Name = "emulator input handler";

//             proxy->io->io_Command = 9;
//             proxy->io->io_Data = inputhandler;

//             proxy->user1 = v0handler;
//             proxy->user2 = inputhandler;

//             DoIO((struct IORequest *)proxy->io);

//             IORequest->io_Error = proxy->io->io_Error;
//             return proxy->io->io_Error;
//         }
//         else if (IORequest->io_Command == 10 /* IND_REMHANDLER */)
//         {
//             struct InterruptV0 *v0handler = (APTR)(IPTR)((struct IOStdReqV0 *)IORequest)->io_Data;

//             proxy->io->io_Command = 10;
//             proxy->io->io_Data = proxy->user2;

//             if (((char *)(IPTR)v0handler->is_Node.ln_Name)[0] == 'c')
//             {
// bug("abiv0_DoIO: STUB\n");
//                 return 0; /* don't install commodities handler yet, it generates crash */
//             }

//             DoIO((struct IORequest *)proxy->io);

//             FreeMem(proxy->user1, sizeof(struct Interrupt));
//             proxy->user1 = NULL;

//             IORequest->io_Error = proxy->io->io_Error;
//             return proxy->io->io_Error;
//         }
// asm("int3");
    }

    if (proxy->type == DEVPROXY_TYPE_TIMER)
    {
        if (IORequest->io_Command == 10 /* TR_GETSYSTIME */)
        {
            struct timeval *p = (struct timeval *)(&((struct timerequestV0 *)IORequest)->tr_time);
            GetSysTime(p);
            return 0;
        }
asm("int3");
    }

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

struct TaskV0 *g_v0Task = NULL;
extern STRPTR program_name;

struct TaskV0 *abiv0_FindTask(CONST_STRPTR name, struct ExecBaseV0 *SysBaseV0)
{
    if (name != NULL)
        asm("int3");

    if (g_v0Task == NULL)
    {
        struct ProcessV0 *dummy = NULL;
        struct CommandLineInterfaceV0 *cli = NULL;

        if (dummy == NULL) dummy = abiv0_AllocMem(sizeof(struct ProcessV0), MEMF_CLEAR, SysBaseV0);
        dummy->pr_Task.tc_Node.ln_Type = NT_PROCESS;
        dummy->pr_Task.tc_Node.ln_Name = (APTR32)(IPTR)abiv0_AllocMem(10, MEMF_CLEAR, SysBaseV0);
        strcpy((char *)(IPTR)dummy->pr_Task.tc_Node.ln_Name, "emulator");
        dummy->pr_CIS = (BPTR32)(IPTR)makeFileHandleProxy(Input());
        dummy->pr_CES = 0x2; //fake
        dummy->pr_COS = (BPTR32)(IPTR)makeFileHandleProxy(Output());
        dummy->pr_HomeDir = (BPTR32)(IPTR)makeFileHandleProxy(GetProgramDir());
        dummy->pr_Arguments = (APTR32)(IPTR)"";

        cli = abiv0_AllocMem(sizeof(struct CommandLineInterfaceV0), MEMF_CLEAR, SysBaseV0);
        cli->cli_CommandName = (APTR32)(IPTR)abiv0_AllocMem(strlen(program_name) + 1, MEMF_CLEAR, SysBaseV0);
        strcpy((char *)(IPTR)cli->cli_CommandName, program_name);

        dummy->pr_CLI = (BPTR32)(IPTR)cli;

        dummy->pr_Task.tc_Flags |= TF_ETASK;
        dummy->pr_Task.tc_UnionETask.tc_ETask = (APTR32)(IPTR)abiv0_AllocMem(sizeof(struct ETaskV0), MEMF_CLEAR, SysBaseV0);

        g_v0Task = (struct TaskV0 *)dummy;

        /* Needed for GET_THIS_TASK; TODO: change for multithreaded applications */
        SysBaseV0->ThisTask = (APTR32)(IPTR)dummy;
    }

    return g_v0Task;
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

struct ExecBaseV0 *init_exec()
{
    APTR tmp;
    TEXT path[64];

    tmp = AllocMem(2048, MEMF_31BIT | MEMF_CLEAR);
    abiv0SysBase = (tmp + 1024);
    global_SysBaseV0Ptr = (APTR32)(IPTR)&abiv0SysBase; /* Needed for LoadSeg32 to resolve SysBase in kernel */

    /* Keep it! This fills global variable */
    NewRawDoFmt("LIBSv0:partial/kernel", RAWFMTFUNC_STRING, path);
    LoadSeg32(path, DOSBase);

    NEWLISTV0(&abiv0SysBase->LibList);
    NEWLISTV0(&abiv0SysBase->ResourceList);
    abiv0SysBase->LibNode.lib_Node.ln_Name = 0x0000E0EC;
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
#if defined(SEMA_DEBUG)
    __AROS_SETVECADDRV0(abiv0SysBase,113, (APTR32)(IPTR)proxy_ObtainSemaphoreShared);
    __AROS_SETVECADDRV0(abiv0SysBase, 94, (APTR32)(IPTR)proxy_ObtainSemaphore);
#else
    __AROS_SETVECADDRV0(abiv0SysBase,113, execfunctable[112]);   // ObtainSemaphoreShared
    __AROS_SETVECADDRV0(abiv0SysBase, 94, execfunctable[93]);    // ObtainSemaphore
#endif
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
    __AROS_SETVECADDRV0(abiv0SysBase,186, (APTR32)(IPTR)proxy_GetParentTaskStorageSlot);
    __AROS_SETVECADDRV0(abiv0SysBase, 61, (APTR32)(IPTR)proxy_PutMsg);
    __AROS_SETVECADDRV0(abiv0SysBase, 64, (APTR32)(IPTR)proxy_WaitPort);
    __AROS_SETVECADDRV0(abiv0SysBase,134, (APTR32)(IPTR)proxy_NewStackSwap);
    __AROS_SETVECADDRV0(abiv0SysBase,150, (APTR32)(IPTR)proxy_FreeVecPooled);
    __AROS_SETVECADDRV0(abiv0SysBase, 55, (APTR32)(IPTR)proxy_AllocSignal);
    __AROS_SETVECADDRV0(abiv0SysBase, 56, (APTR32)(IPTR)proxy_FreeSignal);
    __AROS_SETVECADDRV0(abiv0SysBase, 96, execfunctable[95]);    // AttemptSemaphore
    __AROS_SETVECADDRV0(abiv0SysBase, 51, (APTR32)(IPTR)proxy_SetSignal);


    return abiv0SysBase;
}
