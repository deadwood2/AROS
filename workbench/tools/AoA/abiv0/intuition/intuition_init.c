#include <proto/exec.h>
#include <proto/dos.h>
#include <aros/debug.h>
#include <proto/intuition.h>
#include <exec/rawfmt.h>
#include <proto/graphics.h>

#include <string.h>

#include "../include/exec/structures.h"
#include "../include/exec/proxy_structures.h"
#include "../include/exec/functions.h"
#include "../include/aros/cpu.h"
#include "../include/aros/proxy.h"
#include "../include/intuition/structures.h"
#include "../include/graphics/proxy_structures.h"
#include "../include/utility/structures.h"

struct ExecBaseV0 *Intuition_SysBaseV0;

struct IntScreenV0 // 596
{
    struct ScreenV0         Screen; // 364
    BYTE                    pad1[188];

    APTR32                  WinDecorObj;

    BYTE                    pad2[40];
};

struct ScreenProxy
{
    struct IntScreenV0  base;
    struct Screen       *native;
};

struct WindowProxy
{
    struct WindowV0 base;
    struct Window   *native;
};

struct LibraryV0 *abiv0_Intuition_OpenLib(ULONG version, struct LibraryV0 *IntuitionBaseV0)
{
    return IntuitionBaseV0;
}
MAKE_PROXY_ARG_2(Intuition_OpenLib)

struct IntScreenV0  *g_v0screen;
struct Screen       *g_nativescreen;

struct TextFontV0 *makeTextFontV0(struct TextFont *native)
{
    struct TextFontProxy *proxy = abiv0_AllocMem(sizeof(struct TextFontV0), MEMF_CLEAR, Intuition_SysBaseV0);
    struct TextFontV0 *v0tf = &proxy->base;

    v0tf->tf_YSize          = native->tf_YSize;
    v0tf->tf_Style          = native->tf_Style;
    v0tf->tf_Flags          = native->tf_Flags;
    v0tf->tf_XSize          = native->tf_XSize;
    v0tf->tf_Baseline       = native->tf_Baseline;
    v0tf->tf_BoldSmear      = native->tf_BoldSmear;
    v0tf->tf_Accessors      = native->tf_Accessors;
    v0tf->tf_LoChar         = native->tf_LoChar;
    v0tf->tf_HiChar         = native->tf_HiChar;
    v0tf->tf_CharData       = (APTR32)(IPTR)NULL;
    v0tf->tf_Modulo         = native->tf_Modulo;

    LONG arrlen = v0tf->tf_HiChar - v0tf->tf_LoChar + 1;
    APTR buff;

    buff = abiv0_AllocMem(arrlen * sizeof(LONG), MEMF_CLEAR, Intuition_SysBaseV0);
    CopyMem(native->tf_CharLoc, buff, arrlen * sizeof(LONG));
    v0tf->tf_CharLoc        = (APTR32)(IPTR)buff;

    buff = abiv0_AllocMem(arrlen * sizeof(WORD), MEMF_CLEAR, Intuition_SysBaseV0);
    CopyMem(native->tf_CharSpace, buff, arrlen * sizeof(WORD));
    v0tf->tf_CharSpace      = (APTR32)(IPTR)buff;


    buff = abiv0_AllocMem(arrlen * sizeof(WORD), MEMF_CLEAR, Intuition_SysBaseV0);
    CopyMem(native->tf_CharKern, buff, arrlen * sizeof(WORD));
    v0tf->tf_CharKern       = (APTR32)(IPTR)buff;

    proxy->native = native;
}

APTR abiv0_NewObjectA(struct IClass  *classPtr, UBYTE *classID, struct TagItemV0 * tagList, struct LibraryV0 *IntuitionBaseV0);
struct ScreenV0 *abiv0_LockPubScreen(CONST_STRPTR name, struct LibraryV0 *IntuitionBaseV0)
{
    struct Screen *native = LockPubScreen(name);
    if (native == NULL)
        return NULL;

    struct ScreenProxy *proxy = abiv0_AllocMem(sizeof(struct ScreenProxy), MEMF_CLEAR, Intuition_SysBaseV0);
    proxy->base.Screen.Width    = native->Width;
    proxy->base.Screen.Height   = native->Height;
    proxy->native               = native;

    struct ColorMapProxy *cmproxy = abiv0_AllocMem(sizeof(struct ColorMapProxy), MEMF_CLEAR, Intuition_SysBaseV0);
    cmproxy->native = native->ViewPort.ColorMap;
    proxy->base.Screen.ViewPort.ColorMap    = (APTR32)(IPTR)cmproxy;

    struct TextAttrV0 * v0font = abiv0_AllocMem(sizeof(struct TextAttrV0), MEMF_CLEAR, Intuition_SysBaseV0);
    v0font->ta_YSize    = native->Font->ta_YSize;
    v0font->ta_Flags    = native->Font->ta_Flags;
    v0font->ta_Style    = native->Font->ta_Style;
    STRPTR v0font_name = abiv0_AllocMem(strlen(native->Font->ta_Name) + 1, MEMF_CLEAR, Intuition_SysBaseV0);
    CopyMem(native->Font->ta_Name, v0font_name, strlen(native->Font->ta_Name) + 1);
    v0font->ta_Name     = (APTR32)(IPTR)v0font_name;

