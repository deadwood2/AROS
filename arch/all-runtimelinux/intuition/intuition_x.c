/*
    Copyright (C) 2022, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/layers.h>

#include <intuition/intuition.h>

#include "intuition_intern.h"
#include "intuition_x.h"


/****************************************************************************************/
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include "../../arch/all-runtime/hidd/x11/x11_intui_bridge.h"
#define X11_TYPES_H
#include "../../arch/all-hosted/hidd/x11/x11.h"

#define INTUIXWINDOWCLASS "AxRT_Window"

enum
{
    ATOM_WM_CHANGE_STATE,
    ATOM_WM_DELETE_WINDOW,
    ATOM_WM_STATE,
    ATOM_WM_PROTOCOLS,

    ATOM__NET_CLOSE_WINDOW,
    ATOM__NET_ACTIVE_WINDOW,
    ATOM__NET_RESTACK_WINDOW,
    ATOM__NET_MOVERESIZE_WINDOW,
    ATOM__NET_WM_WINDOW_TYPE,
    ATOM__NET_WM_WINDOW_TYPE_DESKTOP,
    ATOM__NET_WM_WINDOW_TYPE_NORMAL,
    ATOM__NET_WM_WINDOW_TYPE_DOCK,

    ATOM__MOTIF_WM_HINTS,

    ATOM__IWM_SCREEN_TITLE,

    ATOM_LAST
};

#define GRAVITY_NORTHWEST   (1 << 0)
#define VALID_X             (1 << 8)
#define VALID_Y             (1 << 9)
#define VALID_WIDTH         (1 << 10)
#define VALID_HEIGHT        (1 << 11)
#define CLIENT_PAGER        (1 << 13)

#define likely(x)           __builtin_expect(!!(x), 1)

static Atom atoms[ATOM_LAST];
static Display *sendeventxd;

static BOOL WSL_workaround = FALSE;

VOID int_activatewindowcall(struct Window *window, struct IntuitionBase *IntuitionBase);

static struct Window *FindWindow(struct IntuitionBase *IntuitionBase, Window xwindow)
{
    struct Window *win;

    if (!IntuitionBase->ActiveScreen)
        return NULL;

    for (win = IntuitionBase->ActiveScreen->FirstWindow; win; win = win->NextWindow)
    {
        if (((struct IntWindow *)win)->XWindow == xwindow)
            return win;
    }

    return NULL;
}

/****************************************************************************************/

