#include <proto/exec.h>
#include <proto/dos.h>
#include <aros/debug.h>
#include <proto/intuition.h>
#include <exec/rawfmt.h>

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
    struct TextFontV0 *v0tf = abiv0_AllocMem(sizeof(struct TextFontV0), MEMF_CLEAR, Intuition_SysBaseV0);
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

    v0dri->dri_AmigaKey = (APTR32)(IPTR)makeImageV0(dri->dri_AmigaKey);
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
    while (LibNextTagItemV0 (&tmp) != NULL)
    {
        newList[pos].ti_Tag = tmp->ti_Tag;
        newList[pos].ti_Data = tmp->ti_Data;
        pos++;
    }

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
        if (wmarray[i]->native == native)
        {
            return wmarray[i];
        }
    }
asm("int3");
    return NULL;
}

struct WindowV0 *abiv0_OpenWindowTagList(APTR /*struct NewWindowV0 **/newWindow, struct TagItemV0 *tagList, struct LibraryV0 *IntuitionBaseV0)
{
    if (newWindow != NULL) asm("int3");

    struct TagItem *tagListNative = CloneTagItemsV02Native(tagList);
    struct Window *wndnative = OpenWindowTagList(NULL, tagListNative);

    struct WindowProxy *proxy = abiv0_AllocMem(sizeof(struct WindowProxy), MEMF_CLEAR, Intuition_SysBaseV0);
    struct RastPortV0 *rpv0 = abiv0_AllocMem(sizeof(struct RastPortV0), MEMF_CLEAR, Intuition_SysBaseV0);

    proxy->native = wndnative;

    *((IPTR *)&rpv0->longreserved) = (IPTR)proxy->native->RPort;
    proxy->base.RPort = (APTR32)(IPTR)rpv0;

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

    wmAdd(proxy);

bug("abiv0_OpenWindowTagList: STUB\n");
    return (struct WindowV0 *)proxy;
}
MAKE_PROXY_ARG_3(OpenWindowTagList)

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
        imsg->Class == IDCMP_CHANGEWINDOW)
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

    return ModifyIDCMP(winproxy->native, flags);
}
MAKE_PROXY_ARG_2(ModifyIDCMP);

APTR32 *intuitionjmp;

APTR abiv0_NewObjectA(struct IClass  *classPtr, UBYTE *classID, struct TagItemV0 * tagList, struct LibraryV0 *IntuitionBaseV0)
{
    /* Call original function */
    __asm__ volatile (
        "subq $16, %%rsp\n"
        "movl %4, %%eax\n"
        "movl %%eax,12(%%rsp)\n"
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
        "addq $16, %%rsp\n"
        "leave\n"
        "ret\n"
        ::"m"(intuitionjmp[165 - 106]), "m"(classPtr), "m"(classID), "m"(tagList), "m"(IntuitionBaseV0) : "%rax", "%rcx");
}
MAKE_PROXY_ARG_4(NewObjectA)

struct LibraryV0 *shallow_InitResident32(struct ResidentV0 *resident, BPTR segList, struct ExecBaseV0 *SysBaseV0);
BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);
struct ResidentV0 * findResident(BPTR seg, CONST_STRPTR name);
APTR abiv0_DOS_OpenLibrary(CONST_STRPTR name, ULONG version, struct ExecBaseV0 *SysBaseV0);

extern ULONG* segclassesinitlist;
extern ULONG *seginitlist;
extern CONST_STRPTR SYSNAME;

void init_intuition(struct ExecBaseV0 *SysBaseV0)
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

    /* Set internal Intuition pointer of utility */
    *(ULONG *)((IPTR)abiv0IntuitionBase + 0x60) = (APTR32)(IPTR)abiv0_DOS_OpenLibrary("utility.library", 0L, SysBaseV0);
    *(ULONG *)((IPTR)abiv0IntuitionBase + 0x64) = (APTR32)(IPTR)abiv0_DOS_OpenLibrary("graphics.library", 0L, SysBaseV0);
}