    proxy->base.Screen.Font     = (APTR32)(IPTR)v0font;

    *(IPTR *)(&proxy->base.Screen.LayerInfo.PrivateReserve1)    = (IPTR)&native->LayerInfo;

    proxy->base.Screen.RastPort.Font = (APTR32)(IPTR)makeTextFontV0(native->RastPort.Font);

    /* TODO: this should be a proxy to native intuition class */
    proxy->base.WinDecorObj = (APTR32)(IPTR)abiv0_NewObjectA(NULL, WINDECORCLASS, NULL, IntuitionBaseV0);

    g_nativescreen = native;
    g_v0screen = &proxy->base;

bug("abiv0_LockPubScreen: STUB\n");
    return (struct ScreenV0 *)proxy;
}
MAKE_PROXY_ARG_2(LockPubScreen)

void abiv0_UnlockPubScreen(UBYTE *name, struct ScreenV0 *screen, struct LibraryV0 *IntuitionBaseV0)
{
    struct ScreenProxy *proxy = (struct ScreenProxy *)screen;
    UnlockPubScreen(name, proxy->native);
}
MAKE_PROXY_ARG_3(UnlockPubScreen)

void abiv0_ScreenDepth(struct ScreenV0 *screen, ULONG flags, APTR reserved, struct LibraryV0 *IntuitionBaseV0)
{
bug("abiv0_ScreenDepth: STUB\n");
}
MAKE_PROXY_ARG_4(ScreenDepth)


struct ImageV0 *makeImageV0(struct Image *native)
{
    struct ImageV0 *v0img = abiv0_AllocMem(sizeof(struct ImageV0), MEMF_CLEAR, Intuition_SysBaseV0);
    v0img->LeftEdge     = native->LeftEdge;
    v0img->TopEdge      = native->TopEdge;
    v0img->Width        = native->Width;
    v0img->Height       = native->Height;
    v0img->Depth        = native->Depth;
    v0img->ImageData    = (APTR32)(IPTR)NULL;
    v0img->PlanePick    = native->PlanePick;
    v0img->PlaneOnOff   = native->PlaneOnOff;
    v0img->NextImage    = (APTR32)(IPTR)NULL;

    return v0img;
}

struct DrawInfoV0 *abiv0_GetScreenDrawInfo(struct ScreenV0 *screen, struct LibraryV0 *IntuitionBaseV0)
{
    struct ScreenProxy *proxy = (struct ScreenProxy *)screen;
    struct DrawInfo *dri = GetScreenDrawInfo(proxy->native);
    if (dri == NULL)
        return NULL;

    struct DrawInfoV0 *v0dri = abiv0_AllocMem(sizeof(struct DrawInfoV0), MEMF_CLEAR, Intuition_SysBaseV0);
    v0dri->dri_Pens = (APTR32)(IPTR)abiv0_AllocMem(NUMDRIPENS * sizeof(UWORD), MEMF_CLEAR, Intuition_SysBaseV0);
    CopyMem(dri->dri_Pens, (APTR)(IPTR)v0dri->dri_Pens, NUMDRIPENS * sizeof(UWORD));

    v0dri->dri_AmigaKey     = (APTR32)(IPTR)makeImageV0(dri->dri_AmigaKey);
    v0dri->dri_CheckMark    = (APTR32)(IPTR)makeImageV0(dri->dri_CheckMark);
    v0dri->dri_Version  = dri->dri_Version;
    v0dri->dri_Flags    = dri->dri_Flags;
    v0dri->dri_Font     = (APTR32)(IPTR)makeTextFontV0(dri->dri_Font);
    v0dri->dri_Screen   = (APTR32)(IPTR)&proxy->base;


bug("abiv0_GetScreenDrawInfo: STUB\n");
    return v0dri;
}
MAKE_PROXY_ARG_2(GetScreenDrawInfo)

void abiv0_FreeScreenDrawInfo(struct ScreenV0 *screen, struct DrawInfoV0 *drawInfo, struct LibraryV0 *IntuitionBaseV0)
{
bug("abiv0_FreeScreenDrawInfo: STUB\n");
}
MAKE_PROXY_ARG_3(FreeScreenDrawInfo)

static struct TagItemV0 *LibNextTagItemV0(struct TagItemV0 **tagListPtr)
{
    if (!(*tagListPtr))
        return NULL;