VOID HandleFromX11(struct IntuitionBase *IntuitionBase)
{
    struct MsgPort *port = ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng)->intuition_port;
    struct FromX11Msg *msg;

    while ((msg = (struct FromX11Msg *)GetMsg(port)))
    {
        struct Window *win = FindWindow(IntuitionBase, msg->xwindow);

        switch(msg->type)
        {
        case(FROMX11_WINDOWBITMAPRESIZED):
        {
            if (win)
            {
                /* Width */
                win->RPort->BitMap->BytesPerRow = ((msg->Width + 15) >> 3) & ~0x1;

                win->RPort->Layer->parent->bounds.MaxX = msg->Width - 1;
                win->RPort->Layer->parent->visibleshape->bounds.MaxX = msg->Width - 1;
                win->RPort->Layer->parent->visibleshape->RegionRectangle->bounds.MaxX = msg->Width - 1;
                win->RPort->Layer->parent->VisibleRegion->bounds.MaxX = msg->Width - 1;
                win->RPort->Layer->parent->Width = msg->Width;

                win->RPort->Layer->VisibleRegion->bounds.MaxX = msg->Width - 1;
                win->RPort->Layer->VisibleRegion->RegionRectangle->bounds.MaxX = msg->Width - 1;

                /* Height */
                win->RPort->BitMap->Rows = msg->Height;

                win->RPort->Layer->parent->bounds.MaxY = msg->Height - 1;
                win->RPort->Layer->parent->visibleshape->bounds.MaxY = msg->Height - 1;
                win->RPort->Layer->parent->visibleshape->RegionRectangle->bounds.MaxY = msg->Height - 1;
                win->RPort->Layer->parent->VisibleRegion->bounds.MaxY = msg->Height - 1;
                win->RPort->Layer->parent->Height = msg->Height;

                win->RPort->Layer->VisibleRegion->bounds.MaxY = msg->Height - 1;
                win->RPort->Layer->VisibleRegion->RegionRectangle->bounds.MaxY = msg->Height - 1;

            }
            break;
        }
        case(FROMX11_WINDOWPOSSIZE):
        {
            if (win)
            {
                Display *xd =  ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng)->xdisplay; // use display owned by x11gfx
                int dest_x, dest_y;
                Window child;

                /* Translate (0, 0) position of XWindow into screen coordinates */
                XTranslateCoordinates(xd, msg->xwindow, RootWindow(xd, DefaultScreen(xd)), 0, 0, &dest_x, &dest_y, &child);

                if (win->Width == msg->Width && win->Height == msg->Height)
                {
                    /* Only move */
                    DoMoveSizeWindow(win, (WORD)dest_x, (WORD)dest_y, win->Width, win->Height, FALSE, IntuitionBase);
                }
                else
                {
                    /* Only re-size */
                    DoMoveSizeWindow(win, win->LeftEdge, win->TopEdge, msg->Width, msg->Height, TRUE, IntuitionBase);
                }
            }
            break;
        }
        case(FROMX11_CLOSEWINDOW):
        {
            if (win) ih_fire_intuimessage(win,
                                 IDCMP_CLOSEWINDOW,
                                 0,
                                 win,
                                 IntuitionBase);
            break;
        }
        case(FROMX11_FOCUS):
        {
            if (win)
            {
                /* Check for WINDOWACTIVE flag. By the time roundtrip completed, window state might have changed */
                if ((msg->In) && !(win->Flags & WFLG_WINDOWACTIVE))
                {
                    int_activatewindowcall(win, IntuitionBase);
                }
                if ((!msg->In) && (win->Flags & WFLG_WINDOWACTIVE))
                {
                    ULONG lock;
                    struct Window *oldactive;

                    /* Code taken from ActivateWindow
                       Code in #if 0 / #endif blocks is left for comparison with origal code only. The assuption is
                       that activation of window has/will have taken care off all screen title operations, thus they
                       are disabled in deactivation code path. */
                    lock = LockIBase(0UL);
                    oldactive = IntuitionBase->ActiveWindow == win ? win : NULL;

                    if (oldactive)
                    {
#if 0
                        struct IntScreen *scr = NULL;
#endif

                        IntuitionBase->ActiveWindow = NULL;

                        ih_fire_intuimessage(oldactive,
                                    IDCMP_INACTIVEWINDOW,
                                    0,
                                    oldactive,
                                    IntuitionBase);

#if 0
                        scr = GetPrivScreen(oldactive->WScreen);
                        scr->Screen.Title = scr->Screen.DefaultTitle;
#endif
                    }

                    UnlockIBase(lock);

                    if (oldactive)
                    {
                        AROS_ATOMIC_AND(oldactive->Flags, ~WFLG_WINDOWACTIVE);

                        int_refreshwindowframe(oldactive, REFRESHGAD_BORDER, 0, IntuitionBase);
#if 0
                        if (!window || oldactive->WScreen != window->WScreen)
                            RenderScreenBar(oldactive->WScreen, FALSE, IntuitionBase);
#endif
                    }

                }
            }
            break;
        }
        default:
            break;
        }

        FreeMem(msg, sizeof(struct FromX11Msg));
    }
}

