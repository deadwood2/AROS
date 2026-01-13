/*
    Copyright (C) 2026, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/alib.h>
#include <intuition/gadgetclass.h>
#include <intuition/cghooks.h>

#include <aros/debug.h>

#include "../include/exec/functions.h"
#include "../include/aros/cpu.h"
#include "../include/aros/proxy.h"
#include "../include/intuition/structures.h"
#include "../include/intuition/proxy_structures.h"
#include "../include/graphics/proxy_structures.h"
#include "../include/input/structures.h"

#include "../graphics/graphics_rastports.h"
#include "intuition_gadgets.h"
#include "intuition_windows.h"

extern struct ExecBaseV0 *Intuition_SysBaseV0;
extern struct ScreenV0     *g_mainv0screen;
extern struct Screen       *g_mainnativescreen;

struct IClass *gadgetwrappercl;

void syncLayerV0(struct LayerProxy *proxy);
struct TextFontV0 *makeTextFontV0(struct TextFont *native, struct ExecBaseV0 *sysBaseV0);
ULONG abiv0_DoMethodA(APTR object, APTR message);

/* Assumptions:
    1) All nativeg will be ExtGadget as created by 64-bit Intuition
    2) All v0g will be ExtGadgetV0 as created by 32-bit Intuition
    3) v0g contains valid data that needs to be synchronized back to nativeg
*/


static void syncGadgetNative(struct Gadget *nativeg, struct GadgetV0 *v0g)
{
    nativeg->Flags         = v0g->Flags;
    nativeg->Activation    = v0g->Activation;
    nativeg->TopEdge       = v0g->TopEdge;
    nativeg->LeftEdge      = v0g->LeftEdge;
    nativeg->Width         = v0g->Width;
    nativeg->Height        = v0g->Height;
    nativeg->GadgetID      = v0g->GadgetID;
    nativeg->GadgetType    = v0g->GadgetType;

    if (v0g->Flags & GFLG_EXTENDED)
    {
        ((struct ExtGadget *)nativeg)->MoreFlags        = v0g->MoreFlags;
        ((struct ExtGadget *)nativeg)->BoundsLeftEdge   = v0g->BoundsLeftEdge;
        ((struct ExtGadget *)nativeg)->BoundsTopEdge    = v0g->BoundsTopEdge;
        ((struct ExtGadget *)nativeg)->BoundsWidth      = v0g->BoundsWidth;
        ((struct ExtGadget *)nativeg)->BoundsHeight     = v0g->BoundsHeight;
    }
}

UWORD abiv0_AddGList(struct WindowV0 *window, struct GadgetV0 *gadget, ULONG position, LONG numGad, APTR /*struct RequesterV0 **/requester,
        struct LibraryV0 *IntuitionBaseV0)
{
    struct WindowProxy *winproxy = (struct WindowProxy *)window;
    struct Gadget *gadPrev = NULL, *gadFirst = NULL;
    LONG i = numGad;

    for ( ; gadget && i; gadget = (APTR)(IPTR)gadget->NextGadget, i--)
    {
        struct Gadget *gwrapper = NewObjectA(gadgetwrappercl, NULL, NULL);
        struct GadgetWrapperData *data = INST_DATA(gadgetwrappercl, gwrapper);

        data->gwd_Key       = GWD_KEY;
        data->gwd_Wrapped   = gadget;
        syncGadgetNative(gwrapper, gadget);

        if (gadFirst == NULL) gadFirst = gwrapper;
        if (gadPrev != NULL) gadPrev->NextGadget = gwrapper;
        gadPrev = gwrapper;
    }

bug("abiv0_AddGList: STUB\n");

    return AddGList(winproxy->native, gadFirst, position, numGad, NULL);
}
MAKE_PROXY_ARG_6(AddGList)

UWORD abiv0_AddGadget(struct WindowV0 * window, struct GadgetV0 *gadget, ULONG position, struct LibraryV0 *IntuitionBaseV0)
{
bug("abiv0_AddGadget: STUB\n");
return 0;
}
MAKE_PROXY_ARG_4(AddGadget)