    while(1)
    {
        switch(((*tagListPtr)->ti_Tag))
        {
            case TAG_MORE:
asm("int3");
                if (!((*tagListPtr) = (struct TagItemV0 *)(IPTR)(*tagListPtr)->ti_Data))
                    return NULL;
                continue;

            case TAG_IGNORE:
                break;

            case TAG_END:
                (*tagListPtr) = 0;
                return NULL;

            case TAG_SKIP:
asm("int3");
                (*tagListPtr) += (*tagListPtr)->ti_Data + 1;
                continue;

            default:
                return (*tagListPtr)++;

        }

        (*tagListPtr)++;
    }
}

static struct TagItem *CloneTagItemsV02Native(const struct TagItemV0 *tagList)
{
    struct TagItem *newList;
    LONG numTags = 1;

    struct TagItemV0 *tmp;

    tmp = (struct TagItemV0 *)tagList;
    while (LibNextTagItemV0 (&tmp) != NULL)
        numTags++;

    newList = AllocMem(sizeof(struct TagItem) * numTags, MEMF_CLEAR);

    LONG pos = 0;
    tmp = (struct TagItemV0 *)tagList;
    do
    {
        newList[pos].ti_Tag = tmp->ti_Tag;
        newList[pos].ti_Data = tmp->ti_Data;
        pos++;
    } while (LibNextTagItemV0 (&tmp) != NULL);

    return newList;

}

static void syncLayerV0(struct LayerProxy *proxy)
{
    proxy->base.Flags               = proxy->native->Flags;
}

static void syncWindowV0(struct WindowProxy *proxy)
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
}

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

static struct WindowProxy * wmGetByWindow(struct Window *native)
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

static struct RastPortV0 *makeRastPortV0(struct RastPort *native)
{
    struct RastPortV0 *rpv0 = abiv0_AllocMem(sizeof(struct RastPortV0), MEMF_CLEAR, Intuition_SysBaseV0);
    *((IPTR *)&rpv0->longreserved) = (IPTR)native;
    return rpv0;
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

    struct TagItem *tagListNative = CloneTagItemsV02Native(tagList);

    struct TagItem *tagNative = tagListNative;

    while (tagNative->ti_Tag != TAG_DONE)
    {
        if (tagNative->ti_Tag == WA_CustomScreen)
        {
            if (tagNative->ti_Data == (IPTR)g_v0screen)
            {
                tagNative->ti_Data = (IPTR)g_nativescreen;
            }
            else asm("int3");
        }

        tagNative++;
    }


    struct Window *wndnative = OpenWindowTagList(newWindowNative, tagListNative);

    if (newWindowNative) FreeMem(newWindowNative, sizeof(struct NewWindow));

    struct WindowProxy *proxy = abiv0_AllocMem(sizeof(struct WindowProxy), MEMF_CLEAR, Intuition_SysBaseV0);
    proxy->native = wndnative;

    proxy->base.WindowPort = 0xBAADF00D;

    proxy->base.RPort = (APTR32)(IPTR)makeRastPortV0(proxy->native->RPort);

    syncWindowV0(proxy);

    if (proxy->native->WScreen == g_nativescreen)
    {
        proxy->base.WScreen = (APTR32)(IPTR)g_v0screen;
    }
    else
    {
        asm("int3");
    }


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

    wmAdd(proxy);

bug("abiv0_OpenWindowTagList: STUB\n");
    return (struct WindowV0 *)proxy;
}
MAKE_PROXY_ARG_3(OpenWindowTagList)


void abiv0_DrawImageState(struct RastPortV0 *rp, struct ImageV0 *image, LONG leftOffset, LONG topOffset, ULONG state, struct DrawInfoV0 *drawInfo)
{
    // TODO: use original code!!!!
bug("abiv0_DrawImageState: STUB\n");
}
MAKE_PROXY_ARG_6(DrawImageState)

ULONG abiv0_LockIBase(ULONG What, struct LibraryV0 *IntuitionBaseV0)
{
    return LockIBase(What);
}
MAKE_PROXY_ARG_2(LockIBase)

void abiv0_UnlockIBase(ULONG ibLock, struct LibraryV0 *IntuitionBaseV0)
{
    UnlockIBase(ibLock);
}
MAKE_PROXY_ARG_2(UnlockIBase)

BOOL abiv0_WindowLimits(struct WindowV0 *window, WORD MinWidth, WORD MinHeight, UWORD MaxWidth, UWORD MaxHeight, struct LibraryV0 *IntuitionBaseV0)
{
    struct WindowProxy *proxy = (struct WindowProxy *)window;
    return WindowLimits(proxy->native, MinWidth, MinHeight, MaxWidth, MaxHeight);
}
MAKE_PROXY_ARG_6(WindowLimits)

void abiv0_ClearMenuStrip(struct WindowV0 *window, struct LibraryV0 *IntuitionBaseV0)
{
    struct WindowProxy *proxy = (struct WindowProxy *)window;
    return ClearMenuStrip(proxy->native);
}
MAKE_PROXY_ARG_2(ClearMenuStrip)

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

