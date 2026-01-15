/*
    Copyright (C) 2026, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/intuition.h>
#include <intuition/extensions.h>
#include <aros/debug.h>

#include "../include/aros/cpu.h"
#include "../include/aros/proxy.h"
#include "../include/exec/functions.h"
#include "../include/exec/proxy_structures.h"
#include "../include/intuition/structures.h"
#include "../include/intuition/proxy_structures.h"
#include "../include/graphics/proxy_structures.h"

#include "../graphics/graphics_rastports.h"
#include "intuition_gadgets.h"

extern struct ExecBaseV0 *Intuition_SysBaseV0;
extern struct IClass *gadgetwrappercl;

void syncLayerV0(struct LayerProxy *proxy);
struct TagItem *CloneTagItemsV02Native(const struct TagItemV0 *tagList);
void FreeClonedV02NativeTagItems(struct TagItem *tagList);

struct Screen *screenRemapV02N(struct ScreenV0 *v0screen);
struct ScreenV0 *screenRemapN2V0(struct Screen *nscreen);

struct WindowProxy *wmarray[100];

static void wmAdd(struct WindowProxy *proxy)
{
    for (LONG i = 0; i < 100; i++)
    {
        if (wmarray[i] == NULL)
        {
            wmarray[i] = proxy;
            return;
        }
    }
asm("int3");
}

static void wmRemove(struct WindowProxy *proxy)
{
    for (LONG i = 0; i < 100; i++)
    {
        if (wmarray[i] == proxy)
        {
            wmarray[i] = NULL;
            return;
        }
    }
asm("int3");
}

struct WindowProxy * wmGetByWindow(struct Window *native)
{
    for (LONG i = 0; i < 100; i++)
    {
        if (wmarray[i] && wmarray[i]->native == native)
        {
            return wmarray[i];
        }
    }
asm("int3");
    return NULL;
}

static void quirksSyncWindow(struct WindowProxy *proxy)
{
    /* FontTester opens a window with WA_RMBTrap but then removes the private WFLG_RMBTRAP */
    if (!(proxy->base.Flags & WFLG_RMBTRAP) && (proxy->native->Flags & WFLG_RMBTRAP))
        proxy->native->Flags &= ~WFLG_RMBTRAP;
}

void syncWindowV0(struct WindowProxy *proxy)
{
    proxy->base.BorderLeft          = proxy->native->BorderLeft;
    proxy->base.BorderTop           = proxy->native->BorderTop;
    proxy->base.BorderRight         = proxy->native->BorderRight;
    proxy->base.BorderBottom        = proxy->native->BorderBottom;
    proxy->base.Width               = proxy->native->Width;
    proxy->base.Height              = proxy->native->Height;
    proxy->base.MaxHeight           = proxy->native->MaxHeight;
    proxy->base.MinHeight           = proxy->native->MinHeight;
    proxy->base.MaxWidth            = proxy->native->MaxWidth;
    proxy->base.MinWidth            = proxy->native->MinWidth;
    proxy->base.GZZHeight           = proxy->native->GZZHeight;
    proxy->base.GZZWidth            = proxy->native->GZZWidth;
    proxy->base.Flags               = proxy->native->Flags;
    proxy->base.MouseX              = proxy->native->MouseX;
    proxy->base.MouseY              = proxy->native->MouseY;
    proxy->base.LeftEdge            = proxy->native->LeftEdge;
    proxy->base.TopEdge             = proxy->native->TopEdge;
}

static struct MessageV0 *IntuiMessage_translate(struct Message *native)
{
    struct IntuiMessage *imsg = (struct IntuiMessage *)native;

    if (native == NULL)
        return NULL;