VOID StartupIntuitionX(struct IntuitionBase *IntuitionBase)
{
    struct Task *x11task = NULL;

    while ((x11task = FindTask("x11hidd task")) == NULL); // FindTask does not seem to work correctly in SMP!!
    GetPrivIBase(IntuitionBase)->intuixchng = x11task->tc_UserData;
    struct MsgPort *port = CreateMsgPort();
    FreeSignal(port->mp_SigBit);
    port->mp_SigBit  = -1;
    port->mp_Flags   = PA_IGNORE;
    port->mp_SigTask = NULL;
    ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng)->intuition_port = port;
    GetPrivIBase(IntuitionBase)->SBarScreen = NULL;

    /* Use separate Display to send events. If the same display used as one for receiving, sometimes events are not
       send immediately, even with XFlush() */
    sendeventxd = XOpenDisplay(NULL);

    /* Cache Atoms */
    Display *xd = sendeventxd;

    atoms[ATOM_WM_CHANGE_STATE]             = XInternAtom(xd, "WM_CHANGE_STATE",                True);
    atoms[ATOM_WM_DELETE_WINDOW]            = XInternAtom(xd, "WM_DELETE_WINDOW",               True);
    atoms[ATOM_WM_STATE]                    = XInternAtom(xd, "WM_STATE",                       True);
    atoms[ATOM_WM_PROTOCOLS]                = XInternAtom(xd, "WM_PROTOCOLS",                   True);

    atoms[ATOM__NET_CLOSE_WINDOW]           = XInternAtom(xd, "_NET_CLOSE_WINDOW",              True);
    atoms[ATOM__NET_ACTIVE_WINDOW]          = XInternAtom(xd, "_NET_ACTIVE_WINDOW",             True);
    atoms[ATOM__NET_RESTACK_WINDOW]         = XInternAtom(xd, "_NET_RESTACK_WINDOW",            True);
    atoms[ATOM__NET_MOVERESIZE_WINDOW]      = XInternAtom(xd, "_NET_MOVERESIZE_WINDOW",         True);
    atoms[ATOM__NET_WM_WINDOW_TYPE]         = XInternAtom(xd, "_NET_WM_WINDOW_TYPE",            True);
    atoms[ATOM__NET_WM_WINDOW_TYPE_DESKTOP] = XInternAtom(xd, "_NET_WM_WINDOW_TYPE_DESKTOP",    True);
    atoms[ATOM__NET_WM_WINDOW_TYPE_NORMAL]  = XInternAtom(xd, "_NET_WM_WINDOW_TYPE_NORMAL",     True);
    atoms[ATOM__NET_WM_WINDOW_TYPE_DOCK]    = XInternAtom(xd, "_NET_WM_WINDOW_TYPE_DOCK",       True);

    atoms[ATOM__MOTIF_WM_HINTS]             = XInternAtom(xd, "_MOTIF_WM_HINTS",                True);

    atoms[ATOM__IWM_SCREEN_TITLE]           = XInternAtom(xd, "_IWM_SCREEN_TITLE",              True);

    if ((atoms[ATOM__NET_CLOSE_WINDOW] == None) && (atoms[ATOM__NET_MOVERESIZE_WINDOW] == None))
    {
        WSL_workaround = TRUE;
        bug("<<INFO>>: WSL workaround enabled in Intuition\n");
    }
}


/****************************************************************************************/

static VOID sendxevent(XEvent *event, struct IntuitionBase *IntuitionBase)
{
    Display *xd = sendeventxd;
    XSendEvent(xd, RootWindow(xd, DefaultScreen(xd)), False, SubstructureNotifyMask | SubstructureRedirectMask, event);
    XFlush(xd);
}

VOID SendToWM_Close(struct Window *win, struct IntuitionBase *IntuitionBase)
{
    Window w = IW(win)->XWindow;

    if (likely(WSL_workaround == FALSE))
    {
        XEvent event;

        memset(&event, 0, sizeof(event));
        event.xclient.type = ClientMessage;
        event.xclient.window = w;
        event.xclient.message_type = atoms[ATOM__NET_CLOSE_WINDOW];
        event.xclient.format = 32;
        event.xclient.data.l[0] = 0; // FIXME eventTime;
        event.xclient.data.l[1] = 2; //Pager

        sendxevent(&event, IntuitionBase);
    }
    else
    {
        Display *xd = sendeventxd;
        XEvent event;

        memset(&event, 0, sizeof(event));
        event.xclient.type = ClientMessage;
        event.xclient.window = w;
        event.xclient.message_type = atoms[ATOM_WM_PROTOCOLS];
        event.xclient.format = 32;
        event.xclient.data.l[0] = atoms[ATOM_WM_DELETE_WINDOW];
        event.xclient.data.l[1] = 0; // FIXME eventTime;

        XSendEvent(xd, w, False, NoEventMask, &event);
        XFlush(xd);
    }
}

VOID SendToWM_Activate(struct Window *win, struct IntuitionBase *IntuitionBase)
{
    XEvent event;
    Window w = IW(win)->XWindow;

    memset(&event, 0, sizeof(event));
    event.xclient.type = ClientMessage;
    event.xclient.window = w;
    event.xclient.message_type = atoms[ATOM__NET_ACTIVE_WINDOW];
    event.xclient.format = 32;
    event.xclient.data.l[0] = 2; //Pager
    event.xclient.data.l[1] = 0; // FIXME eventTime;
    event.xclient.data.l[2] = 0; // FIXME current active

    sendxevent(&event, IntuitionBase);
}