void abiv0_RefreshGList(struct GadgetV0 *gadgets, struct WindowV0 *window, APTR /*struct Requester **/requester, LONG numGad,
    struct LibraryV0 *IntuitionBaseV0)
{
    struct WindowProxy *winproxy = (struct WindowProxy *)window;
    RefreshGList(winproxy->native->FirstGadget, winproxy->native, NULL, -1);
bug("abiv0_RefreshGList: STUB\n");
}
MAKE_PROXY_ARG_5(RefreshGList)

UWORD abiv0_RemoveGList(struct WindowV0 *remPtr, struct GadgetV0 *gadget, LONG numGad, struct LibraryV0 *IntuitionBaseV0)
{
bug("abiv0_RemoveGList: STUB\n");
}
MAKE_PROXY_ARG_4(RemoveGList)

struct RastPortV0 *abiv0_ObtainGIRPort(struct GadgetInfoV0 *gInfo, struct LibraryV0 *IntuitionBaseV0)
{
    if (gInfo && gInfo->gi_RastPort)
    {
        struct RastPortV0 *v0girp = (struct RastPortV0 *)(IPTR)gInfo->gi_RastPort;
        struct RastPort *girpnative = RastPortV0_getnative(v0girp);
        struct GadgetInfo *ginative = AllocMem(sizeof(struct GadgetInfo), MEMF_CLEAR);

        if (girpnative->BitMap == NULL)
        {
bug("abiv0_ObtainGIRPort: !!NULL girpnative->BitMap, assigning!!\n");
            struct LayerProxy *lproxy = (struct LayerProxy *)(IPTR)v0girp->Layer;
            girpnative->Layer = lproxy->native;
            struct BitMapProxy *bmproxy = (struct BitMapProxy *)(IPTR)v0girp->BitMap;
            girpnative->BitMap = bmproxy->native;
            struct TextFontProxy *tfproxy = (struct TextFontProxy *)(IPTR)v0girp->Font;
            SetFont(girpnative, tfproxy->native);
        }

        ginative->gi_RastPort = girpnative;

        struct RastPort *rpnative = ObtainGIRPort(ginative);
        FreeMem(ginative, sizeof(struct GadgetInfo));

        return makeRastPortV0(rpnative);
    }
    else
    {
        return NULL;
    }
}
MAKE_PROXY_ARG_2(ObtainGIRPort);

void abiv0_ReleaseGIRPort(struct RastPortV0 *rp, struct LibraryV0 *IntuitionBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    ReleaseGIRPort(rpnative);
    freeRastPortV0(rp);
}
MAKE_PROXY_ARG_2(ReleaseGIRPort);

BOOL abiv0_ActivateGadget(struct GadgetV0 *gadget, struct WindowV0 *window, struct Requester *requester, struct LibraryV0 *IntuitionBaseV0)
{
    if (!window || gadget->GadgetID == 0)
    {
bug("abiv0_ActivateGadget: STUB\n");
        return FALSE;
    }

    struct WindowProxy *winproxy = (struct WindowProxy *)window;
    struct Gadget *g = winproxy->native->FirstGadget;
    while (g)
    {
        if (g->GadgetID == gadget->GadgetID)
            break;
        g = g->NextGadget;
    }

    if (g)
        return ActivateGadget(g, winproxy->native, NULL);
    else
    {
bug("abiv0_ActivateGadget: STUB\n");
return FALSE;
    }
}
MAKE_PROXY_ARG_4(ActivateGadget);

void abiv0_NewModifyProp(struct GadgetV0 *gadget, struct WindowV0 *window, struct Requester *requester, ULONG flags, ULONG horizPot,
    ULONG vertPot, ULONG horizBody, ULONG vertBody, LONG numGad, struct LibraryV0 *IntuitionBaseV0)
{
bug("abiv0_NewModifyProp: STUB\n");
}
MAKE_PROXY_ARG_12(NewModifyProp);


struct TextFont *g_nativeTF;
struct TextFontV0 *g_v0TF;

static APTR32 getOrCreateTextFontV0(struct TextFont *native, struct ExecBaseV0 *sysBaseV0)
{
    if (g_nativeTF != NULL)
    {
        if (g_nativeTF == native)
            return (APTR32)(IPTR)g_v0TF;
        else asm("int3");
    }

    g_v0TF = makeTextFontV0(native, sysBaseV0); // MEMLEAK
    g_nativeTF = native;

    return (APTR32)(IPTR)g_v0TF;
}