    if (imsg->Class == IDCMP_CLOSEWINDOW || imsg->Class == IDCMP_INTUITICKS || imsg->Class == IDCMP_MOUSEMOVE ||
        imsg->Class == IDCMP_REFRESHWINDOW || imsg->Class == IDCMP_MOUSEBUTTONS || imsg->Class == IDCMP_NEWSIZE ||
        imsg->Class == IDCMP_CHANGEWINDOW || imsg->Class == IDCMP_INACTIVEWINDOW || imsg->Class == IDCMP_GADGETUP ||
        imsg->Class == IDCMP_ACTIVEWINDOW || imsg->Class == IDCMP_RAWKEY || imsg->Class == IDCMP_MENUPICK ||
        imsg->Class == IDCMP_VANILLAKEY)
    {
        struct IntuiMessageV0 *v0msg = abiv0_AllocMem(sizeof(struct IntuiMessageV0), MEMF_CLEAR, Intuition_SysBaseV0);

        v0msg->Class = imsg->Class;
        struct WindowProxy *proxy = wmGetByWindow(imsg->IDCMPWindow);
        if (proxy != NULL)
            v0msg->IDCMPWindow = (APTR32)(IPTR)proxy;
        else
            asm("int3");

        v0msg->Code         = imsg->Code;
        v0msg->Qualifier    = imsg->Qualifier;
        v0msg->MouseX       = imsg->MouseX;
        v0msg->MouseY       = imsg->MouseY;
        v0msg->Seconds      = imsg->Seconds;
        v0msg->Micros       = imsg->Micros;
        v0msg->IAddress     = (APTR32)(IPTR)NULL;

        if (imsg->Class == IDCMP_GADGETUP)
        {
            struct Gadget *nativeg = (struct Gadget *)imsg->IAddress;
            struct GadgetWrapperData *data = INST_DATA(gadgetwrappercl, nativeg);
            if (data->gwd_Key == GWD_KEY)
                v0msg->IAddress = (APTR32)(IPTR)data->gwd_Wrapped;
            else
            {
                /* Not a wrapped gadget? Then maybe iconify gadget! */
                if (nativeg->GadgetID == ETI_Iconify)
                {
                    struct GadgetV0 *gV0 = abiv0_AllocMem(sizeof(struct GadgetV0), MEMF_CLEAR, Intuition_SysBaseV0); // MEMLEAK
                    gV0->GadgetID = 0; /* ETI_Iconify; */
                    v0msg->IAddress = (APTR32)(IPTR)gV0;
                    /* Iconify is disabled for now as it would require doing a proxy for workbench.library/NotifyWorkbench */
                }
                else
                {
                    asm("int3");
                }
            }
        }

        if (imsg->Class == IDCMP_RAWKEY)
        {
            /* Probably should be a pointer to input event. This prevents crash in sequence of:
                crate local InputEvent based on IntuiMessage
                call MapRawKey
                example: HFinder  */
            v0msg->prevCodeQuals = (APTR32)(IPTR)NULL;
            v0msg->IAddress = (APTR32)(IPTR)&v0msg->prevCodeQuals;
        }

        /* Store original message in Node of v0msg for now */
        *((IPTR *)&v0msg->ExecMessage.mn_Node) = (IPTR)imsg;

        quirksSyncWindow(proxy);
        syncWindowV0(proxy);
        syncLayerV0((struct LayerProxy *)(IPTR)proxy->base.WLayer);

        return (struct MessageV0 *)v0msg;
    }

bug("Intuition_Translate - missing code for class %d\n", imsg->Class);

    return NULL;
}

struct WindowV0 *abiv0_OpenWindowTagList(struct NewWindowV0 *newWindow, struct TagItemV0 *tagList, struct LibraryV0 *IntuitionBaseV0)
{
    struct NewWindow *newWindowNative = NULL;

    if (newWindow != NULL)
    {
        newWindowNative = AllocMem(sizeof(struct NewWindow), MEMF_CLEAR);
        newWindowNative->LeftEdge   = newWindow->LeftEdge;
        newWindowNative->TopEdge    = newWindow->TopEdge;
        newWindowNative->Width      = newWindow->Width;
        newWindowNative->Height     = newWindow->Height;
        newWindowNative->Title      = (UBYTE *)(IPTR)newWindow->Title;
        newWindowNative->Flags      = newWindow->Flags;
        newWindowNative->IDCMPFlags = newWindow->IDCMPFlags;

        if (newWindow->FirstGadget != 0 || newWindow->CheckMark != 0 || newWindow->Screen != 0 || newWindow->BitMap != 0) asm("int3");
    }

    struct GadgetV0 *firstGadgetV0 = NULL;

    struct TagItem *tagListNative = CloneTagItemsV02Native(tagList);

    struct TagItem *tagNative = tagListNative;

    while (tagNative->ti_Tag != TAG_DONE)
    {
        if (tagNative->ti_Tag == WA_CustomScreen)
        {
            tagNative->ti_Data = (IPTR)screenRemapV02N((struct ScreenV0 *)tagNative->ti_Data);
        }

        if (tagNative->ti_Tag == WA_PubScreen)
        {
            tagNative->ti_Data = (IPTR)screenRemapV02N((struct ScreenV0 *)tagNative->ti_Data);
        }

        if (tagNative->ti_Tag == WA_Gadgets)
        {
            /* Gadgets cannot be added at native window creation time. See comment at end of function */
            firstGadgetV0 = (struct GadgetV0 *)tagNative->ti_Data;
            tagNative->ti_Tag = TAG_IGNORE;
        }

        tagNative++;
    }


