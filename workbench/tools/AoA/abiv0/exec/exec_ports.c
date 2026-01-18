/*
    Copyright (C) 2025-2026, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <aros/debug.h>

#include "../include/exec/structures.h"
#include "../include/exec/proxy_structures.h"
#include "../include/exec/functions.h"
#include "../include/aros/cpu.h"
#include "../include/aros/proxy.h"

#include "../support.h"

struct MsgPortV0 * abiv0_CreateMsgPort(struct ExecBaseV0 *SysBaseV0)
{
    struct MsgPortProxy *proxy = abiv0_AllocMem(sizeof(struct MsgPortProxy), MEMF_CLEAR, SysBaseV0);
    struct MsgPort *native = CreateMsgPort();

    proxy->base.mp_SigBit = native->mp_SigBit;
    NEWLISTV0(&proxy->base.mp_MsgList);
    proxy->base.mp_MsgList.l_pad = 1; /* MsgPortProxy */

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

struct MsgPortV0 * abiv0_FindPort(CONST_STRPTR name, struct ExecBaseV0 *SysBaseV0)
{
    struct MsgPort *native = FindPort(name);

    if (native)
    {
        struct MsgPortProxy *proxy = abiv0_AllocMem(sizeof(struct MsgPortProxy), MEMF_CLEAR, SysBaseV0);

        proxy->base.mp_SigBit = native->mp_SigBit;
        NEWLISTV0(&proxy->base.mp_MsgList);
        proxy->base.mp_MsgList.l_pad = 1; /* MsgPortProxy */

        proxy->native = native;
        return (struct MsgPortV0 *)proxy;
    }

    return NULL;
}
MAKE_PROXY_ARG_2(FindPort)

void abiv0_AddPort(struct MsgPortV0 *port, struct ExecBaseV0 *SysBaseV0)
{
    struct MsgPortProxy *proxy = (struct MsgPortProxy *)port;
    AddPort(proxy->native);
}
MAKE_PROXY_ARG_2(AddPort)

void abiv0_RemPort(struct MsgPortV0 * port, struct ExecBaseV0 *SysBaseV0)
{
    struct MsgPortProxy *proxy = (struct MsgPortProxy *)port;
    RemPort(proxy->native);
}
MAKE_PROXY_ARG_2(RemPort)

#define KEY32TO32   0xaabbccdd
struct Message32To32
{
    struct Message      msg;
    struct MessageV0    *v0msg;
    ULONG               key;
};

BOOL MsgPortV0_containsnative(struct MsgPortV0 *port)
{
    return (port->mp_MsgList.l_pad == 1 || port->mp_MsgList.l_pad == 3);
}

void MsgPortV0_fixed_connectnative(struct MsgPortV0 *portv0, struct MsgPort *portnative, struct ExecBaseV0 *SysBaseV0)
{
    struct MsgPortProxy *proxy = abiv0_AllocMem(sizeof(struct MsgPortProxy), MEMF_CLEAR, SysBaseV0); // MEMLEAK
    proxy->native = portnative;
    portv0->mp_MsgList.l_pad = 3;
    *((IPTR *)&portv0->mp_Node) = (IPTR)proxy;
}

struct MsgPort * MsgPortV0_getnative(struct MsgPortV0 *port)
{
    if (port->mp_MsgList.l_pad == 1)
        return ((struct MsgPortProxy *)port)->native;
    else if (port->mp_MsgList.l_pad == 3)
            return ((struct MsgPortProxy *)(*(IPTR *)(&port->mp_Node)))->native;
    else
unhandledCodePath(__func__, "no native", 0, 0);

    return NULL;
}

struct MsgPortProxy * MsgPortV0_getproxy(struct MsgPortV0 *port)
{
    if (port->mp_MsgList.l_pad == 1)
        return (struct MsgPortProxy *)port;
    else if (port->mp_MsgList.l_pad == 3)
        return (struct MsgPortProxy *)(*(IPTR *)(&port->mp_Node));

    return NULL;
}

struct MessageV0 * abiv0_GetMsg(struct MsgPortV0 *port, struct ExecBaseV0 *SysBaseV0)
{
    /* Workaround for shutdown of commodities.library */
    if (port->mp_Flags == PA_IGNORE && port->mp_MsgList.l_pad == 0) return NULL;

    struct MsgPort *nativeport = MsgPortV0_getnative(port);
    struct MsgPortProxy *proxy = MsgPortV0_getproxy(port);

    struct Message *msg = GetMsg(nativeport);

    if (msg == NULL) return NULL;

    /* 64-bit sending a message to a reader who is 32-bit */
    if (proxy && proxy->translate)
    {
        return proxy->translate(msg);
    }