static struct GadgetInfoV0 *composeGadgetInfoV0Int(struct GadgetInfo *nativegi, BOOL nowin)
{
    struct GadgetInfoV0 *v0gi = abiv0_AllocMem(sizeof(struct GadgetInfoV0), MEMF_CLEAR, Intuition_SysBaseV0);

    v0gi->gi_Domain         = nativegi->gi_Domain;
    if (!nowin && nativegi->gi_Window)
    {
        struct WindowProxy *wproxy = wmGetByWindow(nativegi->gi_Window);
        v0gi->gi_Window     = (APTR32)(IPTR)wproxy;
        syncWindowV0(wproxy);
    }
    if (nativegi->gi_RastPort)
        v0gi->gi_RastPort   = (APTR32)(IPTR)makeRastPortV0(nativegi->gi_RastPort);
    if (nativegi->gi_Layer)
    {
        struct LayerProxy *lproxy = abiv0_AllocMem(sizeof(struct LayerProxy), MEMF_CLEAR, Intuition_SysBaseV0);
        lproxy->native = nativegi->gi_Layer;
        syncLayerV0(lproxy);
        v0gi->gi_Layer      = (APTR32)(IPTR)lproxy;
    }

    if (nativegi->gi_Screen && nativegi->gi_Screen == g_mainnativescreen)
    {
        v0gi->gi_Screen     = (APTR32)(IPTR)g_mainv0screen;
    }
    else if (nativegi->gi_Screen != NULL) asm("int3");

    return v0gi;
}

static struct GadgetInfoV0 *composeGadgetInfoV0(struct GadgetInfo *nativegi)
{
    return composeGadgetInfoV0Int(nativegi, FALSE);
}

static void freeComposedGadgetInfoV0(struct GadgetInfoV0 *v0gi)
{
    if (v0gi->gi_RastPort)
        freeRastPortV0((struct RastPortV0 *)(IPTR)v0gi->gi_RastPort);
    if (v0gi->gi_Layer)
        abiv0_FreeMem((APTR)(IPTR)v0gi->gi_Layer, sizeof(struct LayerProxy), Intuition_SysBaseV0);

    abiv0_FreeMem(v0gi, sizeof(struct GadgetInfoV0), Intuition_SysBaseV0);
}

static void syncDummyGadgetV0(struct GadgetV0 *v0g, struct Gadget *nativeg)
{
    v0g->Flags          = nativeg->Flags;
    v0g->Activation     = nativeg->Activation;
    v0g->TopEdge        = nativeg->TopEdge;
    v0g->LeftEdge       = nativeg->LeftEdge;
    v0g->Width          = nativeg->Width;
    v0g->Height         = nativeg->Height;
    v0g->GadgetID       = nativeg->GadgetID;
    v0g->GadgetType     = nativeg->GadgetType;

    if (nativeg->Flags & GFLG_EXTENDED)
    {
        v0g->MoreFlags      = ((struct ExtGadget *)nativeg)->MoreFlags;
        v0g->BoundsLeftEdge = ((struct ExtGadget *)nativeg)->BoundsLeftEdge;
        v0g->BoundsTopEdge  = ((struct ExtGadget *)nativeg)->BoundsTopEdge;
        v0g->BoundsWidth    = ((struct ExtGadget *)nativeg)->BoundsWidth;
        v0g->BoundsHeight   = ((struct ExtGadget *)nativeg)->BoundsHeight;
    }
}

static struct GadgetV0 * createDummyGadgetV0Chain(struct GadgetInfo *nativegi)
{
    struct GadgetV0 *_ret = NULL, *v0prev = NULL;
    struct Gadget *g = nativegi->gi_Window->FirstGadget;
    while (g)
    {
        struct GadgetV0 *v0g = abiv0_AllocMem(sizeof(struct GadgetV0), MEMF_CLEAR, Intuition_SysBaseV0);
        syncDummyGadgetV0(v0g, g);
        if (v0prev) v0prev->NextGadget = (APTR32)(IPTR)v0g;
        v0prev = v0g;

        if (_ret == NULL) _ret = v0g;
        g = g->NextGadget;
    }

    return _ret;
}