    struct Window *wndnative = OpenWindowTagList(newWindowNative, tagListNative);

    if (newWindowNative) FreeMem(newWindowNative, sizeof(struct NewWindow));
    FreeClonedV02NativeTagItems(tagListNative);

    struct WindowProxy *proxy = abiv0_AllocMem(sizeof(struct WindowProxy), MEMF_CLEAR, Intuition_SysBaseV0);
    proxy->native = wndnative;

    proxy->base.WindowPort = 0xBAADF00D;

    proxy->base.RPort = (APTR32)(IPTR)makeRastPortV0(proxy->native->RPort);
    proxy->base.IDCMPFlags = proxy->native->IDCMPFlags; /* Synchronize after creation, not every time via syncWindowV0 */

    syncWindowV0(proxy);

    proxy->base.WScreen = (APTR32)(IPTR)screenRemapN2V0(proxy->native->WScreen);

    struct LayerProxy *lproxy = abiv0_AllocMem(sizeof(struct LayerProxy), MEMF_CLEAR, Intuition_SysBaseV0);
    lproxy->native = proxy->native->WLayer;
    syncLayerV0(lproxy);


    proxy->base.WLayer = (APTR32)(IPTR)lproxy;
    if (proxy->native->WLayer == proxy->native->RPort->Layer)
    {
        ((struct RastPortV0 *)(IPTR)proxy->base.RPort)->Layer = (APTR32)(IPTR)lproxy;
    }
    else
    {
asm("int3");
    }

    struct BitMapProxy *bmproxy = abiv0_AllocMem(sizeof(struct BitMapProxy), MEMF_CLEAR, Intuition_SysBaseV0);
    bmproxy->native = proxy->native->RPort->BitMap;
    ((struct RastPortV0 *)(IPTR)proxy->base.RPort)->BitMap = (APTR32)(IPTR)bmproxy;

    if (proxy->native->UserPort)
    {
        struct MsgPortProxy *mpproxy = abiv0_AllocMem(sizeof(struct MsgPortProxy), MEMF_CLEAR, Intuition_SysBaseV0);

        mpproxy->base.mp_SigBit = proxy->native->UserPort->mp_SigBit;
        NEWLISTV0(&mpproxy->base.mp_MsgList);
        mpproxy->base.mp_MsgList.l_pad = 1; /* MsgPortProxy */

        /* simulate port-not empty: FPC checks this before doing GetMsg */
        mpproxy->base.mp_MsgList.lh_TailPred = 0x2;

        mpproxy->native = proxy->native->UserPort;
        mpproxy->translate = IntuiMessage_translate;
        proxy->base.UserPort = (APTR32)(IPTR)mpproxy;
    }

    wmAdd(proxy);

    /* Adding gadgets means issuing GM_RENDER call and this needs translation from native to v0 window via
       wmGetByWindow and this needs registering first via wmAdd */
    if (firstGadgetV0)
    {
        LONG numGad = 0;
        struct GadgetV0 *gadget = firstGadgetV0;
        for ( ; gadget; gadget = (APTR)(IPTR)gadget->NextGadget) numGad++;
        abiv0_AddGList((struct WindowV0 *)proxy, firstGadgetV0, 0, numGad, NULL, IntuitionBaseV0);
    }

bug("abiv0_OpenWindowTagList: STUB\n");
    return (struct WindowV0 *)proxy;
}
MAKE_PROXY_ARG_3(OpenWindowTagList)

void abiv0_SetWindowTitles(struct WindowV0 *window, CONST_STRPTR windowTitle, CONST_STRPTR screenTitle, struct LibraryV0 *IntuitionBaseV0)
{
    struct WindowProxy *proxy = (struct WindowProxy *)window;

    if ((APTR32)(IPTR)windowTitle == (APTR32)-1) windowTitle = (APTR)-1;
    if ((APTR32)(IPTR)screenTitle == (APTR32)-1) screenTitle = (APTR)-1;

    SetWindowTitles(proxy->native, windowTitle, screenTitle);
}
MAKE_PROXY_ARG_4(SetWindowTitles)

