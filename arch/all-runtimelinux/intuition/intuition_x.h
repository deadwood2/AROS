#ifndef _INTUITION_X_H_
#define _INTUITION_X_H_

/*
    Copyright (C) 2022, The AROS Development Team. All rights reserved.
*/

#ifndef EXEC_TYPES_H
#   include <exec/types.h>
#endif
#ifndef INTUITION_INTUITION_H
#   include <intuition/intuition.h>
#endif

#define WFLG_BORDERLESSNOTREALLY (1L<<19)

VOID HandleFromX11(struct IntuitionBase *IntuitionBase);
struct Layer *WhichLayer_X11(struct Layer_Info *li, LONG x, LONG y, struct IntuitionBase *IntuitionBase);

VOID SendClientMessageClose(struct Window *win, struct IntuitionBase *IntuitionBase);
VOID SendClientMessageActive(struct Window *win, struct IntuitionBase *IntuitionBase);
VOID SendClientMessageRestack(struct Window *win, WORD topbottom, struct IntuitionBase *IntuitionBase);
VOID SendClientMessageMove(struct Window *win, WORD new_left, WORD new_top, struct IntuitionBase *IntuitionBase);
VOID SendClientMessageResize(struct Window *win, WORD new_width, WORD new_height, struct IntuitionBase *IntuitionBase);

VOID OpenXWindow(struct Window *w, struct BitMap **windowBitMap, struct Layer_Info **layerInfo,
    struct IntuitionBase *IntuitionBase, struct GfxBase *GfxBase, struct LayersBase * LayersBase);
VOID AdjustFlagsForWM(struct NewWindow *nw, struct IntuitionBase *IntuitionBase);
VOID GetXScreenDimensions(WORD *width, WORD *height, struct IntuitionBase *IntuitionBase);

#endif /* _INTUITION_X_H_ */

