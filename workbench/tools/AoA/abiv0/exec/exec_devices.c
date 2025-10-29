#include <devices/ahi.h>
#include <proto/exec.h>
#include <aros/debug.h>
#include <string.h>

#include "../include/exec/structures.h"
#include "../include/exec/proxy_structures.h"
#include "../include/exec/functions.h"
#include "../include/aros/cpu.h"
#include "../include/aros/proxy.h"

#include "exec_ports.h"

extern struct LibraryV0 *abiv0TimerBase;

extern struct TaskV0 *g_v0maintask;
extern struct Task *g_nativemaintask;

struct Task * childprocess_getbyv0(struct TaskV0 *childv0);

static void io_checkandwrap_port(struct MsgPortV0 *port)
{
    if (MsgPortV0_containsnative(port))
        return;

    struct Task *sigtasknative = NULL;
    if ((struct TaskV0 *)(IPTR)port->mp_SigTask == g_v0maintask)
        sigtasknative = g_nativemaintask;
    if (sigtasknative == NULL)
        sigtasknative = childprocess_getbyv0((struct TaskV0 *)(IPTR)port->mp_SigTask);

    // MEMLEAK
    /* SDL creates an in-structure port manually to open device */
    /* SDL creates an in-structure port manually for player task */
    /* SDL_Delay uses manually created port */
    if (sigtasknative)
    {
        struct MsgPort *native = CreateMsgPort();
        FreeSignal(native->mp_SigBit);
        native->mp_SigBit = port->mp_SigBit;
        native->mp_SigTask = sigtasknative;
        MsgPortV0_fixed_connectnative(port, native);
        return;
    }
asm("int3");
}

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
        proxy->type = DEVPROXY_TYPE_INPUT;

        // proxy->mp = CreateMsgPort();
        // proxy->io = (struct IOStdReq *)CreateIORequest(proxy->mp, sizeof(struct IOStdReq));

        // OpenDevice(devName, unitNumber, (struct IORequest *)proxy->io, flags);

        iORequest->io_Device = (APTR32)(IPTR)proxy;
        return 0;
    }

    if (strcmp(devName, "console.device") == 0)
    {
bug("abiv0_OpenDevice: console.device STUB\n");
        iORequest->io_Device = 0;
        return 0;
    }

    if (strcmp(devName, "ahi.device") == 0)
    {
bug("abiv0_OpenDevice: ahi.device STUB\n");
        struct DeviceProxy *proxy = abiv0_AllocMem(sizeof(struct DeviceProxy), MEMF_CLEAR, SysBaseV0);
        io_checkandwrap_port((struct MsgPortV0 *)(IPTR)iORequest->io_Message.mn_ReplyPort);
        struct MsgPort *pnative = MsgPortV0_getnative((struct MsgPortV0 *)(IPTR)iORequest->io_Message.mn_ReplyPort);
        proxy->type = DEVPROXY_TYPE_AHI;
        proxy->io   = (struct IOStdReq *)CreateIORequest(pnative, sizeof(struct AHIRequest));
        ((struct AHIRequest *)proxy->io)->ahir_Version = 4;
        LONG _ret = OpenDevice(devName, unitNumber, (struct IORequest *)proxy->io, flags);
        if (_ret == 0)
        {
            proxy->nativeunit       = proxy->io->io_Unit;
            proxy->native           = proxy->io->io_Device;
            iORequest->io_Device    = (APTR32)(IPTR)proxy;
        }

        return _ret;
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
        // CloseDevice((struct IORequest *)proxy->io);
        // DeleteIORequest((struct IORequest *)proxy->io);
        // DeleteMsgPort(proxy->mp);
        FreeMem(proxy, sizeof(struct DeviceProxy));
        return;
    }
    if (proxy->type == DEVPROXY_TYPE_TIMER)
    {
        /* No Op */
        return;
    }
    if (proxy->type == DEVPROXY_TYPE_AHI)
    {
        CloseDevice((struct IORequest *)proxy->io);
        DeleteIORequest((struct IORequest *)proxy->io);
        FreeMem(proxy, sizeof(struct DeviceProxy));
    }
bug("abiv0_CloseDevice: STUB\n");
}
MAKE_PROXY_ARG_2(CloseDevice)

