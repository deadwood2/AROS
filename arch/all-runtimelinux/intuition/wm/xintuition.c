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

static struct MsgPort *replyport;

// temp
struct Window *intuiWin = NULL;

Window OpenBorderWindow(int x, int y, int width, int height, const char *title)
{
        intuiWin = OpenWindowTags(NULL,
        WA_Left, x,
        WA_Top, y,
        WA_Width, width,
        WA_Height, height,
        WA_DragBar, TRUE,
        WA_CloseGadget, TRUE,
        WA_SizeGadget, TRUE,
        WA_DepthGadget, TRUE,
        WA_Title, title,
        WA_IDCMP, IDCMP_CLOSEWINDOW);

    return IW(intuiWin)->XWindow;
}

void StartupXIntuition()
{
    LockPubScreen(NULL);
    replyport = CreateMsgPort();
}

void ShutdownXIntuition()
{
    DeleteMsgPort(replyport);
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

void ProcessIntuiMessages()
{
    // Fixme: instead of this beeing called in context of main task, there
    // should be a separate task that just process window messages from
    // all opened windows.
    struct IntuiMessage *msg;
    BOOL quitme = FALSE;

    if (intuiWin == NULL)
        return;

    while ((msg = (struct IntuiMessage *)GetMsg(intuiWin->UserPort)))
    {
        switch(msg->Class)
        {
        case IDCMP_CLOSEWINDOW:
            quitme = TRUE;
            break;
        }
        ReplyMsg((struct Message *)msg);
    }

    if (quitme)
    {
        CloseWindow(intuiWin);
        intuiWin = NULL;
    }

}
