/*
    Copyright (C) 1995-2017, The AROS Development Team. All rights reserved.

    Desc: X11 hidd. Connects to the X server and receives events.
*/

#include "x11_debug.h"

#include <hardware/intbits.h>

#include <X11/keysym.h>
#include <signal.h>

#include "x11_types.h"
#include "x11.h"
#include "x11_hostlib.h"
#include "x11gfx_fullscreen.h"
#include "../../../all-runtime/hidd/x11/x11_intui_bridge.h"

extern struct intuixchng intuixchng;

VOID X11BM_ExposeFB(APTR data, WORD x, WORD y, WORD width, WORD height);
BOOL X11BM_ResizePMIfNeeded(OOP_Class *cl, OOP_Object *o, UWORD reqwidth, UWORD reqheight, UWORD *actwidth, UWORD *actheight);

/****************************************************************************************/

#define BETTER_REPEAT_HANDLING  1

#define XTASK_NAME "x11hidd task"

/* We need to have highest priority for this task, because we
 are simulating an interrupt. I.e. an "interrupt handler" called
 by this task should NEVER be interrupted by a task (for example input.device),
 otherwise it will give strange effects, especially in the circular-buffer
 handling in gameport/keyboard. (Writing to the buffer is not atomic even
 from within the IRQ handler!)

 Instead of calling
 the irq handler directly from the task, we should instead
 Cause() a software irq, but Cause() does not work at the moment..
 */

#define XTASK_PRIORITY  50

#define XTASK_STACKSIZE 100000

#undef XSD
#define XSD(cl) xsd

struct KeyReleaseInfo
{
    BOOL f12_down;
#if BETTER_REPEAT_HANDLING
    XEvent keyrelease_event;
    BOOL keyrelease_pending;
#endif
};

/****************************************************************************************/

AROS_INTH1(x11VBlank, struct Task *, task)
{
    AROS_INTFUNC_INIT

    DB2(bug("[X11] %s()\n", __PRETTY_FUNCTION__));

    Signal(task, SIGBREAKF_CTRL_D);

    return FALSE;

    AROS_INTFUNC_EXIT
}

/****************************************************************************************/

static VOID x11task_process_xevent(struct x11_staticdata *xsd, struct MinList *xwindowlist, XEvent *event,
        struct KeyReleaseInfo *kri, struct MinList *nmsg_list)
{
    struct xwinnode *node;
    BOOL window_found = FALSE;
    KeySym ks;
    struct FromX11Msg *msg;

    ForeachNode(xwindowlist, node)
    {
        if (node->xwindow == event->xany.window)
        {
            window_found = TRUE;
            break;
        }
    }

    if (x11clipboard_want_event(event))
    {
        x11clipboard_handle_event(xsd, event);
    }