void abiv0_ChangeWindowBox(struct WindowV0 *window, LONG left, LONG top, LONG width, LONG height, struct LibraryV0 *IntuitionBaseV0)
{
    struct WindowProxy *proxy = (struct WindowProxy *)window;
    ChangeWindowBox(proxy->native, left, top, width, height);
}
MAKE_PROXY_ARG_6(ChangeWindowBox)

BOOL abiv0_ModifyIDCMP(struct WindowV0 *window, ULONG flags, struct LibraryV0 *IntuitionBaseV0)
{
    struct WindowProxy *winproxy = (struct WindowProxy *)window;
    struct MsgPortProxy *msgpproxy = (struct MsgPortProxy *)(IPTR)winproxy->base.UserPort;
    if (msgpproxy != NULL)
    {
        winproxy->native->UserPort = msgpproxy->native;
        msgpproxy->translate = IntuiMessage_translate;
    }
    else
    {
        winproxy->native->UserPort = NULL;
    }

    // TODO: instead of duplicating logic on 32-bit side call original function
    winproxy->base.IDCMPFlags = flags;

    return ModifyIDCMP(winproxy->native, flags);
}
MAKE_PROXY_ARG_2(ModifyIDCMP);

void abiv0_ActivateWindow(struct WindowV0 *window, struct LibraryV0 *IntuitionBaseV0)
{
    struct WindowProxy *winproxy = (struct WindowProxy *)window;
    ActivateWindow(winproxy->native);
}
MAKE_PROXY_ARG_2(ActivateWindow);

void  abiv0_WindowToFront(struct WindowV0 *window, struct LibraryV0 *IntuitionBaseV0)
{
    struct WindowProxy *winproxy = (struct WindowProxy *)window;
    WindowToFront(winproxy->native);
}
MAKE_PROXY_ARG_2(WindowToFront);

BOOL abiv0_WindowLimits(struct WindowV0 *window, WORD MinWidth, WORD MinHeight, UWORD MaxWidth, UWORD MaxHeight, struct LibraryV0 *IntuitionBaseV0)
{
    struct WindowProxy *proxy = (struct WindowProxy *)window;
    return WindowLimits(proxy->native, MinWidth, MinHeight, MaxWidth, MaxHeight);
}
MAKE_PROXY_ARG_6(WindowLimits)

void abiv0_CloseWindow(struct Window *window, struct LibraryV0 *IntuitionBaseV0)
{
    struct WindowProxy *proxy = (struct WindowProxy *)window;
    wmRemove(proxy);
    return CloseWindow(proxy->native);
}
MAKE_PROXY_ARG_2(CloseWindow)

void abiv0_BeginRefresh(struct WindowV0 *window, struct LibraryV0 *IntuitionBaseV0)
{
    struct WindowProxy *proxy = (struct WindowProxy *)window;
    BeginRefresh(proxy->native);
}
MAKE_PROXY_ARG_2(BeginRefresh)

void abiv0_EndRefresh(struct WindowV0 *window, BOOL complete, struct LibraryV0 *IntuitionBaseV0)
{
    struct WindowProxy *proxy = (struct WindowProxy *)window;
    EndRefresh(proxy->native, complete);
}
MAKE_PROXY_ARG_3(EndRefresh)

void abiv0_RefreshWindowFrame(struct WindowV0 *window, struct LibraryV0 *IntuitionBaseV0)
{
    struct WindowProxy *proxy = (struct WindowProxy *)window;
    RefreshWindowFrame(proxy->native);
}
MAKE_PROXY_ARG_2(RefreshWindowFrame)

void Intuition_Windows_init(struct IntuitionBaseV0 *abiv0IntuitionBase, APTR32 *intuitionjmp)
{
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 101, (APTR32)(IPTR)proxy_OpenWindowTagList);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  53, (APTR32)(IPTR)proxy_WindowLimits);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  25, (APTR32)(IPTR)proxy_ModifyIDCMP);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  12, (APTR32)(IPTR)proxy_CloseWindow);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  59, (APTR32)(IPTR)proxy_BeginRefresh);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  61, (APTR32)(IPTR)proxy_EndRefresh);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  76, (APTR32)(IPTR)proxy_RefreshWindowFrame);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  46, (APTR32)(IPTR)proxy_SetWindowTitles);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  48, intuitionjmp[165 -  48]);  // SizeWindow
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  81, (APTR32)(IPTR)proxy_ChangeWindowBox);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  75, (APTR32)(IPTR)proxy_ActivateWindow);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  52, (APTR32)(IPTR)proxy_WindowToFront);
}
