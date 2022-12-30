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
    XFlush(xd); /* Push event to X server immediatelly */
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

VOID SendMessageMinimize(struct Window *win, struct IntuitionBase *IntuitionBase)
{
    XEvent event;
    int status;
    Window w = IW(win)->XWindow;

    Display *xd = ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng)->xdisplay; // use display owned by x11gfx
    Atom wm_change_state = XInternAtom(xd, "WM_CHANGE_STATE", False);

    memset(&event, 0, sizeof(event));
    event.xclient.type = ClientMessage;
    event.xclient.window = w;
    event.xclient.message_type = wm_change_state;
    event.xclient.format = 32;
    event.xclient.data.l[0] = IconicState;

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

    Atom window_type = XInternAtom(xd, "_NET_WM_WINDOW_TYPE", False);
    Atom value = XInternAtom(xd, "_NET_WM_WINDOW_TYPE_DESKTOP", False);
    XChangeProperty(xd, xw, window_type, XA_ATOM, 32, PropModeReplace, (unsigned char *) &value, 1);

    int_completewindowopening(xw, screenBitmap, width, height, barBitMap, layerInfo, IntuitionBase);
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

    wm_delete_window = XInternAtom(xd, "WM_DELETE_WINDOW", FALSE);
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
    window_type_value = XInternAtom(xd, "_NET_WM_WINDOW_TYPE_NORMAL", False);

    if ((win->Flags & WFLG_BORDERLESS) && !(win->Flags & WFLG_BACKDROP))
        window_type_value = XInternAtom(xd, "_NET_WM_WINDOW_TYPE_DOCK", False);

    if (win->Flags & WFLG_BACKDROP)
        window_type_value = XInternAtom(xd, "_NET_WM_WINDOW_TYPE_DESKTOP", False);

    Atom window_type = XInternAtom(xd, "_NET_WM_WINDOW_TYPE", False);
    XChangeProperty(xd, xw, window_type, XA_ATOM, 32, PropModeReplace, (unsigned char *) &window_type_value, 1);

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

        Atom _motif_wm_hints = XInternAtom(xd, "_MOTIF_WM_HINTS", False);
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