static void quirks_GM_RENDER_Apply(struct GadgetV0 *v0g, struct GadgetInfoV0 *v0gi, struct GadgetInfo *nativegi)
{
    /* AslEraser class needs gadgets in Window->FirstGadget */
    if (v0g->LeftEdge == 20000 && v0g->Width == 1)
    {
        ((struct WindowV0 *)(IPTR)v0gi->gi_Window)->FirstGadget = (APTR32)(IPTR)createDummyGadgetV0Chain(nativegi);
    }
}

static void quirks_GM_RENDER_Remove(struct GadgetV0 *v0g, struct GadgetInfoV0 *v0gi)
{
    /* AslEraser class needs gadgets in Window->FirstGadget */
    if (v0g->LeftEdge == 20000 && v0g->Width == 1)
    {
        struct GadgetV0 *p = (struct GadgetV0 *)(IPTR)((struct WindowV0 *)(IPTR)v0gi->gi_Window)->FirstGadget;

        while (p)
        {
            struct GadgetV0 *t = p;
            p = (struct GadgetV0 *)(IPTR)p->NextGadget;
            abiv0_FreeMem(t, sizeof(struct GadgetV0), Intuition_SysBaseV0);
        }

        ((struct WindowV0 *)(IPTR)v0gi->gi_Window)->FirstGadget = (APTR32)(IPTR)NULL;
    }
}


/*
 * Messages are processed on 31bit stack. This is needed for case where 64-bit Intuition input handler issues a call that is
 * then forwarded to 32-bit gadget method. This method needs to be executed on 31-bit stack, not on original stack.
 */