VOID SendToWM_Restack(struct Window *win, WORD topbottom, struct IntuitionBase *IntuitionBase)
{
    if (atoms[ATOM__NET_RESTACK_WINDOW] != None)
    {
        XEvent event;
        Window w = IW(win)->XWindow;
        int detail;

        switch(topbottom)
        {
            case(-1): detail = BottomIf; break;
            case(1): detail = TopIf; break;
            default: detail = Opposite; break;
        }

        memset(&event, 0, sizeof(event));
        event.xclient.type = ClientMessage;
        event.xclient.window = w;
        event.xclient.message_type = atoms[ATOM__NET_RESTACK_WINDOW];
        event.xclient.format = 32;
        event.xclient.data.l[0] = 2;
        event.xclient.data.l[1] = 0; /* absolute */
        event.xclient.data.l[2] = detail;

        sendxevent(&event, IntuitionBase);
    }
}

VOID SendToWM_Move(struct Window *win, WORD new_left, WORD new_top, struct IntuitionBase *IntuitionBase)
{
    Window w = IW(win)->XWindow;

    if (likely(WSL_workaround == FALSE))
    {
        XEvent event;

        memset(&event, 0, sizeof(event));
        event.xclient.type = ClientMessage;
        event.xclient.window = w;
        event.xclient.message_type = atoms[ATOM__NET_MOVERESIZE_WINDOW];
        event.xclient.format = 32;
        event.xclient.data.l[0] = CLIENT_PAGER | VALID_Y | VALID_X | GRAVITY_NORTHWEST;
        event.xclient.data.l[1] = new_left;
        event.xclient.data.l[2] = new_top;

        sendxevent(&event, IntuitionBase);
    }
    else
    {
        Display *xd = sendeventxd;

        XMoveWindow(xd, w, new_left, new_top);
        XFlush(xd);
    }
}

VOID SendToWM_Resize(struct Window *win, WORD new_width, WORD new_height, struct IntuitionBase *IntuitionBase)
{
    Window w = IW(win)->XWindow;

    if (likely(WSL_workaround == FALSE))
    {
        XEvent event;

        memset(&event, 0, sizeof(event));
        event.xclient.type = ClientMessage;
        event.xclient.window = w;
        event.xclient.message_type = atoms[ATOM__NET_MOVERESIZE_WINDOW];
        event.xclient.format = 32;
        event.xclient.data.l[0] = CLIENT_PAGER | VALID_WIDTH | VALID_HEIGHT | GRAVITY_NORTHWEST;
        event.xclient.data.l[1] = 0;
        event.xclient.data.l[2] = 0;
        event.xclient.data.l[3] = new_width;
        event.xclient.data.l[4] = new_height;

        sendxevent(&event, IntuitionBase);
    }
    else
    {
        Display *xd = sendeventxd;

        XResizeWindow(xd, w, new_width, new_height);
        XFlush(xd);
    }
}

VOID SendToWM_Minimize(struct Window *win, struct IntuitionBase *IntuitionBase)
{
    XEvent event;
    Window w = IW(win)->XWindow;

    memset(&event, 0, sizeof(event));
    event.xclient.type = ClientMessage;
    event.xclient.window = w;
    event.xclient.message_type = atoms[ATOM_WM_CHANGE_STATE];
    event.xclient.format = 32;
    event.xclient.data.l[0] = IconicState;

    sendxevent(&event, IntuitionBase);
}

/****************************************************************************************/

static struct Layer *int_searchactivescreen(Window w, struct IntuitionBase *IntuitionBase)
{
    struct Window *win;

    for (win = IntuitionBase->ActiveScreen->FirstWindow; win; win = win->NextWindow)
        if (((struct IntWindow *)win)->XWindow == w)
            return win->WLayer;

    return NULL;
}

static struct Layer *int_searchchildren(Window w, struct IntuitionBase *IntuitionBase)
{
    struct Layer *l = NULL;
    Display *xd;
    Window root, child, parent;
    Window *children = NULL;
    int count, i;

