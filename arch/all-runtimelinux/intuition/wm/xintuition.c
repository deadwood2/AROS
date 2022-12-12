/*
    Copyright (C) 2022, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/intuition.h>

#include "xintuition.h"

#include "../../../../rom/intuition/intuition_intern.h"
#include "../../../../arch/all-runtime/hidd/x11/x11_intui_bridge.h"

#include <X11/Xutil.h>
#define X11_TYPES_H
#include "../../../../arch/all-hosted/hidd/x11/x11.h"

static void WindowTaskLoop();

static struct MsgPort *replyport;

static struct Task *windowTask;
static struct MsgPort *windowPort;

Window OpenBorderWindow(int x, int y, int width, int height, const char *title)
{
    struct Window *intuiWin = OpenWindowTags(NULL,
            WA_Left, x,
            WA_Top, y,
            WA_Width, width,
            WA_Height, height,
            WA_DragBar, TRUE,
            WA_CloseGadget, TRUE,
            WA_SizeGadget, TRUE,
            WA_DepthGadget, TRUE,
            WA_Title, title,
            WA_SizeBBottom, TRUE,
            WA_SizeBRight, TRUE,
            WA_MinWidth, 96,
            WA_MinHeight, 96,
            WA_MaxWidth, -1,
            WA_MaxHeight, -1,
            TAG_DONE);

    /* Use shared port for all windows */
    intuiWin->UserPort = windowPort;

    ModifyIDCMP(intuiWin, IDCMP_CLOSEWINDOW);

    return IW(intuiWin)->XWindow;
}

void StartupXIntuition()
{
    struct notify_msg msg;
    struct intuixchng *intuixchng = ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng);

    replyport = CreateMsgPort();

    /* Take over reading XEvents from x11hidd */
    msg.notify_type = 6;
    msg.execmsg.mn_ReplyPort = replyport;
    PutMsg(intuixchng->x11task_notify_port, &msg.execmsg);
    WaitPort(msg.execmsg.mn_ReplyPort);
    GetMsg(msg.execmsg.mn_ReplyPort);

    LockPubScreen(NULL);
    windowTask = NewCreateTask(
            TASKTAG_PC, WindowTaskLoop,
            TASKTAG_NAME, "WindowTask",
            TAG_DONE);
}

void ShutdownXIntuition()
{
    DeleteMsgPort(replyport);
    Signal(windowTask, SIGBREAKF_CTRL_C);
}

Display *GetIntuitionDisplay()
{
    struct intuixchng *intuixchng = ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng);
    return intuixchng->xdisplay;
}

void SendXEventToIntuition(XEvent *event)
{
    struct notify_msg msg;
    struct intuixchng *intuixchng = ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng);

    msg.notify_type = 5;
    msg.bmobj = (OOP_Object *)event;
    msg.execmsg.mn_ReplyPort = replyport;
    PutMsg(intuixchng->x11task_notify_port, &msg.execmsg);

    WaitPort(msg.execmsg.mn_ReplyPort);

    GetMsg(msg.execmsg.mn_ReplyPort);
}

static void WindowTaskLoop()
{
    windowPort = CreateMsgPort();

    ULONG windowSig = 1L << windowPort->mp_SigBit;

    for (;;)
    {
        struct IntuiMessage *msg;

        ULONG sigs = Wait(SIGBREAKF_CTRL_C | windowSig);

        if (sigs & SIGBREAKF_CTRL_C)
            break;

        while ((msg = (struct IntuiMessage *)GetMsg(windowPort)))
        {
            switch(msg->Class)
            {
            case IDCMP_CLOSEWINDOW:
                {
                struct Window *p = msg->IDCMPWindow;
                ReplyMsg((struct Message *)msg);
                p->UserPort = NULL;
                CloseWindow(p);
                break;
                }
            }
        }
    }

    // TODO: close outstanding not-closed windows
    // TODO: free port
}