    if (window_found)
    {
        D(bug("Got event for window %x\n", event->xany.window));
        switch (event->type)
        {
        case GraphicsExpose:
            break;

        case Expose:
            LOCK_X11
            X11BM_ExposeFB(OOP_INST_DATA(OOP_OCLASS(node->bmobj), node->bmobj), event->xexpose.x,
                    event->xexpose.y, event->xexpose.width, event->xexpose.height);
            UNLOCK_X11
            break;

        case ConfigureRequest:
            bug("!!! CONFIGURE REQUEST !!\n");
            break;

#if 0
        /* stegerg: not needed */
        case ConfigureNotify:
        {
            /* The window has been resized */

            XConfigureEvent *me;
            struct notify_msg *nmsg, *safe;

            me = (XConfigureEvent *)event;
            ForeachNodeSafe(&nmsg_list, nmsg, safe)
            {
                if ( me->window == nmsg->xwindow
                        && nmsg->notify_type == NOTY_RESIZEWINDOW)
                {
                    /*  The window has now been mapped.
                        Send reply to app */

                    Remove((struct Node *)nmsg);
                    ReplyMsg((struct Message *)nmsg);
                }
            }

            break;
        }
#endif

        case ConfigureNotify:
        {
            if (intuixchng.intuition_port)
            {
                XConfigureEvent *me;
                me = (XConfigureEvent *)event;

                {
                    UWORD actwidth = 0, actheight = 0;

                    D(bug("ConfigureNotify (w)(x,y,w,h) (%ld)(%d, %d, %d, %d)\n", me->window, me->x, me->y, me->width, me->height));
                    if (X11BM_ResizePMIfNeeded(OOP_OCLASS(node->bmobj), node->bmobj, me->width, me->height, &actwidth, &actheight))
                    {
                        D(bug("ConfigureNotify, resized bitmap to %d, %d\n", actwidth, actheight));
                        msg = AllocMem(sizeof(struct FromX11Msg), MEMF_CLEAR);
                        msg->type   = FROMX11_WINDOWBITMAPRESIZED;
                        msg->xwindow = me->window;
                        msg->Width  = actwidth;
                        msg->Height = actheight;
                        PutMsg(intuixchng.intuition_port,(struct Message *)msg);
                    }
                    msg = AllocMem(sizeof(struct FromX11Msg), MEMF_CLEAR);
                    msg->type   = FROMX11_WINDOWPOSSIZE;
                    msg->xwindow = me->window;
                    msg->LeftEdge   = -1; /* Coordinates are parent-relative, mark as invalid */
                    msg->TopEdge    = -1;
                    msg->Width  = me->width;
                    msg->Height = me->height;
                    PutMsg(intuixchng.intuition_port,(struct Message *)msg);
                }

            }

            break;
        }
        case ButtonPress:
            xsd->x_time = event->xbutton.time;
            D(bug("ButtonPress event\n"));

            ObtainSemaphoreShared(&xsd->sema);
            if (xsd->mousehidd)
                Hidd_Mouse_X11_HandleEvent(xsd->mousehidd, event);
            ReleaseSemaphore(&xsd->sema);
            break;

        case ButtonRelease:
            xsd->x_time = event->xbutton.time;
            D(bug("ButtonRelease event\n"));

            ObtainSemaphoreShared(&xsd->sema);
            if (xsd->mousehidd)
                Hidd_Mouse_X11_HandleEvent(xsd->mousehidd, event);
            ReleaseSemaphore(&xsd->sema);
            break;

        case MotionNotify:
            xsd->x_time = event->xmotion.time;
            D(bug("Motionnotify event\n"));

            ObtainSemaphoreShared(&xsd->sema);
            if (xsd->mousehidd)
                Hidd_Mouse_X11_HandleEvent(xsd->mousehidd, event);
            ReleaseSemaphore(&xsd->sema);
            break;

        case FocusOut:
#if !BETTER_REPEAT_HANDLING
            LOCK_X11
            XCALL(XAutoRepeatOn, xsd->display);
            UNLOCK_X11
#endif
            msg = AllocMem(sizeof(struct FromX11Msg), MEMF_CLEAR);
            msg->type = FROMX11_FOCUS;
            msg->xwindow = event->xany.window;
            msg->In = FALSE;
            PutMsg(intuixchng.intuition_port,(struct Message *)msg);
            break;

        case FocusIn:
            /* Call the user supplied callback func, if supplied */
            if (NULL != xsd->activecallback)
            {
                xsd->activecallback(xsd->callbackdata, NULL);
            }
            msg = AllocMem(sizeof(struct FromX11Msg), MEMF_CLEAR);
            msg->type = FROMX11_FOCUS;
            msg->xwindow = event->xany.window;
            msg->In = TRUE;
            PutMsg(intuixchng.intuition_port,(struct Message *)msg);
            break;

        case KeyPress:
            xsd->x_time = event->xkey.time;

            LOCK_X11
#if !BETTER_REPEAT_HANDLING
            XCALL(XAutoRepeatOff, XSD(cl)->display);
#endif
            ks = XCALL(XLookupKeysym, (XKeyEvent *)event, 0);
            if (ks == XK_F12)
            {
                kri->f12_down = TRUE;
            }
            else if (kri->f12_down && ((ks == XK_Q) || (ks == XK_q)))
            {
                CCALL(raise, SIGINT);
            }
            UNLOCK_X11

            ObtainSemaphoreShared(&xsd->sema);
            if (xsd->kbdhidd)
            {
                Hidd_Kbd_X11_HandleEvent(xsd->kbdhidd, event);
            }
            ReleaseSemaphore(&xsd->sema);
            break;

        case KeyRelease:
            xsd->x_time = event->xkey.time;

#if BETTER_REPEAT_HANDLING
            kri->keyrelease_pending = TRUE;
            kri->keyrelease_event = *event;
#else
            LOCK_X11
            if (XCALL(XLookupKeysym, event, 0) == XK_F12)
            {
                kri->f12_down = FALSE;
            }
            XCALL(XAutoRepeatOn, XSD(cl)->display);
            UNLOCK_X11

            ObtainSemaphoreShared( &xsd->sema );
            if (xsd->kbdhidd)
            {
                Hidd_Kbd_X11_HandleEvent(xsd->kbdhidd, event);
            }
            ReleaseSemaphore( &xsd->sema );
#endif
            break;

        case EnterNotify:
            break;

        case LeaveNotify:
            break;

        case MapNotify:
        {

            XMapEvent *me;
            struct notify_msg *nmsg, *safe;
            struct xwinnode *node;

            me = (XMapEvent *) event;

            ForeachNodeSafe(nmsg_list, nmsg, safe)
            {
                if (me->window == nmsg->xwindow && nmsg->notify_type == NOTY_MAPWINDOW)
                {
                    /*  The window has now been mapped.
                        Send reply to app */

                    Remove((struct Node *) nmsg);
                    ReplyMsg((struct Message *) nmsg);
                }
            }

            /* Find it in the window list and mark it as mapped */

            ForeachNode(&xwindowlist, node)
            {
                if (node->xwindow == me->window)
                {
                    node->window_mapped = TRUE;
                }
            }

            break;
        }

        case ClientMessage:
            if (event->xclient.data.l[0] == xsd->delete_win_atom)
            {
                msg = AllocMem(sizeof(struct FromX11Msg), MEMF_CLEAR);
                msg->type = FROMX11_CLOSEWINDOW;
                msg->xwindow = event->xany.window;
                PutMsg(intuixchng.intuition_port,(struct Message *)msg);
            }
            break;

        } /* switch (X11 event type) */

    } /* if (is event for HIDD window) */

}