    child = w;
    xd = ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng)->xdisplay; // use display owned by x11gfx

    XQueryTree(xd, child, &root, &parent, &children, &count);

    for (i = count - 1; i >= 0; i--)
    {
        child = children[i];

        l = int_searchactivescreen(child, IntuitionBase);

        if (!l)
            l = int_searchchildren(child, IntuitionBase);

        if (l)
            break;
    }

    XFree(children);
    return l;
}

struct Layer *WhichLayer_X11(struct Layer_Info *li, LONG x, LONG y, struct IntuitionBase *IntuitionBase)
{
    struct Layer *l = NULL;
    Display *xd;
    Window root, child;
    int xt,yt,mask;

    xd = ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng)->xdisplay; // use display owned by x11gfx

    /* Find window under pointer, this is:
        a) outer WM border window or
        b) Intuition window itself in case it is BORDERLESS or
        c) Intuition window itself when runnig under Intuition as Window Manager
    */
    XQueryPointer(xd, RootWindow(xd, DefaultScreen(xd)), &root, &child, &xt, &yt, &xt, &yt, &mask);
    if (child == 0)
        return NULL;

    l = int_searchactivescreen(child, IntuitionBase);

    if (l)
        return l; /* This is case b) or c) */

    /* This is case a). Intuition window is most likely a child (inner X11 window) */
    l = int_searchchildren(child, IntuitionBase);

    return l;
}

static void int_completewindowopening(Window xw, struct BitMap *bm, WORD width, WORD height,
        struct BitMap **windowBitMap, struct Layer_Info **layerInfo, struct IntuitionBase *IntuitionBase)
{
    struct MsgPort *port;
    struct intuixchng *intuixchng = (struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng;
    struct GfxBase *GfxBase = GetPrivIBase(IntuitionBase)->GfxBase;
    struct LayersBase *LayersBase = GetPrivIBase(IntuitionBase)->LayersBase;

    /* Create bitmap using this window */
    {
        struct TagItem xwindowtags [] =
        {
            {BMATags_Friend, (IPTR)bm },
            {BMATags_Private1, (IPTR)xw },
            {TAG_DONE}
        };
        (*windowBitMap) = AllocBitMap(width, height, bm->Depth,
                BMF_CHECKVALUE, (struct BitMap *)xwindowtags);
    }

    /*
     * How this works:
     * Previous call to AllocBitMap with BMATags_Private1 element got passed to x11gfx.hidd where
     * this window became the backing buffer of bitmap. Window was also registered with
     * NOTY_WINCREATE call (X11BM_InitPM->X11BM_NotifyFB). Sending the NOTY_MAPWINDOW message
     * will map the window reply. We will only continue once window is mapped (WaitPort below)
     * NOTE: this sequence requires OPTION_DELAYXWINMAPPING set in x11gfx.hidd.
     */
    {
        port = CreateMsgPort();
        struct notify_msg msg;

        msg.notify_type = NOTY_MAPWINDOW;
        msg.xdisplay = intuixchng->xdisplay;
        msg.xwindow = xw;
        msg.execmsg.mn_ReplyPort = port;
        PutMsg(intuixchng->x11task_notify_port, &msg.execmsg);
        WaitPort(msg.execmsg.mn_ReplyPort);
        GetMsg(msg.execmsg.mn_ReplyPort);
        DeleteMsgPort(port);
    }

    /* Create layer info */
    (*layerInfo) = AllocMem(sizeof(struct Layer_Info), MEMF_CLEAR);
    InitLayers(*layerInfo);
}

VOID OpenScreenBarXWindow(struct BitMap *screenBitmap, struct BitMap **barBitMap, struct Layer_Info **layerInfo,
        WORD width, WORD height, struct IntuitionBase *IntuitionBase, struct GfxBase *GfxBase,
        struct LayersBase * LayersBase)
{
    Display *xd;
    Window xw;
    int xs;
    XSizeHints *hints;
    XClassHint *classhint;
    struct MsgPort *port;
    struct intuixchng *intuixchng = ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng);

    xd =  intuixchng->xdisplay; /* Use display owned by x11gfx */

    xs = DefaultScreen(xd);
    xw = XCreateSimpleWindow(xd, RootWindow(xd, xs), 0, 0, width, height, 0, BlackPixel(xd, xs), WhitePixel(xd, xs));

