/*
    Copyright (C) 2019-2022, The AROS Development Team. All rights reserved.
*/

#ifndef X11_INTUI_BRIDGE_H
#define X11_INTUI_BRIDGE_H

struct intuixchng
{
    struct MsgPort  *intuition_port;
    Display         *xdisplay;
    Atom            delete_win_atom;
    struct MsgPort  *x11task_notify_port;
};

struct FromX11Msg
{
    struct Message  base;
    ULONG           type;
    Window          xwindow;
    union
    {
        LONG        LeftEdge;
        BOOL        In;
    };

    LONG            TopEdge;
    LONG            Width;
    LONG            Height;
};

#define FROMX11_WINDOWPOSSIZE       (1)
#define FROMX11_CLOSEWINDOW         (2)
#define FROMX11_FOCUS               (5)
#define FROMX11_WINDOWBITMAPRESIZED (6)

#endif