/****************************************************************************************/

static VOID x11task_keyrelease_pre(struct x11_staticdata *xsd, struct KeyReleaseInfo *kri)
{
#if BETTER_REPEAT_HANDLING
    if (kri->keyrelease_pending)
    {
        LOCK_X11
        if (XCALL(XLookupKeysym, (XKeyEvent *)&kri->keyrelease_event, 0)
                == XK_F12)
        {
            kri->f12_down = FALSE;
        }
        UNLOCK_X11

        ObtainSemaphoreShared(&xsd->sema);
        if (xsd->kbdhidd)
        {
            Hidd_Kbd_X11_HandleEvent(xsd->kbdhidd, &kri->keyrelease_event);
        }
        ReleaseSemaphore(&xsd->sema);
        kri->keyrelease_pending = FALSE;
    }
#endif
}

/****************************************************************************************/

static BOOL x11task_keyrelease_post(struct x11_staticdata *xsd, struct KeyReleaseInfo *kri, XEvent *event)
{
#if BETTER_REPEAT_HANDLING
    if (kri->keyrelease_pending)
    {
        BOOL repeated_key = FALSE;

        /* Saw this in SDL/X11 code, where a comment says that
            the idea for this was coming from GII, whatever that
            is. */

        if ((event->xany.window == kri->keyrelease_event.xany.window)
                && (event->type == KeyPress)
                && (event->xkey.keycode == kri->keyrelease_event.xkey.keycode)
                && ((event->xkey.time - kri->keyrelease_event.xkey.time) < 2))
        {
            repeated_key = TRUE;
        }

        kri->keyrelease_pending = FALSE;

        if (repeated_key)
        {
            /* Drop both previous keyrelease and this keypress event. */
            TRUE;
        }

        LOCK_X11
        if (XCALL(XLookupKeysym, (XKeyEvent *)&kri->keyrelease_event, 0)
                == XK_F12)
        {
            kri->f12_down = FALSE;
        }
        UNLOCK_X11

        ObtainSemaphoreShared(&xsd->sema);
        if (xsd->kbdhidd)
        {
            Hidd_Kbd_X11_HandleEvent(xsd->kbdhidd, &kri->keyrelease_event);
        }
        ReleaseSemaphore(&xsd->sema);
    }
#endif
    return FALSE;
}