    XSelectInput(xd, xw, ButtonPressMask | ButtonReleaseMask | ExposureMask);

    hints = XAllocSizeHints();
    hints->flags |= PMaxSize | PMinSize | PPosition;
    hints->min_width = hints->max_width = width;
    hints->min_height = hints->max_height = height;
    XSetWMNormalHints(xd, xw, hints);
    XFree(hints);

    /* Set the window class */
    classhint = XAllocClassHint();
    classhint->res_name = "SBar_Window";
    classhint->res_class = INTUIXWINDOWCLASS;
    XSetClassHint(xd, xw, classhint);
    XFree(classhint);

    Atom value = atoms[ATOM__NET_WM_WINDOW_TYPE_DESKTOP];
    XChangeProperty(xd, xw, atoms[ATOM__NET_WM_WINDOW_TYPE], XA_ATOM, 32, PropModeReplace, (unsigned char *) &value, 1);

    int_completewindowopening(xw, screenBitmap, width, height, barBitMap, layerInfo, IntuitionBase);
}

VOID SendToWM_SetScreenBarTitle(struct Window *win, struct IntuitionBase *IntuitionBase)
{
    if (atoms[ATOM__IWM_SCREEN_TITLE] != None)
    {
        Display *xd = sendeventxd;

        XChangeProperty(xd, RootWindow(xd, DefaultScreen(xd)), atoms[ATOM__IWM_SCREEN_TITLE], XA_STRING, 8,
            PropModeReplace, win->WScreen->Title, strlen(win->WScreen->Title));
        XFlush(xd);
    }
}

VOID OpenXWindow(struct Window *win, struct BitMap **windowBitMap, struct Layer_Info **layerInfo,
        struct IntuitionBase *IntuitionBase, struct GfxBase *GfxBase, struct LayersBase * LayersBase)
{
    Display *xd;
    Window xw;
    int xs;
    XSizeHints *hints;
    XClassHint *classhint;
    Atom wm_delete_window;
    Atom window_type_value;
    struct intuixchng *intuixchng = ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng);

    xd =  intuixchng->xdisplay; /* Use display owned by x11gfx */

    xs = DefaultScreen(xd);
    xw = XCreateSimpleWindow(xd, RootWindow(xd, xs), win->LeftEdge, win->TopEdge, win->Width, win->Height, 0,
                            BlackPixel(xd, xs), WhitePixel(xd, xs));
    if (win->Title)
        XStoreName(xd, xw, win->Title);

    XSelectInput(xd, xw, ButtonPressMask | ButtonReleaseMask | ExposureMask | PointerMotionMask | StructureNotifyMask
            | KeyPressMask | KeyReleaseMask | FocusChangeMask);

    wm_delete_window = atoms[ATOM_WM_DELETE_WINDOW];
    XSetWMProtocols(xd, xw, &wm_delete_window, 1);

    hints = XAllocSizeHints();
    hints->flags    = PPosition | PSize;
    hints->x        = win->LeftEdge;
    hints->y        = win->TopEdge;
    hints->width    = win->Width;
    hints->height   = win->Height;
    XSetWMNormalHints(xd, xw, hints);
    XFree(hints);

    /* Set the window class */
    classhint = XAllocClassHint();
    classhint->res_name = NULL;
    classhint->res_class = INTUIXWINDOWCLASS;
    XSetClassHint(xd, xw, classhint);
    XFree(classhint);

    /* Set _NET_WM_WINDOW_TYPE */
    window_type_value = atoms[ATOM__NET_WM_WINDOW_TYPE_NORMAL];

    if ((win->Flags & WFLG_BORDERLESS) && !(win->Flags & WFLG_BACKDROP))
        window_type_value = atoms[ATOM__NET_WM_WINDOW_TYPE_DOCK];

    if (win->Flags & WFLG_BACKDROP)
        window_type_value = atoms[ATOM__NET_WM_WINDOW_TYPE_DESKTOP];

    XChangeProperty(xd, xw, atoms[ATOM__NET_WM_WINDOW_TYPE], XA_ATOM, 32, PropModeReplace, (unsigned char *) &window_type_value, 1);

    /* Inform the window manager not to put any decorations on the window, they will be rendered by Intuition */
    {
        struct
        {
            unsigned long flags;
            unsigned long functions;
            unsigned long decorations;
            long          inputMode;
            unsigned long status;
        } mwmHints;

        Atom _motif_wm_hints = atoms[ATOM__MOTIF_WM_HINTS];
        mwmHints.flags = (1L << 1); /* DECORATIONS */
        mwmHints.decorations = 0; /* No decorations */
        XChangeProperty(xd, xw, _motif_wm_hints, _motif_wm_hints, 32, PropModeReplace, (unsigned char *) &mwmHints, 5);
    }

    int_completewindowopening(xw, win->WScreen->RastPort.BitMap, win->Width, win->Height, windowBitMap, layerInfo, IntuitionBase);

    IW(win)->XWindow = xw;

    XWindowLimits(win, IntuitionBase);
}

