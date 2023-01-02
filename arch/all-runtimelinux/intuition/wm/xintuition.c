/*
    Copyright (C) 2022, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/intuition.h>
#include <intuition/extensions.h>

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

static struct List borderWinList;
static struct BorderWinNode *lastbwn;

struct CommandMessage
{
    struct Message ExecMessage;

    UBYTE   Type;
    IPTR    Param1;
    IPTR    Param2;
};

#define WMCMD_SET_WINDOW_TITLE  (1)
#define WMCMD_SET_SCREEN_TITLE  (2)

static void CommandTaskLoop();
static struct Task *commandTask;
static struct MsgPort *commandTaskPort;

static STRPTR screenTitleBuffer;

struct BorderWinNode
{
    struct Node     Node;
    struct Window   *Window;

    UWORD           Width;
    UWORD           Height;

    STRPTR          NameBuffer;
};

Window OpenBorderWindow(int x, int y, int width, int height, const char *title)
{
    struct BorderWinNode *bwn = AllocMem(sizeof(struct BorderWinNode), MEMF_PUBLIC | MEMF_CLEAR);
    bwn->NameBuffer = StrDup(title);

    struct Window *intuiWin = OpenWindowTags(NULL,
            WA_Left, x,
            WA_Top, y,
            WA_Width, width,
            WA_Height, height,
            WA_DragBar, TRUE,
            WA_CloseGadget, TRUE,
            WA_SizeGadget, TRUE,
            WA_DepthGadget, TRUE,
            WA_Title, bwn->NameBuffer,
            WA_SizeBBottom, TRUE,
            WA_SizeBRight, TRUE,
            WA_MinWidth, 96,
            WA_MinHeight, 96,
            WA_MaxWidth, -1,
            WA_MaxHeight, -1,
            WA_ExtraGadget_Iconify, TRUE,
            TAG_DONE);

    /* Use shared port for all windows */
    intuiWin->UserPort = windowPort;

    ModifyIDCMP(intuiWin, IDCMP_CLOSEWINDOW | IDCMP_NEWSIZE | IDCMP_CHANGEWINDOW);

    bwn->Window = intuiWin;
    bwn->Width  = width;
    bwn->Height = height;

/* FIXME: this should be semaphore protected or be separate list that is added to main one by window loop task*/
    AddTail(&borderWinList, &bwn->Node);

    return IW(intuiWin)->XWindow;
}

void StartupXIntuition()
{
    struct notify_msg msg;
    struct intuixchng *intuixchng = ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng);
    NEWLIST(&borderWinList);

    replyport = CreateMsgPort();

    /* Take over reading XEvents from x11hidd */
    msg.notify_type = 6;
    msg.execmsg.mn_ReplyPort = replyport;
    PutMsg(intuixchng->x11task_notify_port, &msg.execmsg);
    WaitPort(msg.execmsg.mn_ReplyPort);
    GetMsg(msg.execmsg.mn_ReplyPort);

    windowTask = NewCreateTask(
            TASKTAG_PC, WindowTaskLoop,
            TASKTAG_NAME, "WindowTask",
            TAG_DONE);

    commandTask = NewCreateTask(
            TASKTAG_PC, CommandTaskLoop,
            TASKTAG_NAME, "CommandTask",
            TAG_DONE);

}