// #include "../include/input/structures.h"

// void call_handler_on_31bit_stack(struct InterruptV0 *v0handler, APTR v0chain)
// {
//     __asm__ volatile (
//         "pushq %%rbx\n"
//         "subq $8, %%rsp\n"
//         "movl %2, %%eax\n"
//         "movl %%eax, 4(%%rsp)\n"
//         "movl %1, %%eax\n"
//         "movl %%eax, (%%rsp)\n"
//         "movl %0, %%eax\n"
//         ENTER32
//         "call *%%eax\n"
//         ENTER64
//         "addq $8, %%rsp\n"
//         "popq %%rbx\n"
//         "leave\n"
//         "ret\n"
//         ::"m"(v0handler->is_Code), "m"(v0chain), "m"(v0handler->is_Data)
//         : SCRATCH_REGS_64_TO_32 );
// }

// AROS_UFH2(struct InputEvent *, EmulatorInputHandler,
//           AROS_UFHA(struct InputEvent *,      oldchain,       A0),
//           AROS_UFHA(struct DeviceProxy *,       proxy,        A1)
//          )
// {
//     AROS_USERFUNC_INIT

//     struct InputEvent *next_ie = oldchain;

//     ULONG count = 0;

//     while(next_ie) { count++; next_ie = next_ie->ie_NextEvent; }
//     struct InputEventV0 *v0chain = abiv0_AllocMem(count * sizeof(struct InputEventV0), MEMF_CLEAR, abiv0SysBase);

//     next_ie = oldchain;

//     ULONG pos = 0;
//     struct InputEventV0 *v0event = NULL;
//     while (next_ie)
//     {
//         v0event = &v0chain[pos];
//         v0event->ie_NextEvent   = (APTR32)(IPTR)&v0chain[pos + 1];

//         v0event->ie_Class       = next_ie->ie_Class;
//         v0event->ie_SubClass    = next_ie->ie_SubClass;
//         v0event->ie_Code        = next_ie->ie_Code;
//         v0event->ie_Qualifier   = next_ie->ie_Qualifier;

//         v0event->ie_position.ie_xy.ie_x = next_ie->ie_position.ie_xy.ie_x;
//         v0event->ie_position.ie_xy.ie_y = next_ie->ie_position.ie_xy.ie_y;

//         v0event->ie_TimeStamp   = next_ie->ie_TimeStamp;

//         next_ie = next_ie->ie_NextEvent;
//         pos++;
//     }
//     v0event->ie_NextEvent = (APTR32)(IPTR)NULL; /* Last event */

//     /* Switch to 31-bit stack, because handler is in 32-bit code */
//     static APTR stack31bit = NULL;
//     struct StackSwapStruct sss;
//     struct StackSwapArgs ssa;

//     if (stack31bit == NULL) stack31bit = AllocMem(64 * 1024, MEMF_CLEAR | MEMF_31BIT);

//     sss.stk_Lower = stack31bit;
//     sss.stk_Upper = sss.stk_Lower + 64 * 1024;
//     sss.stk_Pointer = sss.stk_Upper;

//     struct InterruptV0 *v0handler = (struct InterruptV0 *)proxy->user1;

//     ssa.Args[0] = (IPTR)v0handler;
//     ssa.Args[1] = (IPTR)v0chain;

//     NewStackSwap(&sss, call_handler_on_31bit_stack, &ssa);

//     FreeMem(v0chain, count * sizeof(struct InputEventV0));

//     return oldchain;

//     AROS_USERFUNC_EXIT
// }

#include <proto/timer.h>
#include "../include/timer/structures.h"

