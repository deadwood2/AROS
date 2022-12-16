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

                    lock = LockIBase(0UL);
                    oldactive = IntuitionBase->ActiveWindow == win ? win : NULL;

                    if (oldactive)
                    {
                        /* Code taken from ActivateWindow */
                        struct IntScreen *scr = NULL;

                        IntuitionBase->ActiveWindow = NULL;

                        ih_fire_intuimessage(oldactive,
                                    IDCMP_INACTIVEWINDOW,
                                    0,
                                    oldactive,
                                    IntuitionBase);

                        scr = GetPrivScreen(oldactive->WScreen);
                        scr->Screen.Title = scr->Screen.DefaultTitle;
                    }

                    UnlockIBase(lock);

                    if (oldactive)
                    {
                        AROS_ATOMIC_AND(oldactive->Flags, ~WFLG_WINDOWACTIVE);

                        int_refreshwindowframe(oldactive, REFRESHGAD_BORDER, 0, IntuitionBase);
                        if (1) /* !window is always TRUE */
                            RenderScreenBar(oldactive->WScreen, FALSE, IntuitionBase);
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

/****************************************************************************************/

VOID SendClientMessageClose(struct Window *win, struct IntuitionBase *IntuitionBase)
{
    XEvent event;
    int status;
    Window w = IW(win)->XWindow;

    Display *xd = ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng)->xdisplay; // use display owned by x11gfx
    Atom _net_close_window = XInternAtom(xd, "_NET_CLOSE_WINDOW", False);

    memset(&event, 0, sizeof(event));
    event.xclient.type = ClientMessage;
    event.xclient.window = w;
    event.xclient.message_type = _net_close_window;
    event.xclient.format = 32;
    event.xclient.data.l[0] = 0; // FIXME eventTime;
    event.xclient.data.l[1] = 2; //Pager

    status = XSendEvent(xd, RootWindow(xd, DefaultScreen(xd)), False, SubstructureNotifyMask | SubstructureRedirectMask, &event);
}

VOID SendClientMessageActive(struct Window *win, struct IntuitionBase *IntuitionBase)
{
    XEvent event;
    int status;
    Window w = IW(win)->XWindow;

    Display *xd = ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng)->xdisplay; // use display owned by x11gfx
    Atom _net_active_window = XInternAtom(xd, "_NET_ACTIVE_WINDOW", False);

    memset(&event, 0, sizeof(event));
    event.xclient.type = ClientMessage;
    event.xclient.window = w;
    event.xclient.message_type = _net_active_window;
    event.xclient.format = 32;
    event.xclient.data.l[0] = 2; //Pager
    event.xclient.data.l[1] = 0; // FIXME eventTime;
    event.xclient.data.l[2] = 0; // FIXME current active

    status = XSendEvent(xd, RootWindow(xd, DefaultScreen(xd)), False, SubstructureNotifyMask | SubstructureRedirectMask, &event);
}

VOID SendClientMessageRestack(struct Window *win, WORD topbottom, struct IntuitionBase *IntuitionBase)
{
    XEvent event;
    int status;
    Window w = IW(win)->XWindow;
    int detail;

    Display *xd = ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng)->xdisplay; // use display owned by x11gfx
    Atom _net_restack_window = XInternAtom(xd, "_NET_RESTACK_WINDOW", False);

    switch(topbottom)
    {
        case(-1): detail = BottomIf; break;
        case(1): detail = TopIf; break;
        default: detail = Opposite; break;
    }

    memset(&event, 0, sizeof(event));
    event.xclient.type = ClientMessage;
    event.xclient.window = w;
    event.xclient.message_type = _net_restack_window;
    event.xclient.format = 32;
    event.xclient.data.l[0] = 2;
    event.xclient.data.l[1] = 0; /* absolute */
    event.xclient.data.l[2] = detail;

    status = XSendEvent(xd, RootWindow(xd, DefaultScreen(xd)), False, SubstructureNotifyMask | SubstructureRedirectMask, &event);
}

