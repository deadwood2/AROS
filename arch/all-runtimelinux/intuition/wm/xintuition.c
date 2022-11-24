/*
    Copyright (C) 2022, The AROS Development Team. All rights reserved.
*/

#include <proto/intuition.h>

#include "xintuition.h"

#include "../../../../rom/intuition/intuition_intern.h"
#include "../../../../arch/all-runtime/hidd/x11/x11_intui_bridge.h"

Window OpenBorderWindow(int x, int y, int width, int height, const char *title)
{
    struct Window *w = OpenWindowTags(NULL,
        WA_Left, x,
        WA_Top, y,
        WA_Width, width,
        WA_Height, height,
        WA_DragBar, TRUE,
        WA_CloseGadget, TRUE,
        WA_SizeGadget, TRUE,
        WA_DepthGadget, TRUE,
        WA_Title, title);

    return IW(w)->XWindow;
}

void StartupXIntuition()
{
    LockPubScreen(NULL);
}

Display *GetIntuitionDisplay()
{
    struct intuixchng *intuixchng = ((struct intuixchng *)GetPrivIBase(IntuitionBase)->intuixchng);
    return intuixchng->xdisplay;
}