extern struct Device *TimerBase;

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
        if (IORequest->io_Command ==  9 /* TR_ADDREQUEST */)
        {
            struct timerequest *io = AllocMem(sizeof(struct timerequest), MEMF_PUBLIC | MEMF_CLEAR);
            io_checkandwrap_port((struct MsgPortV0 *)(IPTR)IORequest->io_Message.mn_ReplyPort);
            io->tr_node.io_Message.mn_ReplyPort =
                MsgPortV0_getnative((struct MsgPortV0 *)(IPTR)IORequest->io_Message.mn_ReplyPort);
            io->tr_node.io_Message.mn_Length = sizeof(struct timerequest);

            io->tr_node.io_Device = TimerBase;
            io->tr_node.io_Command = IORequest->io_Command;
            io->tr_time = ((struct timerequestV0 *)IORequest)->tr_time;
            DoIO((struct IORequest *)io);
            FreeMem(io, sizeof(struct timerequest));
            (((struct timerequestV0 *)IORequest)->tr_time).tv_micro = 0;
            (((struct timerequestV0 *)IORequest)->tr_time).tv_secs = 0;
// bug("abiv0_DoIO: TR_ADDREQUEST STUB\n");
            return 0;
        }
asm("int3");
    }

    if (proxy->type == DEVPROXY_TYPE_AHI)
    {
asm("int3");
    }

bug("abiv0_DoIO: STUB\n");
    return 0;
}
MAKE_PROXY_ARG_2(DoIO)

static struct IORequest * get_nativeio(struct IORequest **arraynative, struct IORequestV0 **arrayv0, struct IORequestV0 *pv0)
{
    if (pv0)
        for (int i = 0; i < 10; i++)
            if (arrayv0[i] == pv0) return arraynative[i];

    return NULL;
}
static struct IORequestV0 * get_v0io(struct IORequest **arraynative, struct IORequestV0 **arrayv0, struct IORequest *pnative)
{
    if (pnative)
        for (int i = 0; i < 10; i++)
            if (arraynative[i] == pnative) return arrayv0[i];

    return NULL;
}

static void cleanby_nativeio(struct IORequest **arraynative, struct IORequestV0 **arrayv0, struct IORequest *pnative)
{
    for (int i = 0; i < 10; i++)
    {
        if (arraynative[i] == pnative)
        {
            arraynative[i] = NULL;
            arrayv0[i] = NULL;
            return;
        }
    }
}

// not thread safe
LONG nativeio_request_slot(struct IORequest **arraynative)
{
    for (int i = 0; i < 10; i++)
    {
        if (arraynative[i] == NULL)
        {
            arraynative[i] = (APTR)-1;
            return i;
        }
    }

    return -1;
}

// TIMER

struct IORequest *g_trio[10];
struct IORequestV0 *g_v0trio[10];

static struct IORequest * get_trio(struct IORequestV0 *pv0)
{
    return get_nativeio(g_trio, g_v0trio, pv0);
}

static struct IORequestV0 * get_v0trio(struct IORequest *pnative)
{
    return get_v0io(g_trio, g_v0trio, pnative);
}

void cleanby_trio(struct IORequest *pnative)
{
    cleanby_nativeio(g_trio, g_v0trio, pnative);
}

// not thread safe
LONG trio_request_slot()
{
    return nativeio_request_slot(g_trio);
}

// AHI

struct AHIRequestV0
{
    struct	IOStdReqV0  ahir_Std;
    UWORD               ahir_Version;
    UWORD               ahir_Pad1;
    APTR32              ahir_Private[2];
    ULONG               ahir_Type;
    ULONG               ahir_Frequency;
    Fixed               ahir_Volume;
    Fixed               ahir_Position;
    APTR32              ahir_Link;
};

struct IORequest *g_ahiio[10];
struct IORequestV0 *g_v0ahiio[10];

static struct IORequest * get_ahiio(struct IORequestV0 *pv0)
{
    return get_nativeio(g_ahiio, g_v0ahiio, pv0);
}

static struct IORequestV0 * get_v0ahiio(struct IORequest *pnative)
{
    return get_v0io(g_ahiio, g_v0ahiio, pnative);
}

void cleanby_ahiio(struct IORequest *pnative)
{
    cleanby_nativeio(g_ahiio, g_v0ahiio, pnative);
}

// not thread safe
LONG ahiio_request_slot()
{
    return nativeio_request_slot(g_ahiio);
}