VOID SendClientMessageMove(struct Window *win, WORD new_left, WORD new_top, struct IntuitionBase *IntuitionBase)
{
    XEvent event;
    int status;
    Window w = IW(win)->XWindow;

    Display *xd = ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng)->xdisplay; // use display owned by x11gfx
    Atom _net_moveresize_window = XInternAtom(xd, "_NET_MOVERESIZE_WINDOW", False);

    memset(&event, 0, sizeof(event));
    event.xclient.type = ClientMessage;
    event.xclient.window = w;
    event.xclient.message_type = _net_moveresize_window;
    event.xclient.format = 32;
    event.xclient.data.l[0] = 8961;
    event.xclient.data.l[1] = new_left;
    event.xclient.data.l[2] = new_top;

    status = XSendEvent(xd, RootWindow(xd, DefaultScreen(xd)), False, SubstructureNotifyMask | SubstructureRedirectMask, &event);
}

VOID SendClientMessageResize(struct Window *win, WORD new_width, WORD new_height, struct IntuitionBase *IntuitionBase)
{
    XEvent event;
    int status;
    Window w = IW(win)->XWindow;

    Display *xd = ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng)->xdisplay; // use display owned by x11gfx
    Atom _net_moveresize_window = XInternAtom(xd, "_NET_MOVERESIZE_WINDOW", False);

    memset(&event, 0, sizeof(event));
    event.xclient.type = ClientMessage;
    event.xclient.window = w;
    event.xclient.message_type = _net_moveresize_window;
    event.xclient.format = 32;
    event.xclient.data.l[0] = 11265;
    event.xclient.data.l[1] = 0;
    event.xclient.data.l[2] = 0;
    event.xclient.data.l[3] = new_width;
    event.xclient.data.l[4] = new_height;

    status = XSendEvent(xd, RootWindow(xd, DefaultScreen(xd)), False, SubstructureNotifyMask | SubstructureRedirectMask, &event);
}

/****************************************************************************************/

static struct Layer *SearchActiveScreen(Window w, struct IntuitionBase *IntuitionBase)
{
    struct Window *win;

    for (win = IntuitionBase->ActiveScreen->FirstWindow; win; win = win->NextWindow)
        if (((struct IntWindow *)win)->XWindow == w)
            return win->WLayer;

    return NULL;
}

struct Layer *WhichLayer_X11(struct Layer_Info *li, LONG x, LONG y, struct IntuitionBase *IntuitionBase)
{
    struct Layer *l = NULL;
    Display *xd;
    Window root, child, parent;
    Window *children = NULL;
    int count, i;
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

    l = SearchActiveScreen(child, IntuitionBase);

    if (l)
        return l; /* This is case b) or c) */

    /* This is case a). Intuition window is most likely a child (inner X11 window) */
    XQueryTree(xd, child, &root, &parent, &children, &count);
    for (i = count - 1; i >= 0; i--)
    {
        child = children[i];
        l = SearchActiveScreen(child, IntuitionBase);
    }

    XFree(children);

    return l;
}

VOID AdjustFlagsForWM(struct NewWindow *nw, struct IntuitionBase *IntuitionBase)
{
    XClassHint classhint;
    Display *xd;
    int xs;
    struct intuixchng *intuixchng = ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng);

    xd =  intuixchng->xdisplay; /* Use display owned by x11gfx */
    xs = DefaultScreen(xd);

    XGetClassHint(xd, RootWindow(xd, xs), &classhint);

    if (classhint.res_class[0] != 'I' && classhint.res_name[0] != 'I')
    {
        /* Not running under Intuition, so border will come from window manager */
        if (!(nw->Flags & WFLG_BORDERLESS))
        {
            nw->Flags &= ~(WFLG_SIZEBRIGHT|WFLG_SIZEBBOTTOM|WFLG_SIZEGADGET|WFLG_CLOSEGADGET|WFLG_DEPTHGADGET|WFLG_HASZOOM);
            nw->Flags |= WFLG_BORDERLESS;
            nw->Flags |= WFLG_BORDERLESSNOTREALLY;
        }
        /* else BORDELESS window actually requested, do nothing */
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
    struct MsgPort *port;
    struct intuixchng *intuixchng = ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng);
    WORD ypos   = win->TopEdge;
    WORD height = win->Height;