BOOL abiv0_SetMenuStrip(struct WindowV0 *window, APTR32 menu, struct LibraryV0 *IntuitionBaseV0)
{
bug("abiv0_SetMenuStrip: STUB\n");
    return TRUE;
}
MAKE_PROXY_ARG_3(SetMenuStrip)

void abiv0_SetWindowPointerA(struct WindowV0 *window, struct TagItemV0 *taglist, struct LibraryV0 *IntuitionBaseV0)
{
bug("abiv0_SetWindowPointerA: STUB\n");
}
MAKE_PROXY_ARG_3(SetWindowPointerA)

BOOL abiv0_DoubleClick(ULONG sSeconds, ULONG sMicros, ULONG cSeconds, ULONG cMicros, struct LibraryV0 *IntuitionBaseV0)
{
    return DoubleClick(sSeconds, sMicros, cSeconds, cMicros);
}
MAKE_PROXY_ARG_5(DoubleClick)

static struct MessageV0 *IntuiMessage_translate(struct Message *native)
{
    struct IntuiMessage *imsg = (struct IntuiMessage *)native;

    if (native == NULL)
        return NULL;

    if (imsg->Class == IDCMP_CLOSEWINDOW || imsg->Class == IDCMP_INTUITICKS || imsg->Class == IDCMP_MOUSEMOVE ||
        imsg->Class == IDCMP_REFRESHWINDOW || imsg->Class == IDCMP_MOUSEBUTTONS || imsg->Class == IDCMP_NEWSIZE ||
        imsg->Class == IDCMP_CHANGEWINDOW || imsg->Class == IDCMP_INACTIVEWINDOW || imsg->Class == IDCMP_GADGETUP ||
        imsg->Class == IDCMP_ACTIVEWINDOW)
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

        if (imsg->Class == IDCMP_GADGETUP)
        {
            struct Gadget *nativeg = (struct Gadget *)imsg->IAddress;
            // hacky way of struct GadgetWrapperData *data = INST_DATA(CLASS, self);data->wrapped
            struct GadgetV0 *v0g = (struct GadgetV0 *)(*(IPTR *)((char *)nativeg + 0x80));
            v0msg->IAddress = (APTR32)(IPTR)v0g;
        }

        /* Store original message in Node of v0msg for now */
        *((IPTR *)&v0msg->ExecMessage.mn_Node) = (IPTR)imsg;
        syncWindowV0((struct WindowProxy *)proxy);
        syncLayerV0((struct LayerProxy *)(IPTR)proxy->base.WLayer);


        return (struct MessageV0 *)v0msg;
    }

bug("Intuition_Translate - missing code for class %d\n", imsg->Class);

    return NULL;
}

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

#include <intuition/cghooks.h>

struct RastPortV0 *abiv0_ObtainGIRPort(struct GadgetInfoV0 *gInfo, struct LibraryV0 *IntuitionBaseV0)
{
    if (gInfo && gInfo->gi_RastPort)
    {
        struct RastPortV0 *v0girp = (struct RastPortV0 *)(IPTR)gInfo->gi_RastPort;
        struct RastPort *girpnative = (struct RastPort *)*(IPTR *)&v0girp->longreserved;
        struct GadgetInfo *ginative = AllocMem(sizeof(struct GadgetInfo), MEMF_CLEAR);

        if (girpnative == NULL)
        {
bug("abiv0_ObtainGIRPort: !!NULL girpnative, creating!!\n");
            girpnative = AllocMem(sizeof(struct RastPort), MEMF_CLEAR);
            struct LayerProxy *lproxy = (struct LayerProxy *)(IPTR)v0girp->Layer;
            girpnative->Layer = lproxy->native;
            struct BitMapProxy *bmproxy = (struct BitMapProxy *)(IPTR)v0girp->BitMap;
            girpnative->BitMap = bmproxy->native;
            struct TextFontProxy *tfproxy = (struct TextFontProxy *)(IPTR)v0girp->Font;
            SetFont(girpnative, tfproxy->native);

            *(IPTR *)&v0girp->longreserved = (IPTR)girpnative;
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
    struct RastPort *rpnative = (struct RastPort *)*(IPTR *)&rp->longreserved;
    ReleaseGIRPort(rpnative);
}
MAKE_PROXY_ARG_2(ReleaseGIRPort);

BOOL abiv0_ActivateGadget(struct Gadget *gadget, struct WindowV0 *window, struct Requester *requester, struct LibraryV0 *IntuitionBaseV0)
{
bug("abiv0_ActivateGadget: STUB\n");
    return TRUE;
}
MAKE_PROXY_ARG_2(ActivateGadget);

APTR32 *intuitionjmp;

APTR abiv0_NewObjectA(struct IClass  *classPtr, UBYTE *classID, struct TagItemV0 * tagList, struct LibraryV0 *IntuitionBaseV0)
{
    APTR ret = NULL;

    /* pointerclass needs internal support, disable for now */
    if (classID && classID[0] == 'p' && classID[6] == 'r')
    {
bug("abiv0_NewObjectA: NOT creating pointerclass object");
        return ret;
    }

    /* Call original function */
    __asm__ volatile (
        "subq $16, %%rsp\n"
        "movl %5, %%eax\n"
        "movl %%eax,12(%%rsp)\n"
        "movl %4, %%eax\n"
        "movl %%eax, 8(%%rsp)\n"
        "movl %3, %%eax\n"
        "movl %%eax, 4(%%rsp)\n"
        "movl %2, %%eax\n"
        "movl %%eax, (%%rsp)\n"
        "movl %1, %%eax\n"
        ENTER32
        "call *%%eax\n"
        ENTER64
        "addq $16, %%rsp\n"
        "movl %%eax, %0\n"
        :"=m"(ret):"m"(intuitionjmp[165 - 106]), "m"(classPtr), "m"(classID), "m"(tagList), "m"(IntuitionBaseV0) : "%rax", "%rcx");

    return ret;
}
MAKE_PROXY_ARG_4(NewObjectA)

struct IClass *gadgetwrappercl;
struct GadgetWrapperData
{
    struct GadgetV0 *wrapped;
};

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
}

UWORD abiv0_AddGList(struct WindowV0 *window, struct GadgetV0 *gadget, ULONG position, LONG numGad, APTR /*struct RequesterV0 **/requester,
        struct LibraryV0 *IntuitionBaseV0)
{
    struct WindowProxy *winproxy = (struct WindowProxy *)window;
    struct Gadget *gadPrev = NULL, *gadFirst = NULL;