void ShutdownXIntuition()
{
    DeleteMsgPort(replyport);
    Signal(windowTask, SIGBREAKF_CTRL_C);
    Signal(commandTask, SIGBREAKF_CTRL_C);
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

static struct BorderWinNode *FindByIntuiWindow(struct Window *w)
{
    struct BorderWinNode *bwn = NULL;
    struct Node *nxt = NULL;

    if (lastbwn && lastbwn->Window == w)
        return lastbwn;
    else
    {
        ForeachNodeSafe(&borderWinList, bwn, nxt)
        {
            if (bwn->Window == w)
            {
                lastbwn = bwn;
                return bwn;
            }
        }
    }
}

static struct BorderWinNode *FindByXWindow(Window w)
{
    struct BorderWinNode *bwn = NULL;
    struct Node *nxt = NULL;

    ForeachNodeSafe(&borderWinList, bwn, nxt)
        if (IW(bwn->Window)->XWindow == w)
            return bwn;

    return NULL;
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
            struct Window *w = msg->IDCMPWindow;
            struct BorderWinNode *bwn = NULL;

            bwn = FindByIntuiWindow(w);

            if (bwn == NULL)
            {
                ReplyMsg((struct Message *)msg);
                bug("WindowTaskLoop: Not registered window %p\n", w);
                continue;
            }

            switch(msg->Class)
            {
            case IDCMP_CLOSEWINDOW:
                ReplyMsg((struct Message *)msg);
                w->UserPort = NULL;
                CloseWindow(w);
                Remove((struct Node *)bwn);
                lastbwn = NULL;
                FreeVec(bwn->NameBuffer);
                FreeMem(bwn, sizeof(struct BorderWinNode));
                break;
            case IDCMP_NEWSIZE:
            case IDCMP_CHANGEWINDOW:
                if (bwn->Width != w->Width || bwn->Height != w->Height)
                {
                    ReplyMsg((struct Message *)msg);
                    BeginRefresh(w);
                    EndRefresh(w, TRUE);
                    bwn->Width = w->Width;
                    bwn->Height = w->Height;
                }
                break;
            default:
                bug("WindowTaskLoop: Not handled class %x\n", msg->Class);
                ReplyMsg((struct Message *)msg);
                break;
            }
        }
    }

    // TODO: close outstanding not-closed windows
    // TODO: free port
}

/*  Commands are executed on separate task, not to cause deadlocks between Intuition and JWM main task.
    This happens in following situation:
        Window 1 active, Window 2 inactive, Window 2 is clicked and dragged
        Window 2 locks layers for move
            Window 1 becomes inactive which causes refresh of boarder on JWM side which refreshes title
                Title refreshing blocks on layers. If this is JWM main task, then it stops pumping events
                to x11gfx.hidd and move never finishes, never unlocking the layers
*/

static void CommandTaskLoop()
{
    commandTaskPort = CreateMsgPort();

    ULONG commandSig = 1L << commandTaskPort->mp_SigBit;

    for (;;)
    {
        struct CommandMessage *msg;

        ULONG sigs = Wait(SIGBREAKF_CTRL_C | commandSig);

        if (sigs & SIGBREAKF_CTRL_C)
            break;

        while ((msg = (struct CommandMessage *)GetMsg(commandTaskPort)))
        {
            switch(msg->Type)
            {
            case(WMCMD_SET_WINDOW_TITLE):
                SetWindowTitles((struct Window *)msg->Param1, (CONST_STRPTR)msg->Param2, (CONST_STRPTR)~0L);
                break;
            case(WMCMD_SET_SCREEN_TITLE):
            {
                struct Window dummy;
                dummy.WScreen = (struct Screen *)msg->Param1;
                dummy.Flags |= WFLG_WINDOWACTIVE;
                SetWindowTitles(&dummy, (CONST_STRPTR)~0L, (CONST_STRPTR)msg->Param2);
                break;
            }
            }

            FreeMem(msg, sizeof(struct CommandMessage));
        }
    }
}

void SetScreenTitle(const char *title)
{
    if (screenTitleBuffer) FreeVec(screenTitleBuffer);
    screenTitleBuffer = StrDup(title);

    struct CommandMessage *msg2 = AllocMem(sizeof(struct CommandMessage), MEMF_CLEAR);
    msg2->Type = WMCMD_SET_SCREEN_TITLE;
    msg2->Param1 = (IPTR)GetPrivIBase(IntuitionBase)->SBarScreen;
    msg2->Param2 = (IPTR)screenTitleBuffer;
    PutMsg(commandTaskPort, &msg2->ExecMessage);
}

void SetBorderWindowTitle(Window w, const char *title)
{
    struct BorderWinNode *bwn = FindByXWindow(w);

    if (bwn)
    {
        FreeVec(bwn->NameBuffer);
        bwn->NameBuffer = StrDup(title);

        struct CommandMessage *msg = AllocMem(sizeof(struct CommandMessage), MEMF_CLEAR);
        msg->Type = WMCMD_SET_WINDOW_TITLE;
        msg->Param1 = (IPTR)bwn->Window;
        msg->Param2 = (IPTR)bwn->NameBuffer;
        PutMsg(commandTaskPort, &msg->ExecMessage);
    }
}

char IsSBarWindow(const char *className, const char *instanceName)
{
    if (className == NULL || instanceName == NULL)
        return 0;

    if (strcmp(className, "AxRT_Window") == 0)
        if (strcmp(instanceName, "SBar_Window") == 0)
            return 1;

    return 0;
}