static struct MessageV0 * TRIO_translate(struct Message *ior)
{
    struct timerequest *pnative = (struct timerequest *)ior;
    struct timerequestV0 * pv0 = (struct timerequestV0 *)get_v0trio((struct IORequest *)pnative);
    if (pv0 != NULL)
    {
        pv0->tr_node.io_Error   = pnative->tr_node.io_Error;
        pv0->tr_time            = pnative->tr_time;

        cleanby_trio((struct IORequest *)pnative);

        FreeMem(pnative, sizeof(struct timerequest));
        return (struct MessageV0 *)pv0;
    }

asm("int3");
}

void abiv0_SendIO(struct IORequestV0 *iORequest, struct ExecBaseV0 *SysBaseV0)
{
    struct DeviceProxy *dproxy = (struct DeviceProxy *)(IPTR)iORequest->io_Device;
    if (dproxy->type == DEVPROXY_TYPE_TIMER)
    {
        if (iORequest->io_Command ==  9 /* TR_ADDREQUEST */)
        {
            LONG slot = trio_request_slot();

            if (slot == -1) asm("int3");

            struct timerequest *io = AllocMem(sizeof(struct timerequest), MEMF_PUBLIC | MEMF_CLEAR);
            io->tr_node.io_Message.mn_ReplyPort =
                MsgPortV0_getnative((struct MsgPortV0 *)(IPTR)iORequest->io_Message.mn_ReplyPort);
            io->tr_node.io_Message.mn_Length = sizeof(struct timerequest);

            struct MsgPortProxy *pproxy = MsgPortV0_getproxy((struct MsgPortV0 *)(IPTR)iORequest->io_Message.mn_ReplyPort);
            if (!pproxy || (pproxy->translate != NULL && pproxy->translate != TRIO_translate)) asm("int3");
            else pproxy->translate = TRIO_translate;

            io->tr_node.io_Device = TimerBase;
            io->tr_node.io_Command = iORequest->io_Command;
            io->tr_time = ((struct timerequestV0 *)iORequest)->tr_time;

            g_trio[slot] = (struct IORequest *)io;
            g_v0trio[slot] = iORequest;

            SendIO((struct IORequest *)io);
            return;
        }
asm("int3");
    }

    if (dproxy->type == DEVPROXY_TYPE_AHI)
    {
        if (iORequest->io_Command ==  3 /* CMD_WRITE */)
        {
            LONG slot = ahiio_request_slot();

            if (slot == -1) asm("int3");

            struct AHIRequest *io = AllocMem(sizeof(struct AHIRequest), MEMF_PUBLIC | MEMF_CLEAR);
            io_checkandwrap_port((struct MsgPortV0 *)(IPTR)iORequest->io_Message.mn_ReplyPort);
            io->ahir_Std.io_Message.mn_ReplyPort =
                MsgPortV0_getnative((struct MsgPortV0 *)(IPTR)iORequest->io_Message.mn_ReplyPort);
            io->ahir_Std.io_Message.mn_Node.ln_Pri = iORequest->io_Message.mn_Node.ln_Pri;
            io->ahir_Std.io_Message.mn_Length = sizeof(struct AHIRequest);

            struct MsgPortProxy *pproxy = MsgPortV0_getproxy((struct MsgPortV0 *)(IPTR)iORequest->io_Message.mn_ReplyPort);
            // if (!pproxy || (pproxy->translate != NULL && pproxy->translate != TRIO_translate)) asm("int3");
            // else pproxy->translate = TRIO_translate;

            io->ahir_Std.io_Device  = dproxy->native;
            io->ahir_Std.io_Command = iORequest->io_Command;
            io->ahir_Std.io_Unit    = dproxy->nativeunit;
            io->ahir_Std.io_Data    = (APTR)(IPTR)(((struct IOStdReqV0 *)iORequest)->io_Data);
            io->ahir_Std.io_Length  = ((struct IOStdReqV0 *)iORequest)->io_Length;
            io->ahir_Std.io_Offset  = ((struct IOStdReqV0 *)iORequest)->io_Offset;

            struct AHIRequestV0 *iov0 = ((struct AHIRequestV0 *)iORequest);
            io->ahir_Version        = iov0->ahir_Version;
            io->ahir_Frequency      = iov0->ahir_Frequency;
            io->ahir_Type           = iov0->ahir_Type;
            io->ahir_Volume         = iov0->ahir_Volume;
            io->ahir_Position       = iov0->ahir_Position;
            io->ahir_Link           = NULL;
            if (iov0->ahir_Link != (APTR32)(IPTR)NULL)
                io->ahir_Link = (struct AHIRequest *)get_ahiio((struct IORequestV0 *)(IPTR)iov0->ahir_Link);

            g_ahiio[slot] = (struct IORequest *)io;
            g_v0ahiio[slot] = iORequest;

            SendIO((struct IORequest *)io);
            return;
        }
asm("int3");
    }

bug("abiv0_SendIO: STUB\n");
}
MAKE_PROXY_ARG_2(SendIO)