    for ( ; gadget && numGad; gadget = (APTR)(IPTR)gadget->NextGadget, numGad--)
    {
        struct Gadget *gwrapper = NewObjectA(gadgetwrappercl, NULL, NULL);
        struct GadgetWrapperData *data = INST_DATA(gadgetwrappercl, gwrapper);

        data->wrapped = gadget;
        syncGadgetNative(gwrapper, gadget);

        if (gadFirst == NULL) gadFirst = gwrapper;
        if (gadPrev != NULL) gadPrev->NextGadget = gwrapper;
        gadPrev = gwrapper;
    }

bug("abiv0_AddGList: STUB\n");

    return AddGList(winproxy->native, gadFirst, position, numGad, NULL);
}
MAKE_PROXY_ARG_6(AddGList)

void abiv0_RefreshGList(struct GadgetV0 *gadgets, struct WindowV0 *window, APTR /*struct Requester **/requester, LONG numGad,
    struct LibraryV0 *IntuitionBaseV0)
{
bug("abiv0_RefreshGList: STUB\n");
}
MAKE_PROXY_ARG_5(RefreshGList)

UWORD abiv0_RemoveGList(struct WindowV0 *remPtr, struct GadgetV0 *gadget, LONG numGad, struct LibraryV0 *IntuitionBaseV0)
{
bug("abiv0_RemoveGList: STUB\n");
}
MAKE_PROXY_ARG_4(RemoveGList)

struct LibraryV0 *shallow_InitResident32(struct ResidentV0 *resident, BPTR segList, struct ExecBaseV0 *SysBaseV0);
BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);
struct ResidentV0 * findResident(BPTR seg, CONST_STRPTR name);
APTR abiv0_DOS_OpenLibrary(CONST_STRPTR name, ULONG version, struct ExecBaseV0 *SysBaseV0);

extern ULONG* segclassesinitlist;
extern ULONG *seginitlist;
extern CONST_STRPTR SYSNAME;

struct _ObjectV0
{
    struct MinNodeV0  o_Node;  /* PRIVATE */
    APTR32 o_Class;
};

#define _OBJV0(obj) ((struct _ObjectV0 *)(obj))
#define _OBJECTV0(obj) (_OBJV0(obj) - 1)

#define OCLASSV0(obj) ((_OBJECTV0(obj))->o_Class)

ULONG abiv0_DoMethodA(APTR object, APTR message)
{
    struct HookV0 *clhook = (struct HookV0 *)(IPTR)OCLASSV0(object);

    __asm__ volatile (
        "pushq %%rbx\n"
        "subq $12, %%rsp\n"
        "movl %3, %%eax\n"
        "movl %%eax, 8(%%rsp)\n"
        "movl %2, %%eax\n"
        "movl %%eax, 4(%%rsp)\n"
        "movl %1, %%eax\n"
        "movl %%eax, (%%rsp)\n"
        "movl %0, %%eax\n"
        ENTER32
        "call *%%eax\n"
        ENTER64
        "addq $12, %%rsp\n"
        "popq %%rbx\n"
        "leave\n"
        "ret\n"
        ::"m"(clhook->h_Entry), "m"(clhook), "m"(object), "m"(message) : "%rax", "%rcx");
}


static struct GadgetInfoV0 *composeGadgetInfoV0Int(struct GadgetInfo *nativegi, BOOL nowin)
{
    struct GadgetInfoV0 *v0gi = abiv0_AllocMem(sizeof(struct GadgetInfoV0), MEMF_CLEAR, Intuition_SysBaseV0);