#ifdef AXRTBARHACK
    if (win->Flags & WFLG_BACKDROP)
    {
        ypos    += win->WScreen->BarHeight;
        height  -= win->WScreen->BarHeight;
    }
#endif

    xd =  intuixchng->xdisplay; /* Use display owned by x11gfx */

    xs = DefaultScreen(xd);
    xw = XCreateSimpleWindow(xd, RootWindow(xd, xs), win->LeftEdge, ypos, win->Width, height, 0,
                            BlackPixel(xd, xs), WhitePixel(xd, xs));
    if (win->Title)
        XStoreName(xd, xw, win->Title);

    XSelectInput(xd, xw, ButtonPressMask | ButtonReleaseMask | ExposureMask | PointerMotionMask | StructureNotifyMask
            | KeyPressMask | KeyReleaseMask | FocusChangeMask);
    XSetWMProtocols(xd, xw, &((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng)->delete_win_atom, 1);

    hints = XAllocSizeHints();
    hints->flags    = PPosition | PSize;
    hints->x        = win->LeftEdge;
    hints->y        = win->TopEdge;
    hints->width    = win->Width;
    hints->height   = height;
    XSetWMNormalHints(xd, xw, hints);
    XFree(hints);

    /* Set the window class */
    classhint = XAllocClassHint();
    classhint->res_name = "AxRuntime";
    classhint->res_class = "AxRuntime";
    XSetClassHint(xd, xw, classhint);
    XFree(classhint);

    if (((win->Flags & WFLG_BORDERLESS) && !(win->Flags & WFLG_BORDERLESSNOTREALLY)) && !(win->Flags & WFLG_BACKDROP))
    {
        Atom window_type = XInternAtom(xd, "_NET_WM_WINDOW_TYPE", False);
        Atom value = XInternAtom(xd, "_NET_WM_WINDOW_TYPE_DOCK", False);
        XChangeProperty(xd, xw, window_type, XA_ATOM, 32, PropModeReplace, (unsigned char *) &value, 1);
    }

    if (win->Flags & WFLG_BACKDROP)
    {
        Atom window_type = XInternAtom(xd, "_NET_WM_WINDOW_TYPE", False);
        Atom value = XInternAtom(xd, "_NET_WM_WINDOW_TYPE_DESKTOP", False);
        XChangeProperty(xd, xw, window_type, XA_ATOM, 32, PropModeReplace, (unsigned char *) &value, 1);
    }

    /* Create bitmap using this window */
    {
        struct TagItem xwindowtags [] =
        {
            {BMATags_Friend, (IPTR)win->WScreen->RastPort.BitMap },
            {BMATags_Private1, (IPTR)xw },
            {TAG_DONE}
        };
        (*windowBitMap) = AllocBitMap(win->Width, height, win->WScreen->RastPort.BitMap->Depth,
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
        msg.xdisplay = xd;
        msg.xwindow = xw;
        msg.execmsg.mn_ReplyPort = port;
        PutMsg(intuixchng->x11task_notify_port, &msg.execmsg);
        WaitPort(msg.execmsg.mn_ReplyPort);
        GetMsg(msg.execmsg.mn_ReplyPort);
        DeleteMsgPort(port);
    }

    IW(win)->XWindow = xw;

    XWindowLimits(win, IntuitionBase);

    /* Create layer info */
    (*layerInfo) = AllocMem(sizeof(struct Layer_Info), MEMF_CLEAR);
    InitLayers(*layerInfo);

}

VOID GetXScreenDimensions(WORD *width, WORD *height, struct IntuitionBase *IntuitionBase)
{
    Display *xd =  ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng)->xdisplay; // use display owned by x11gfx
    *width = WidthOfScreen(DefaultScreenOfDisplay(xd));
    *height = HeightOfScreen(DefaultScreenOfDisplay(xd));
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