static IPTR process_message_on_31bit_stack(struct IClass *CLASS, Object *self, Msg message)
{
    struct GadgetWrapperData *data = INST_DATA(CLASS, self);
    struct Gadget *nativeg = (struct Gadget *)self;

    switch (message->MethodID)
    {
        case OM_NEW: return DoSuperMethodA(CLASS, self, message);
        case OM_GET:
        {
            struct opGet *nativemsg = (struct opGet *)message;
            ULONG storage;

            struct opGetV0 *v0msg = abiv0_AllocMem(sizeof(struct opGetV0), MEMF_CLEAR, Intuition_SysBaseV0);
            v0msg->MethodID     = nativemsg->MethodID;
            v0msg->opg_AttrID   = nativemsg->opg_AttrID;
            v0msg->opg_Storage  = (APTR32)(IPTR)&storage;

            IPTR ret = (IPTR)abiv0_DoMethodA(data->gwd_Wrapped, v0msg);

            *nativemsg->opg_Storage = (IPTR)storage;

            abiv0_FreeMem(v0msg, sizeof(struct opGetV0), Intuition_SysBaseV0);

            return ret;
        }
        case GM_HITTEST:
        {
            struct gpHitTest *nativemsg = (struct gpHitTest *)message;
            struct GadgetV0 *v0g = data->gwd_Wrapped;

            struct gpHitTestV0 *v0msg = abiv0_AllocMem(sizeof(struct gpHitTestV0), MEMF_CLEAR, Intuition_SysBaseV0);
            struct GadgetInfoV0 * v0gi = composeGadgetInfoV0Int(nativemsg->gpht_GInfo, TRUE); /* workaround for trash gi_Window */

            v0msg->MethodID     = nativemsg->MethodID;
            v0msg->gpht_GInfo   = (APTR32)(IPTR)v0gi;
            v0msg->gpht_Mouse.X = nativemsg->gpht_Mouse.X;
            v0msg->gpht_Mouse.Y = nativemsg->gpht_Mouse.Y;

            IPTR ret = (IPTR)abiv0_DoMethodA(data->gwd_Wrapped, v0msg);

            freeComposedGadgetInfoV0(v0gi);
            abiv0_FreeMem(v0msg, sizeof(struct gpHitTestV0), Intuition_SysBaseV0);

            return ret;
        }
        case GM_GOACTIVE:
        {
            struct gpInput *nativemsg = (struct gpInput *)message;
            struct GadgetV0 *v0g = data->gwd_Wrapped;
            struct InputEventV0 *v0ie = NULL;
            LONG gpi_Termination = *nativemsg->gpi_Termination;

            struct gpInputV0 *v0msg = abiv0_AllocMem(sizeof(struct gpInputV0), MEMF_CLEAR, Intuition_SysBaseV0);
            struct GadgetInfoV0 *v0gi = composeGadgetInfoV0(nativemsg->gpi_GInfo);

            struct DrawInfoV0 *v0dri = abiv0_AllocMem(sizeof(struct DrawInfoV0), MEMF_CLEAR, Intuition_SysBaseV0);
            v0dri->dri_Pens = (APTR32)(IPTR)abiv0_AllocMem(NUMDRIPENS * sizeof(UWORD), MEMF_CLEAR, Intuition_SysBaseV0);
            CopyMem(nativemsg->gpi_GInfo->gi_DrInfo->dri_Pens, (APTR)(IPTR)v0dri->dri_Pens, NUMDRIPENS * sizeof(UWORD));
            v0dri->dri_Font = (APTR32)(IPTR)getOrCreateTextFontV0(nativemsg->gpi_GInfo->gi_DrInfo->dri_Font, Intuition_SysBaseV0);

            v0gi->gi_DrInfo     = (APTR32)(IPTR)v0dri;

            if (nativemsg->gpi_IEvent)
            {
                v0ie = abiv0_AllocMem(sizeof(struct InputEventV0), MEMF_CLEAR, Intuition_SysBaseV0);
                v0ie->ie_Class      = nativemsg->gpi_IEvent->ie_Class;
                v0ie->ie_SubClass   = nativemsg->gpi_IEvent->ie_SubClass;
                v0ie->ie_Code       = nativemsg->gpi_IEvent->ie_Code;
                v0ie->ie_Qualifier  = nativemsg->gpi_IEvent->ie_Qualifier;
                v0ie->ie_TimeStamp  = nativemsg->gpi_IEvent->ie_TimeStamp;
            }

            v0msg->MethodID     = nativemsg->MethodID;
            v0msg->gpi_GInfo    = (APTR32)(IPTR)v0gi;
            v0msg->gpi_IEvent   = (APTR32)(IPTR)v0ie;
            v0msg->gpi_Mouse.X  = nativemsg->gpi_Mouse.X;
            v0msg->gpi_Mouse.Y  = nativemsg->gpi_Mouse.Y;
            v0msg->gpi_Termination = (APTR32)(IPTR)&gpi_Termination;

            IPTR ret = (IPTR)abiv0_DoMethodA(data->gwd_Wrapped, v0msg);

            syncGadgetNative(nativeg, v0g);

            *nativemsg->gpi_Termination = gpi_Termination;

            abiv0_FreeMem((APTR)(IPTR)v0dri->dri_Pens, NUMDRIPENS * sizeof(UWORD), Intuition_SysBaseV0);
            abiv0_FreeMem(v0dri, sizeof(struct DrawInfoV0), Intuition_SysBaseV0);
            if (v0ie)
                abiv0_FreeMem(v0ie, sizeof(struct InputEventV0), Intuition_SysBaseV0);
            freeComposedGadgetInfoV0(v0gi);
            abiv0_FreeMem(v0msg, sizeof(struct gpInputV0), Intuition_SysBaseV0);

            return ret;

        }

        case GM_HANDLEINPUT:
        {
            struct gpInput *nativemsg = (struct gpInput *)message;
            struct GadgetV0 *v0g = data->gwd_Wrapped;
            LONG gpi_Termination = *nativemsg->gpi_Termination;

            struct gpInputV0 *v0msg = abiv0_AllocMem(sizeof(struct gpInputV0), MEMF_CLEAR, Intuition_SysBaseV0);
            struct GadgetInfoV0 *v0gi = composeGadgetInfoV0(nativemsg->gpi_GInfo);

            struct DrawInfoV0 *v0dri = abiv0_AllocMem(sizeof(struct DrawInfoV0), MEMF_CLEAR, Intuition_SysBaseV0);
            v0dri->dri_Pens = (APTR32)(IPTR)abiv0_AllocMem(NUMDRIPENS * sizeof(UWORD), MEMF_CLEAR, Intuition_SysBaseV0);
            CopyMem(nativemsg->gpi_GInfo->gi_DrInfo->dri_Pens, (APTR)(IPTR)v0dri->dri_Pens, NUMDRIPENS * sizeof(UWORD));
            v0dri->dri_Font = (APTR32)(IPTR)getOrCreateTextFontV0(nativemsg->gpi_GInfo->gi_DrInfo->dri_Font, Intuition_SysBaseV0);

            v0gi->gi_DrInfo     = (APTR32)(IPTR)v0dri;

            struct InputEventV0 *v0ie = abiv0_AllocMem(sizeof(struct InputEventV0), MEMF_CLEAR, Intuition_SysBaseV0);
            v0ie->ie_Class      = nativemsg->gpi_IEvent->ie_Class;
            v0ie->ie_SubClass   = nativemsg->gpi_IEvent->ie_SubClass;
            v0ie->ie_Code       = nativemsg->gpi_IEvent->ie_Code;
            v0ie->ie_Qualifier  = nativemsg->gpi_IEvent->ie_Qualifier;
            v0ie->ie_TimeStamp  = nativemsg->gpi_IEvent->ie_TimeStamp;

            v0msg->MethodID     = nativemsg->MethodID;
            v0msg->gpi_GInfo    = (APTR32)(IPTR)v0gi;
            v0msg->gpi_IEvent   = (APTR32)(IPTR)v0ie;
            v0msg->gpi_Mouse.X  = nativemsg->gpi_Mouse.X;
            v0msg->gpi_Mouse.Y  = nativemsg->gpi_Mouse.Y;
            v0msg->gpi_Termination = (APTR32)(IPTR)&gpi_Termination;

            IPTR ret = (IPTR)abiv0_DoMethodA(data->gwd_Wrapped, v0msg);

            syncGadgetNative(nativeg, v0g);

            *nativemsg->gpi_Termination = gpi_Termination;

            abiv0_FreeMem((APTR)(IPTR)v0dri->dri_Pens, NUMDRIPENS * sizeof(UWORD), Intuition_SysBaseV0);
            abiv0_FreeMem(v0dri, sizeof(struct DrawInfoV0), Intuition_SysBaseV0);
            abiv0_FreeMem(v0ie, sizeof(struct InputEventV0), Intuition_SysBaseV0);
            freeComposedGadgetInfoV0(v0gi);
            abiv0_FreeMem(v0msg, sizeof(struct gpInputV0), Intuition_SysBaseV0);

            return ret;

        }
        case GM_GOINACTIVE:
        {
            struct gpGoInactive *nativemsg = (struct gpGoInactive *)message;

            struct gpGoInactiveV0 *v0msg = abiv0_AllocMem(sizeof(struct gpGoInactiveV0), MEMF_CLEAR, Intuition_SysBaseV0);

            v0msg->MethodID     = nativemsg->MethodID;
            v0msg->gpgi_Abort   = nativemsg->gpgi_Abort;

            IPTR ret = (IPTR)abiv0_DoMethodA(data->gwd_Wrapped, v0msg);

            abiv0_FreeMem(v0msg, sizeof(struct gpGoInactiveV0), Intuition_SysBaseV0);

            return ret;
        }
        case GM_LAYOUT:
        {
            struct gpLayout *nativemsg = (struct gpLayout *)message;
            struct GadgetV0 *v0g = data->gwd_Wrapped;

            struct gpLayoutV0 *v0msg = abiv0_AllocMem(sizeof(struct gpLayoutV0), MEMF_CLEAR, Intuition_SysBaseV0);
            struct GadgetInfoV0 *v0gi = composeGadgetInfoV0(nativemsg->gpl_GInfo);

            v0msg->MethodID     = nativemsg->MethodID;
            v0msg->gpl_GInfo    = (APTR32)(IPTR)v0gi;
            v0msg->gpl_Initial  = nativemsg->gpl_Initial;

            IPTR ret = (IPTR)abiv0_DoMethodA(data->gwd_Wrapped, v0msg);

            syncGadgetNative(nativeg, v0g);

            freeComposedGadgetInfoV0(v0gi);
            abiv0_FreeMem(v0msg, sizeof(struct gpLayoutV0), Intuition_SysBaseV0);

            return ret;
        }
        case GM_RENDER:
        {
            struct gpRender *nativemsg = (struct gpRender *)message;
            struct GadgetV0 *v0g = data->gwd_Wrapped;

            struct gpRenderV0 *v0msg = abiv0_AllocMem(sizeof(struct gpRenderV0), MEMF_CLEAR, Intuition_SysBaseV0);
            struct GadgetInfoV0 *v0gi = composeGadgetInfoV0(nativemsg->gpr_GInfo);
            quirks_GM_RENDER_Apply(v0g, v0gi, nativemsg->gpr_GInfo);

            struct DrawInfoV0 *v0dri = abiv0_AllocMem(sizeof(struct DrawInfoV0), MEMF_CLEAR, Intuition_SysBaseV0);
            v0dri->dri_Pens = (APTR32)(IPTR)abiv0_AllocMem(NUMDRIPENS * sizeof(UWORD), MEMF_CLEAR, Intuition_SysBaseV0);
            CopyMem(nativemsg->gpr_GInfo->gi_DrInfo->dri_Pens, (APTR)(IPTR)v0dri->dri_Pens, NUMDRIPENS * sizeof(UWORD));
            v0dri->dri_Font = (APTR32)(IPTR)getOrCreateTextFontV0(nativemsg->gpr_GInfo->gi_DrInfo->dri_Font, Intuition_SysBaseV0);

            v0gi->gi_DrInfo     = (APTR32)(IPTR)v0dri;

            v0msg->MethodID     = nativemsg->MethodID;
            v0msg->gpr_Redraw   = nativemsg->gpr_Redraw;
            v0msg->gpr_GInfo    = (APTR32)(IPTR)v0gi;
            v0msg->gpr_RPort    = (APTR32)(IPTR)makeRastPortV0(nativemsg->gpr_RPort);

            IPTR ret = (IPTR)abiv0_DoMethodA(data->gwd_Wrapped, v0msg);

            quirks_GM_RENDER_Remove(v0g, v0gi);
            freeRastPortV0((struct RastPortV0 *)(IPTR)v0msg->gpr_RPort);
            abiv0_FreeMem((APTR)(IPTR)v0dri->dri_Pens, NUMDRIPENS * sizeof(UWORD), Intuition_SysBaseV0);
            abiv0_FreeMem(v0dri, sizeof(struct DrawInfoV0), Intuition_SysBaseV0);
            freeComposedGadgetInfoV0(v0gi);
            abiv0_FreeMem(v0msg, sizeof(struct gpRenderV0), Intuition_SysBaseV0);

            return ret;
        }
        default:
asm("int3");
            return DoSuperMethodA(CLASS, self, message);
    }

    return (IPTR) NULL;
}