    v0gi->gi_Domain         = nativegi->gi_Domain;
    if (!nowin && nativegi->gi_Window)
        v0gi->gi_Window     = (APTR32)(IPTR)wmGetByWindow(nativegi->gi_Window);
    if (nativegi->gi_RastPort)
        v0gi->gi_RastPort   = (APTR32)(IPTR)makeRastPortV0(nativegi->gi_RastPort);
    if (nativegi->gi_Layer)
    {
        struct LayerProxy *lproxy = abiv0_AllocMem(sizeof(struct LayerProxy), MEMF_CLEAR, Intuition_SysBaseV0);
        lproxy->native = nativegi->gi_Layer;
        syncLayerV0(lproxy);
        v0gi->gi_Layer      = (APTR32)(IPTR)lproxy;
    }

    if (nativegi->gi_Screen && nativegi->gi_Screen == g_nativescreen)
    {
        v0gi->gi_Screen     = (APTR32)(IPTR)g_v0screen;
    }
    else if (nativegi->gi_Screen != NULL) asm("int3");

    return v0gi;
}

static struct GadgetInfoV0 *composeGadgetInfoV0(struct GadgetInfo *nativegi)
{
    return composeGadgetInfoV0Int(nativegi, FALSE);
}

void freeComposedGadgetInfoV0(struct GadgetInfoV0 *v0gi)
{
    if (v0gi->gi_RastPort)
        abiv0_FreeMem((APTR)(IPTR)v0gi->gi_RastPort, sizeof(struct RastPortV0), Intuition_SysBaseV0);
    if (v0gi->gi_Layer)
        abiv0_FreeMem((APTR)(IPTR)v0gi->gi_Layer, sizeof(struct LayerProxy), Intuition_SysBaseV0);

    abiv0_FreeMem(v0gi, sizeof(struct GadgetInfoV0), Intuition_SysBaseV0);
}

/*
 * Messages are processed on 31bit stack. This is needed for case where 64-bit Intuition input handler issues a call that is
 * the forwarded to 32-bit gadget method. This method needs to be executed on 31-bit stack, not on original stack.
 */
