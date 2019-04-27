/*
    Copyright © 2019, The AROS Development Team. All rights reserved.
    $Id$
*/

#ifndef X11_INTUI_BRIDGE_H
#define X11_INTUI_BRIDGE_H

struct intuixchng
{
    struct MsgPort  *intuition_port;
    Display         *xdisplay;
    Atom            delete_win_atom;
};

struct FromX11Msg
{
    struct Message  base;
    ULONG           type;
    Window          xwindow;
    LONG            A;
    LONG            B;
};

#define FROMX11_WINDOWPOS       (1)
#define FROMX11_CLOSEWINDOW     (2)
#define FROMX11_REFRESHWINDOW   (3)
#define FROMX11_WINDOWSIZE      (4)

#endif