/****************************************************************************************/

VOID x11task_entry(struct x11task_params *xtpparam)
{
    ULONG notifysig;
    struct MinList nmsg_list;
    struct MinList xwindowlist;
    ULONG hostclipboardmask;
    struct KeyReleaseInfo kri;
    kri.f12_down = FALSE;
    BOOL readxevents = TRUE;

    /* copy needed parameter's because they are allocated on the parent's stack */

    struct Task *task_Parent = xtpparam->parent;
    ULONG  task_SigKill = xtpparam->kill_signal;
    struct x11_staticdata *xsd = xtpparam->xsd;

    struct Interrupt myint;

    D(bug("[X11] %s()\n", __PRETTY_FUNCTION__));

    xsd->x11task_notify_port = CreateMsgPort();
    if (NULL == xsd->x11task_notify_port)
    {
        D(bug("[X11] %s: failed to create notification port!\n", __PRETTY_FUNCTION__));
        Signal(task_Parent, xtpparam->fail_signal);
        return;
    }

    D(bug("[X11] %s: notification port @ 0x%p\n", __PRETTY_FUNCTION__, xsd->x11task_notify_port));

    notifysig = 1L << xsd->x11task_notify_port->mp_SigBit;

    D(bug("[X11] %s: notification signal = %08x (bit %d)\n", __PRETTY_FUNCTION__, notifysig, xsd->x11task_notify_port->mp_SigBit));

    NEWLIST(&nmsg_list);
    NEWLIST(&xwindowlist);

    myint.is_Code = (VOID_FUNC) x11VBlank;
    myint.is_Data = FindTask(NULL);
    myint.is_Node.ln_Name = "X11 VBlank server";
    myint.is_Node.ln_Pri = 0;
    myint.is_Node.ln_Type = NT_INTERRUPT;

    AddIntServer(INTB_VERTB, &myint);

    Signal(task_Parent, xtpparam->ok_signal);

    /* N.B : do not attempt to use xtpparam after this point! */

    hostclipboardmask = x11clipboard_init(xsd);

    for (;;)
    {
        XEvent event;
#if BETTER_REPEAT_HANDLING
        kri.keyrelease_pending = FALSE;
#endif
        struct notify_msg *nmsg;
        ULONG sigs;

        DB2(bug("[X11] %s: waiting for signals...\n", __PRETTY_FUNCTION__));

        sigs = Wait(SIGBREAKF_CTRL_D | notifysig | task_SigKill| hostclipboardmask);

        DB2(bug("[X11] %s: signal %08x received\n", __PRETTY_FUNCTION__, sigs));

        if (sigs & task_SigKill)
        {
            D(bug("[X11] %s: kill signal received - exiting\n", __PRETTY_FUNCTION__));
            break;
        }

        if (sigs & notifysig)
        {
            D(bug("[X11] %s: notification signal received\n", __PRETTY_FUNCTION__));

            while ((nmsg = (struct notify_msg *) GetMsg(xsd->x11task_notify_port)))
            {
                /* Add the messages to an internal list */

                switch (nmsg->notify_type)
                {
                case NOTY_WINCREATE:
                    {
                        struct xwinnode * node;
                        /* Maintain a list of open windows for the X11 event handler in x11.c */

                        D(bug("[X11] %s: NOTY_WINCREATE\n", __PRETTY_FUNCTION__));

                        node = AllocMem(sizeof(struct xwinnode), MEMF_CLEAR);

                        if (NULL != node)
                        {
                            node->xwindow = nmsg->xwindow;
                            node->masterxwindow = nmsg->masterxwindow;
                            node->bmobj = nmsg->bmobj;
                            AddTail((struct List *) &xwindowlist, (struct Node *) node);
                        }
                        else
                        {
                            bug("!!!! CANNOT GET MEMORY FOR X11 WIN NODE\n");
                            CCALL(raise, 19);
                        }

                        ReplyMsg((struct Message *) nmsg);
                        break;
                    }
                case NOTY_MAPWINDOW:
                    {
                        D(bug("[X11] %s: NOTY_MAPWINDOW Window @ 0x%p (Display = 0x%p)\n", __PRETTY_FUNCTION__, nmsg->xwindow, nmsg->xdisplay));

                        LOCK_X11
                        XCALL(XMapWindow, nmsg->xdisplay, nmsg->xwindow);
                        UNLOCK_X11

                        // AddTail((struct List *) &nmsg_list, (struct Node *) nmsg);
                        ReplyMsg((struct Message *) nmsg);

                        /* Do not reply message yet */
                        break;
                    }
                case NOTY_RESIZEWINDOW:
                    {
                        XWindowChanges xwc;
                        XSizeHints sizehint;
                        BOOL replymsg = TRUE;

                        D(bug("[X11] %s: NOTY_RESIZEWINDOW\n", __PRETTY_FUNCTION__));

                        xwc.width = nmsg->width;
                        xwc.height = nmsg->height;

                        sizehint.flags = PMinSize | PMaxSize;
                        sizehint.min_width = nmsg->width;
                        sizehint.min_height = nmsg->height;
                        sizehint.max_width = nmsg->width;
                        sizehint.max_height = nmsg->height;

                        LOCK_X11
                        if (xsd->options & OPTION_FULLSCREEN)
                        {
                            x11_fullscreen_switchmode(nmsg->xdisplay, &xwc.width, &xwc.height);
                        }

                        XCALL(XSetWMNormalHints, nmsg->xdisplay, nmsg->masterxwindow, &sizehint);
                        XCALL(XConfigureWindow, nmsg->xdisplay, nmsg->masterxwindow, CWWidth | CWHeight, &xwc);
                        XCALL(XFlush, nmsg->xdisplay);
                        UNLOCK_X11

                        if (xsd->options & OPTION_DELAYXWINMAPPING)
                        {
                            struct xwinnode *node;
                            ForeachNode(&xwindowlist, node)
                            {
                                if (node->xwindow == nmsg->xwindow)
                                {
                                    if (!node->window_mapped)
                                    {
                                        LOCK_X11
                                        XCALL(XMapWindow, nmsg->xdisplay, nmsg->xwindow);
#if ADJUST_XWIN_SIZE
                                        XCALL(XMapRaised, nmsg->xdisplay, nmsg->masterxwindow);
#endif
                                        if (xsd->options & OPTION_FULLSCREEN)
                                        {
                                            XCALL(XGrabKeyboard, nmsg->xdisplay, nmsg->xwindow, False, GrabModeAsync, GrabModeAsync, CurrentTime);
                                            XCALL(XGrabPointer, nmsg->xdisplay, nmsg->xwindow, 1, PointerMotionMask | ButtonPressMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync, nmsg->xwindow, None, CurrentTime);
                                        }

                                        XCALL(XFlush, nmsg->xdisplay);
                                        UNLOCK_X11

                                        nmsg->notify_type = NOTY_MAPWINDOW;
                                        AddTail((struct List *) &nmsg_list, (struct Node *) nmsg);

                                        /* Do not reply message yet */
                                        replymsg = FALSE;

                                        break;
                                    }
                                }
                            }
                        }

                        if (replymsg)
                            ReplyMsg((struct Message *) nmsg);

                        break;
                    }
                case NOTY_WINDISPOSE:
                    {
                        struct xwinnode *node, *safe;

                        D(bug("[X11] %s: NOTY_WINDISPOSE\n", __PRETTY_FUNCTION__));

                        ForeachNodeSafe(&xwindowlist, node, safe)
                        {
                            if (node->xwindow == nmsg->xwindow)
                            {
                                Remove((struct Node *) node);
                                FreeMem(node, sizeof(struct xwinnode));
                            }
                        }
                        ReplyMsg((struct Message *) nmsg);
                        break;
                    }
                case NOTY_NEWCURSOR:
                    {
                        struct xwinnode *node;

                        D(bug("[X11] %s: NOTY_NEWCURSOR\n",
                            __PRETTY_FUNCTION__));

                        LOCK_X11
                        ForeachNode(&xwindowlist, node)
                        {
                            XCALL(XDefineCursor, nmsg->xdisplay, node->xwindow,
                                (Cursor) nmsg->xwindow);
                        }
                        XCALL(XFlush, nmsg->xdisplay);
                        UNLOCK_X11

                        ReplyMsg((struct Message *) nmsg);

                        break;
                    }
                case 5:
                    x11task_process_xevent(xsd, &xwindowlist, (XEvent *)nmsg->bmobj, &kri, &nmsg_list);
                    ReplyMsg((struct Message *) nmsg);
                    break;
                case 6: /* Intuition window manager will be reading XEvents and forwarding when needed */
                    readxevents = FALSE;
                    ReplyMsg((struct Message *) nmsg);
                    break;
                } /* switch() */
            } /* while () */
            //continue;
        } /* if (message from notify port) */

        if (sigs & hostclipboardmask)
        {
            x11clipboard_handle_commands(xsd);
        }

        if (readxevents)
        {
        for (;;)
        {
            struct xwinnode *node;
            int pending;

            LOCK_X11
            XCALL(XFlush, xsd->display);
            XCALL(XSync, xsd->display, FALSE);
            pending = XCALL(XEventsQueued, xsd->display, QueuedAlready);
            UNLOCK_X11

            if (pending == 0)
            {
                x11task_keyrelease_pre(xsd, &kri);

                /* Get out of for(;;) loop */
                break;
            }

            LOCK_X11
            XCALL(XNextEvent, xsd->display, &event);
            UNLOCK_X11

            D(bug("Got Event for X=%d\n", event.xany.window));

            if (x11task_keyrelease_post(xsd, &kri, &event))
                continue;

            if (event.type == MappingNotify)
            {
                LOCK_X11
                XCALL(XRefreshKeyboardMapping, (XMappingEvent*)&event);
                UNLOCK_X11

                continue;
            }

#if ADJUST_XWIN_SIZE
#if 0
            /* Must check this here, because below only the inner
             window events are recognized */

            if ((event.type == ClientMessage) && (event.xclient.data.l[0] == xsd->delete_win_atom))
            {
                D(bug("Shutting down AROS\n"));
                CCALL(raise, SIGINT);
            }
#endif

            /* Redirect focus from outer window to inner window. This allows
             keys to be seen without the mouse hovering over the window */
            if (event.type == FocusIn)
            {
                ForeachNode(&xwindowlist, node)
                {
                    if (node->masterxwindow == event.xfocus.window)
                    {
                        LOCK_X11
                        XCALL(XSetInputFocus, xsd->display, node->xwindow,
                            RevertToParent, CurrentTime);
                        UNLOCK_X11
                        break;
                    }
                }
            }
#endif

            x11task_process_xevent(xsd, &xwindowlist, &event, &kri, &nmsg_list);

        } /* while (events from X)  */
        } /* if (readxevents) */

    } /* Forever */

    /* Also try to free window node list ? */
    if (xsd->x11task_notify_port)
    {
        DeleteMsgPort(xsd->x11task_notify_port);
    }
}

/****************************************************************************************/

struct Task *create_x11task(struct x11task_params *params)
{
    struct Task *task;

    D(bug("[X11] %s()\n", __PRETTY_FUNCTION__));

    task = NewCreateTask(TASKTAG_PC, x11task_entry, TASKTAG_STACKSIZE,
            XTASK_STACKSIZE, TASKTAG_NAME, XTASK_NAME, TASKTAG_PRI,
            XTASK_PRIORITY, TASKTAG_ARG1, params, TAG_DONE);
    if (task)
    {
        D(bug("[X11] %s: task @ 0x%p\n", __PRETTY_FUNCTION__, task));

        /* Everything went OK. Wait for task to initialize */
        ULONG sigset;

        sigset = Wait(params->ok_signal | params->fail_signal);
        if (sigset & params->ok_signal)
        {
            D(bug("[X11] %s: got ok signal\n", __PRETTY_FUNCTION__));

            return task;
        }
    }
    return NULL;
}

/****************************************************************************************/