BOOPSI_DISPATCHER(IPTR, GadgetWrapperClass_Dispatcher, CLASS, self, message)
{
    static APTR stack31bit = NULL;
    struct StackSwapStruct sss;
    struct StackSwapArgs ssa;

    if (stack31bit == NULL) stack31bit = AllocMem(64 * 1024, MEMF_CLEAR | MEMF_31BIT);

    sss.stk_Lower = stack31bit;
    sss.stk_Upper = sss.stk_Lower + 64 * 1024;
    sss.stk_Pointer = sss.stk_Upper;

    ssa.Args[0] = (IPTR)CLASS;
    ssa.Args[1] = (IPTR)self;
    ssa.Args[2] = (IPTR)message;

    return NewStackSwap(&sss, process_message_on_31bit_stack, &ssa);
}
BOOPSI_DISPATCHER_END

void Intuition_Gadgets_init(struct IntuitionBaseV0 *abiv0IntuitionBase, APTR32 *intuitionjmp)
{
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  73, (APTR32)(IPTR)proxy_AddGList);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  72, (APTR32)(IPTR)proxy_RefreshGList);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  74, (APTR32)(IPTR)proxy_RemoveGList);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,   7, (APTR32)(IPTR)proxy_AddGadget);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 110, intuitionjmp[165 - 110]);  // SetGadgetAttrs
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  93, (APTR32)(IPTR)proxy_ObtainGIRPort);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  77, (APTR32)(IPTR)proxy_ActivateGadget);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 135, intuitionjmp[165 - 135]);  // DoGadgetMethodA
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  94, (APTR32)(IPTR)proxy_ReleaseGIRPort);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  38, intuitionjmp[165 -  38]);  // RemoveGadget
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  78, (APTR32)(IPTR)proxy_NewModifyProp);
}

void Intuition_Gadgets_init_GadgetWrapper_class()
{
    gadgetwrappercl = MakeClass(NULL, GADGETCLASS, NULL, sizeof(struct GadgetWrapperData), 0);
    gadgetwrappercl->cl_Dispatcher.h_Entry = (APTR)GadgetWrapperClass_Dispatcher;
    gadgetwrappercl->cl_Dispatcher.h_SubEntry = NULL;
}