VOID GetXScreenDimensions(WORD *width, WORD *height, struct IntuitionBase *IntuitionBase)
{
    Display *xd =  ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng)->xdisplay; // use display owned by x11gfx
    *width = WidthOfScreen(DefaultScreenOfDisplay(xd));
    *height = HeightOfScreen(DefaultScreenOfDisplay(xd));
}

BOOL IsXWindowMinimized(struct Window *win, struct IntuitionBase *IntuitionBase)
{
    Display *xd =  ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng)->xdisplay; // use display owned by x11gfx
    Window xw =  IW(win)->XWindow;
    BOOL _return = FALSE;

    int status;
    unsigned long count;
    unsigned long extra;
    Atom realType;
    int realFormat;
    unsigned long *temp;

    count = 0;
    status = XGetWindowProperty(xd, xw, atoms[ATOM_WM_STATE], 0, 2, False, atoms[ATOM_WM_STATE],
                                &realType, &realFormat,&count, &extra, (unsigned char**)&temp);

    if(status == Success && realFormat != 0)
    {
        if (temp[0] == IconicState)
            _return = TRUE;

        XFree(temp);
    }

    return _return;
}

VOID XWindowLimits(struct Window *win, struct IntuitionBase *IntuitionBase)
{
    XSizeHints *hints;
    Display *xd =  ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng)->xdisplay; // use display owned by x11gfx
    Window xw =  IW(win)->XWindow;
    long temp;

    hints = XAllocSizeHints();
    XGetWMNormalHints(xd, xw, hints, &temp);

    hints->flags        |= PMinSize | PMaxSize;
    hints->min_width    = win->MinWidth;
    hints->min_height   = win->MinHeight;
    hints->max_width    = win->MaxWidth;
    hints->max_height   = win->MaxHeight;

    XSetWMNormalHints(xd, xw, hints);
    XFree(hints);
}


/* Functions DrawOutline and ClearOutline are (C) Joe Wingbermuehle, part of JWM, MIT License */

static GC outlineGC = None;
static int lastX, lastY;
static int lastWidth, lastHeight;

/** Draw an outline. */
void DrawOutline(WORD x1, WORD y1, WORD x2, WORD y2, struct IntuitionBase *IntuitionBase)
{
    Display *display =  ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng)->xdisplay; // use display owned by x11gfx
    Window rootWindow = DefaultRootWindow(display);
    int height = y2 - y1 + 1;
    int width = x2 - x1 + 1;
    int x = x1;
    int y = y1;

    XGCValues gcValues;
    gcValues.function = GXinvert;
    gcValues.subwindow_mode = IncludeInferiors;
    gcValues.line_width = 2;
    outlineGC = XCreateGC(display, rootWindow,
                            GCFunction | GCSubwindowMode | GCLineWidth,
                            &gcValues);
    XDrawRectangle(display, rootWindow, outlineGC, x, y, width, height);
    lastX = x;
    lastY = y;
    lastWidth = width;
    lastHeight = height;
}

/** Clear the last outline. */
void ClearOutline(struct IntuitionBase *IntuitionBase)
{
    Display *display =  ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng)->xdisplay; // use display owned by x11gfx
    Window rootWindow = DefaultRootWindow(display);

    if(outlineGC != None) {
        XDrawRectangle(display, rootWindow, outlineGC,
                        lastX, lastY, lastWidth, lastHeight);

        XFreeGC(display, outlineGC);
        outlineGC = None;
    }
}

/* Functions DrawOutline and ClearOutline are (C) Joe Wingbermuehle, part of JWM, MIT License */