struct IORequestV0 *abiv0_CheckIO(struct IORequestV0 *iORequest, struct ExecBaseV0 *SysBaseV0)
{
    struct DeviceProxy *dproxy = (struct DeviceProxy *)(IPTR)iORequest->io_Device;
    struct IORequest *ionative = NULL;

    if (dproxy->type == DEVPROXY_TYPE_TIMER)
        ionative = get_trio(iORequest);

    if (dproxy->type == DEVPROXY_TYPE_AHI)
        ionative = get_ahiio(iORequest);

    if (ionative)
    {
        if (CheckIO(ionative) == ionative)
            return iORequest;
        else
            return NULL;
    }


bug("abiv0_CheckIO: STUB\n");
    return NULL;
}
MAKE_PROXY_ARG_2(CheckIO)

LONG abiv0_AbortIO(struct IORequestV0 *iORequest, struct ExecBaseV0 *SysBaseV0)
{
    struct DeviceProxy *dproxy = (struct DeviceProxy *)(IPTR)iORequest->io_Device;
    struct IORequest *ionative = NULL;

    if (dproxy->type == DEVPROXY_TYPE_TIMER)
        ionative = get_trio(iORequest);

    if (dproxy->type == DEVPROXY_TYPE_AHI)
        ionative = get_ahiio(iORequest);

    if (ionative)
        return AbortIO(ionative);


bug("abiv0_AbortIO: STUB\n");
    return 0;
}
MAKE_PROXY_ARG_2(AbortIO)

LONG abiv0_WaitIO(struct IORequestV0 *iORequest, struct ExecBaseV0 *SysBaseV0)
{
    struct DeviceProxy *dproxy = (struct DeviceProxy *)(IPTR)iORequest->io_Device;
    struct IORequest *ionative = NULL;
    LONG _ret;

    if (dproxy->type == DEVPROXY_TYPE_TIMER)
        ionative = get_trio(iORequest);

    if (dproxy->type == DEVPROXY_TYPE_AHI)
        ionative = get_ahiio(iORequest);

    if (ionative)
    {
        _ret = WaitIO(ionative);
        if (dproxy->type == DEVPROXY_TYPE_TIMER)
        {
            cleanby_trio(ionative);
            FreeMem(ionative, sizeof(struct timerequest));
        }

        if (dproxy->type == DEVPROXY_TYPE_AHI)
        {
            cleanby_ahiio(ionative);
            FreeMem(ionative, sizeof(struct AHIRequest));
        }

        return _ret;
    }

bug("abiv0_WaitIO: STUB\n");
    return 0;
}
MAKE_PROXY_ARG_2(WaitIO)

void Exec_Devices_init(struct ExecBaseV0 *abiv0SysBase)
{
    __AROS_SETVECADDRV0(abiv0SysBase, 74, (APTR32)(IPTR)proxy_OpenDevice);
    __AROS_SETVECADDRV0(abiv0SysBase, 76, (APTR32)(IPTR)proxy_DoIO);
    __AROS_SETVECADDRV0(abiv0SysBase, 75, (APTR32)(IPTR)proxy_CloseDevice);
    __AROS_SETVECADDRV0(abiv0SysBase, 77, (APTR32)(IPTR)proxy_SendIO);
    __AROS_SETVECADDRV0(abiv0SysBase, 78, (APTR32)(IPTR)proxy_CheckIO);
    __AROS_SETVECADDRV0(abiv0SysBase, 80, (APTR32)(IPTR)proxy_AbortIO);
    __AROS_SETVECADDRV0(abiv0SysBase, 79, (APTR32)(IPTR)proxy_WaitIO);
}