    /* 32-bit sender and 32-bit reader */
    if (msg)
    {
        struct Message32To32 *m3232 = (struct Message32To32 *)msg;
        if (m3232->key == KEY32TO32)
        {
            struct MessageV0 *_ret = m3232->v0msg;
            // FreeMem(m3232, sizeof(struct Message32To32)); // causes memory damage, why?
            return _ret;
        }
    }

bug("abiv0_GetMsg: STUB\n");
 unhandledCodePath(__func__, "End", 0, 0);
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


/* Hack for NList START */
/* NList, NListtree and TextEditor do the same thing if within one application */
static struct MsgPortV0 *replyport[3];

static BOOL NList_hack_PutMsg(struct MsgPortV0 *port, struct MessageV0 *message)
{
    /* CreateNewProc returns 0x1 as Process for NList clipboard. 0x61 is me->pr_MsgPort */
    /* StartClipboardServer */
    if (port == (APTR)0x61)
    {
        message->mn_Node.ln_Name = 0xabcdef01; /* This becomes serverPort in NList. */
        for (int i = 0; i < 3; i++)
            if (replyport[i] == NULL)
            {
                replyport[i] = (struct MsgPortV0 *)(IPTR)message->mn_ReplyPort;
                return TRUE;
            }
    }

    /* ShutdownClipboardServer */
    if (port == (APTR)0xabcdef01)
    {
        return TRUE;
    }

    return FALSE;
}

static struct MessageV0 * NList_hack_WaitPort(struct MsgPortV0 *port, struct ExecBaseV0 *SysBaseV0)
{
    /* StartClipboardServer & ShutdownClipboardServer */
    for (int i = 0; i < 3; i++)
        if ((replyport[i] != NULL && port == replyport[i]))
        {
            struct MessageV0 *dummy = abiv0_AllocMem(sizeof(struct MessageV0 *), MEMF_CLEAR, SysBaseV0);
            ADDHEADV0(&port->mp_MsgList, &dummy->mn_Node);
            return (struct MessageV0 *)(IPTR)port->mp_MsgList.lh_Head;
        }

    return NULL;
}

/* Hack for NList END */

void abiv0_PutMsg(struct MsgPortV0 *port, struct MessageV0 *message, struct ExecBaseV0 *SysBaseV0)
{
    if (NList_hack_PutMsg(port, message)) return;

bug("abiv0_PutMsg: STUB\n");
    /* 32-bit sender sending to 32-bit reader (opaque transport through 64-bit MsgPort) */
    struct MsgPort *nativeport = MsgPortV0_getnative(port);
    struct Message32To32 *native = AllocMem(sizeof(struct Message32To32), MEMF_CLEAR);
    native->v0msg   = message;
    native->key     = KEY32TO32;
    if (message->mn_ReplyPort != (APTR32)(IPTR)NULL)
        native->msg.mn_ReplyPort = MsgPortV0_getnative((struct MsgPortV0 *)(IPTR)message->mn_ReplyPort);
    else
        native->msg.mn_ReplyPort = NULL;

    /* Store native message in mn_Node of v0msg for now */
    *(IPTR *)(&message->mn_Node) = (IPTR)native;

    PutMsg(nativeport, (struct Message *)native);
}
MAKE_PROXY_ARG_3(PutMsg)

struct MessageV0 *abiv0_WaitPort(struct MsgPortV0 *port, struct ExecBaseV0 *SysBaseV0)
{
    struct MessageV0 *_ret = NULL;
    if ((_ret = NList_hack_WaitPort(port, SysBaseV0)) != NULL)
        return _ret;

bug("abiv0_WaitPort: STUB\n");
    struct MsgPort *nativeport = MsgPortV0_getnative(port);
    WaitPort(nativeport);
    return _ret;
}
MAKE_PROXY_ARG_2(WaitPort)

void Exec_Ports_init(struct ExecBaseV0 *abiv0SysBase)
{
    __AROS_SETVECADDRV0(abiv0SysBase,111, (APTR32)(IPTR)proxy_CreateMsgPort);
    __AROS_SETVECADDRV0(abiv0SysBase, 62, (APTR32)(IPTR)proxy_GetMsg);
    __AROS_SETVECADDRV0(abiv0SysBase, 63, (APTR32)(IPTR)proxy_ReplyMsg);
    __AROS_SETVECADDRV0(abiv0SysBase,112, (APTR32)(IPTR)proxy_DeleteMsgPort);
    __AROS_SETVECADDRV0(abiv0SysBase, 61, (APTR32)(IPTR)proxy_PutMsg);
    __AROS_SETVECADDRV0(abiv0SysBase, 64, (APTR32)(IPTR)proxy_WaitPort);
    __AROS_SETVECADDRV0(abiv0SysBase, 65, (APTR32)(IPTR)proxy_FindPort);
    __AROS_SETVECADDRV0(abiv0SysBase, 59, (APTR32)(IPTR)proxy_AddPort);
    __AROS_SETVECADDRV0(abiv0SysBase, 60, (APTR32)(IPTR)proxy_RemPort);
}