static IPTR process_message_on_31bit_stack(struct IClass *CLASS, Object *self, Msg message)
{
    struct GadgetWrapperData *data = INST_DATA(CLASS, self);
    struct Gadget *nativeg = (struct Gadget *)self;

    switch (message->MethodID)
    {
        case OM_NEW: return DoSuperMethodA(CLASS, self, message);
        case GM_HITTEST:
        {
            struct gpHitTest *nativemsg = (struct gpHitTest *)message;
            struct GadgetV0 *v0g = data->wrapped;

            struct gpHitTestV0 *v0msg = abiv0_AllocMem(sizeof(struct gpHitTestV0), MEMF_CLEAR, Intuition_SysBaseV0);
            struct GadgetInfoV0 * v0gi = composeGadgetInfoV0Int(nativemsg->gpht_GInfo, TRUE); /* workaround for trash gi_Window */

            v0msg->MethodID     = nativemsg->MethodID;
            v0msg->gpht_GInfo   = (APTR32)(IPTR)v0gi;
            v0msg->gpht_Mouse.X = nativemsg->gpht_Mouse.X;
            v0msg->gpht_Mouse.Y = nativemsg->gpht_Mouse.Y;

            IPTR ret = (IPTR)abiv0_DoMethodA(data->wrapped, v0msg);

            freeComposedGadgetInfoV0(v0gi);
            abiv0_FreeMem(v0msg, sizeof(struct gpHitTestV0), Intuition_SysBaseV0);

            return ret;
        }
        case GM_GOACTIVE:
        {
            struct gpInput *nativemsg = (struct gpInput *)message;
            struct GadgetV0 *v0g = data->wrapped;

            struct gpInputV0 *v0msg = abiv0_AllocMem(sizeof(struct gpInputV0), MEMF_CLEAR, Intuition_SysBaseV0);
            struct GadgetInfoV0 *v0gi = composeGadgetInfoV0(nativemsg->gpi_GInfo);

            v0msg->MethodID     = nativemsg->MethodID;
            v0msg->gpi_GInfo    = (APTR32)(IPTR)v0gi;
            v0msg->gpi_Mouse.X  = nativemsg->gpi_Mouse.X;
            v0msg->gpi_Mouse.Y  = nativemsg->gpi_Mouse.Y;

            IPTR ret = (IPTR)abiv0_DoMethodA(data->wrapped, v0msg);

            syncGadgetNative(nativeg, v0g);

            freeComposedGadgetInfoV0(v0gi);
            abiv0_FreeMem(v0msg, sizeof(struct gpInputV0), Intuition_SysBaseV0);

            return ret;

        }

        case GM_HANDLEINPUT:
        {
            struct gpInput *nativemsg = (struct gpInput *)message;
            struct GadgetV0 *v0g = data->wrapped;
            LONG gpi_Termination = *nativemsg->gpi_Termination;

            struct gpInputV0 *v0msg = abiv0_AllocMem(sizeof(struct gpInputV0), MEMF_CLEAR, Intuition_SysBaseV0);
            struct GadgetInfoV0 *v0gi = composeGadgetInfoV0(nativemsg->gpi_GInfo);
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

            IPTR ret = (IPTR)abiv0_DoMethodA(data->wrapped, v0msg);

            syncGadgetNative(nativeg, v0g);

            *nativemsg->gpi_Termination = gpi_Termination;

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

            IPTR ret = (IPTR)abiv0_DoMethodA(data->wrapped, v0msg);

            abiv0_FreeMem(v0msg, sizeof(struct gpGoInactiveV0), Intuition_SysBaseV0);

            return ret;
        }
        case GM_LAYOUT:
        {
            struct gpLayout *nativemsg = (struct gpLayout *)message;
            struct GadgetV0 *v0g = data->wrapped;

            struct gpLayoutV0 *v0msg = abiv0_AllocMem(sizeof(struct gpLayoutV0), MEMF_CLEAR, Intuition_SysBaseV0);
            struct GadgetInfoV0 *v0gi = composeGadgetInfoV0(nativemsg->gpl_GInfo);

            v0msg->MethodID     = nativemsg->MethodID;
            v0msg->gpl_GInfo    = (APTR32)(IPTR)v0gi;
            v0msg->gpl_Initial  = nativemsg->gpl_Initial;

            IPTR ret = (IPTR)abiv0_DoMethodA(data->wrapped, v0msg);

            syncGadgetNative(nativeg, v0g);

            freeComposedGadgetInfoV0(v0gi);
            abiv0_FreeMem(v0msg, sizeof(struct gpLayoutV0), Intuition_SysBaseV0);

            return ret;
        }
        case GM_RENDER:
        {
            struct gpRender *nativemsg = (struct gpRender *)message;
            struct GadgetV0 *v0g = data->wrapped;

            struct gpRenderV0 *v0msg = abiv0_AllocMem(sizeof(struct gpRenderV0), MEMF_CLEAR, Intuition_SysBaseV0);
            struct GadgetInfoV0 *v0gi = composeGadgetInfoV0(nativemsg->gpr_GInfo);

            struct DrawInfoV0 *v0dri = abiv0_AllocMem(sizeof(struct DrawInfoV0), MEMF_CLEAR, Intuition_SysBaseV0);
            v0dri->dri_Pens = (APTR32)(IPTR)abiv0_AllocMem(NUMDRIPENS * sizeof(UWORD), MEMF_CLEAR, Intuition_SysBaseV0);
            CopyMem(nativemsg->gpr_GInfo->gi_DrInfo->dri_Pens, (APTR)(IPTR)v0dri->dri_Pens, NUMDRIPENS * sizeof(UWORD));

            v0gi->gi_DrInfo     = (APTR32)(IPTR)v0dri;

            v0msg->MethodID     = nativemsg->MethodID;
            v0msg->gpr_Redraw   = nativemsg->gpr_Redraw;
            v0msg->gpr_GInfo    = (APTR32)(IPTR)v0gi;
            v0msg->gpr_RPort    = (APTR32)(IPTR)makeRastPortV0(nativemsg->gpr_RPort);

            IPTR ret = (IPTR)abiv0_DoMethodA(data->wrapped, v0msg);

            abiv0_FreeMem((APTR)(IPTR)v0msg->gpr_RPort, sizeof(struct RastPortV0), Intuition_SysBaseV0);
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

static void init_gadget_wrapper_class()
{
    gadgetwrappercl = MakeClass(NULL, GADGETCLASS, NULL, sizeof(struct GadgetWrapperData), 0);
    gadgetwrappercl->cl_Dispatcher.h_Entry = (APTR)GadgetWrapperClass_Dispatcher;
    gadgetwrappercl->cl_Dispatcher.h_SubEntry = NULL;
}

void init_intuition(struct ExecBaseV0 *SysBaseV0, struct LibraryV0 *timerBase)
{
    TEXT path[64];
    NewRawDoFmt("%s:Libs32/partial/intuition.library", RAWFMTFUNC_STRING, path, SYSNAME);
    BPTR intuitionseg = LoadSeg32(path, DOSBase);
    struct ResidentV0 *intuitionres = findResident(intuitionseg, NULL);
    struct LibraryV0 *abiv0IntuitionBase = shallow_InitResident32(intuitionres, intuitionseg, SysBaseV0);
    Intuition_SysBaseV0 = SysBaseV0;


    /* Remove all vectors for now */
    const ULONG intuitionjmpsize = 165 * sizeof(APTR32);
    intuitionjmp = AllocMem(intuitionjmpsize, MEMF_CLEAR);
    CopyMem((APTR)abiv0IntuitionBase - intuitionjmpsize, intuitionjmp, intuitionjmpsize);
    for (int i = 1; i <= 164; i++) __AROS_SETVECADDRV0(abiv0IntuitionBase, i, 0);

    /* Call SysBase_autoinit */
    __asm__ volatile (
        "subq $4, %%rsp\n"
        "movl %0, %%eax\n"
        "movl %%eax, (%%rsp)\n"
        "movl %1, %%eax\n"
        ENTER32
        "call *%%eax\n"
        ENTER64
        "addq $4, %%rsp\n"
        ::"m"(Intuition_SysBaseV0), "m"(seginitlist[1]) : "%rax", "%rcx");

    __AROS_SETVECADDRV0(abiv0IntuitionBase,   1, (APTR32)(IPTR)proxy_Intuition_OpenLib);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 113, intuitionjmp[165 - 113]);  // MakeClass
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 112, intuitionjmp[165 - 112]);  // FindClass
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 114, intuitionjmp[165 - 114]);  // AddClass
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 106, (APTR32)(IPTR)proxy_NewObjectA);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 108, intuitionjmp[165 - 108]);  // SetAttrs
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 109, intuitionjmp[165 - 109]);  // GetAttr
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 111, intuitionjmp[165 - 111]);  // NextObject
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  85, (APTR32)(IPTR)proxy_LockPubScreen);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  42, intuitionjmp[165 -  42]);  // ScreenToFront
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 131, (APTR32)(IPTR)proxy_ScreenDepth);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 115, (APTR32)(IPTR)proxy_GetScreenDrawInfo);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 107, intuitionjmp[165 - 107]);  // DisposeObject
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 116, (APTR32)(IPTR)proxy_FreeScreenDrawInfo);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 101, (APTR32)(IPTR)proxy_OpenWindowTagList);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  53, (APTR32)(IPTR)proxy_WindowLimits);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  25, (APTR32)(IPTR)proxy_ModifyIDCMP);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,   9, (APTR32)(IPTR)proxy_ClearMenuStrip);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  12, (APTR32)(IPTR)proxy_CloseWindow);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  86, (APTR32)(IPTR)proxy_UnlockPubScreen);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 119, intuitionjmp[165 - 119]);  // FreeClass
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 118, intuitionjmp[165 - 118]);  // RemoveClass
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  59, (APTR32)(IPTR)proxy_BeginRefresh);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  61, (APTR32)(IPTR)proxy_EndRefresh);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  76, (APTR32)(IPTR)proxy_RefreshWindowFrame);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  44, (APTR32)(IPTR)proxy_SetMenuStrip);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 136, (APTR32)(IPTR)proxy_SetWindowPointerA);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  17, (APTR32)(IPTR)proxy_DoubleClick);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 110, intuitionjmp[165 - 110]);  // SetGadgetAttrs
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  93, (APTR32)(IPTR)proxy_ObtainGIRPort);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 145, intuitionjmp[165 - 145]);  // DoNotify
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  77, (APTR32)(IPTR)proxy_ActivateGadget);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  73, (APTR32)(IPTR)proxy_AddGList);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 135, intuitionjmp[165 - 135]);  // DoGadgetMethodA
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  72, (APTR32)(IPTR)proxy_RefreshGList);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  69, (APTR32)(IPTR)proxy_LockIBase);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  70, (APTR32)(IPTR)proxy_UnlockIBase);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  94, (APTR32)(IPTR)proxy_ReleaseGIRPort);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 103, (APTR32)(IPTR)proxy_DrawImageState);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  74, (APTR32)(IPTR)proxy_RemoveGList);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 146, intuitionjmp[165 - 146]);  // FreeICData
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  41, intuitionjmp[165 -  41]);  // ScreenToBack
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  14, intuitionjmp[165 -  14]);  // CurrentTime

    /* Call CLASSESINIT_LIST */
    ULONG pos = 1;
    APTR32 func = segclassesinitlist[pos];
    while (func != 0)
    {
        __asm__ volatile (
            "subq $4, %%rsp\n"
            "movl %0, %%eax\n"
            "movl %%eax, (%%rsp)\n"
            "movl %1, %%eax\n"
            ENTER32
            "call *%%eax\n"
            ENTER64
            "addq $4, %%rsp\n"
            ::"m"(abiv0IntuitionBase), "m"(func) : "%rax", "%rcx");
        pos++;
        func = segclassesinitlist[pos];
    }

    /* Set internal Intuition pointer of utility, graphics and timer */
    *(ULONG *)((IPTR)abiv0IntuitionBase + 0x60) = (APTR32)(IPTR)abiv0_DOS_OpenLibrary("utility.library", 0L, SysBaseV0);
    *(ULONG *)((IPTR)abiv0IntuitionBase + 0x64) = (APTR32)(IPTR)abiv0_DOS_OpenLibrary("graphics.library", 0L, SysBaseV0);
    *(ULONG *)((IPTR)abiv0IntuitionBase + 0x74) = (APTR32)(IPTR)timerBase;

    init_gadget_wrapper_class();
}
