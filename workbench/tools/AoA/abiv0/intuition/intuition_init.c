#include <proto/exec.h>
#include <proto/dos.h>
#include <aros/debug.h>
#include <proto/intuition.h>

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

struct ScreenProxy
{
    struct ScreenV0 base;
    struct Screen   *native;
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

struct ScreenV0 *abiv0_LockPubScreen(CONST_STRPTR name, struct LibraryV0 *IntuitionBaseV0)
{
    struct Screen *scr = LockPubScreen(name);
    if (scr == NULL)
        return NULL;

    struct ScreenProxy *ret = abiv0_AllocMem(sizeof(struct ScreenProxy), MEMF_CLEAR, Intuition_SysBaseV0);
    ret->base.Width     = scr->Width;
    ret->base.Height    = scr->Height;
    ret->native         = scr;

    struct ColorMapProxy *cmproxy = abiv0_AllocMem(sizeof(struct ColorMapProxy), MEMF_CLEAR, Intuition_SysBaseV0);
    cmproxy->native = scr->ViewPort.ColorMap;
    ret->base.ViewPort.ColorMap = (APTR32)(IPTR)cmproxy;

    struct TextAttrV0 * v0font = abiv0_AllocMem(sizeof(struct TextAttrV0), MEMF_CLEAR, Intuition_SysBaseV0);
    v0font->ta_YSize    = scr->Font->ta_YSize;
    v0font->ta_Flags    = scr->Font->ta_Flags;
    v0font->ta_Style    = scr->Font->ta_Style;
    STRPTR v0font_name = abiv0_AllocMem(strlen(scr->Font->ta_Name) + 1, MEMF_CLEAR, Intuition_SysBaseV0);
    CopyMem(scr->Font->ta_Name, v0font_name, strlen(scr->Font->ta_Name) + 1);
    v0font->ta_Name     = (APTR32)(IPTR)v0font_name;
    ret->base.Font = (APTR32)(IPTR)v0font;

bug("abiv0_LockPubScreen: STUB\n");
    return (struct ScreenV0 *)ret;
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

struct DrawInfoV0 *abiv0_GetScreenDrawInfo(struct ScreenV0 *screen, struct LibraryV0 *IntuitionBaseV0)
{
    struct ScreenProxy *proxy = (struct ScreenProxy *)screen;
    struct DrawInfo *dri = GetScreenDrawInfo(proxy->native);
    if (dri == NULL)
        return NULL;

    struct DrawInfoV0 *ret = abiv0_AllocMem(sizeof(struct DrawInfoV0), MEMF_CLEAR, Intuition_SysBaseV0);
    ret->dri_Pens = (APTR32)(IPTR)abiv0_AllocMem(NUMDRIPENS * sizeof(UWORD), MEMF_CLEAR, Intuition_SysBaseV0);
    CopyMem(dri->dri_Pens, (APTR)(IPTR)ret->dri_Pens, NUMDRIPENS * sizeof(UWORD));
    ret->dri_Font = proxy->base.Font;

bug("abiv0_GetScreenDrawInfo: STUB\n");
    return ret;
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

struct WindowV0 *g_v0window;
struct Window   *g_nativewindow;

struct WindowV0 *abiv0_OpenWindowTagList(APTR /*struct NewWindowV0 **/newWindow, struct TagItemV0 *tagList, struct LibraryV0 *IntuitionBaseV0)
{
    if (newWindow != NULL) asm("int3");

    struct TagItem *tagListNative = CloneTagItemsV02Native(tagList);
    struct Window *wndnative = OpenWindowTagList(NULL, tagListNative);

    struct WindowProxy *proxy = abiv0_AllocMem(sizeof(struct WindowProxy), MEMF_CLEAR, Intuition_SysBaseV0);
    struct RastPortV0 *rpv0 = abiv0_AllocMem(sizeof(struct RastPortV0), MEMF_CLEAR, Intuition_SysBaseV0);

    *((IPTR *)&rpv0->longreserved) = (IPTR)wndnative->RPort;
    proxy->base.RPort = (APTR32)(IPTR)rpv0;

    proxy->base.BorderLeft          = wndnative->BorderLeft;
    proxy->base.BorderTop           = wndnative->BorderTop;
    proxy->base.BorderRight         = wndnative->BorderRight;
    proxy->base.BorderBottom        = wndnative->BorderBottom;
    proxy->base.Width               = wndnative->Width;
    proxy->base.Height              = wndnative->Height;
    proxy->base.MaxHeight           = wndnative->MaxHeight;
    proxy->base.MinHeight           = wndnative->MinHeight;
    proxy->base.MaxWidth            = wndnative->MaxWidth;
    proxy->base.MinWidth            = wndnative->MinWidth;
    proxy->base.GZZHeight           = wndnative->GZZHeight;
    proxy->base.GZZWidth            = wndnative->GZZWidth;

    proxy->native = wndnative;

    g_v0window = &proxy->base;
    g_nativewindow = wndnative;

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
    return CloseWindow(proxy->native);
}
MAKE_PROXY_ARG_2(CloseWindow)

static struct MessageV0 *Intuition_Translate(struct Message *native)
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
        if (imsg->IDCMPWindow == g_nativewindow)
            v0msg->IDCMPWindow = (APTR32)(IPTR)g_v0window;

        v0msg->Code         = imsg->Code;
        v0msg->Qualifier    = imsg->Qualifier;
        v0msg->MouseX       = imsg->MouseX;
        v0msg->MouseY       = imsg->MouseY;
        v0msg->Seconds      = imsg->Seconds;
        v0msg->Micros       = imsg->Micros;

        /* Store original message in Node of v0msg for now */
        *((IPTR *)&v0msg->ExecMessage.mn_Node) = (IPTR)imsg;

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
        msgpproxy->translate = Intuition_Translate;
    }
    else
    {
        winproxy->native->UserPort = NULL;
    }

    return ModifyIDCMP(winproxy->native, flags);
}
MAKE_PROXY_ARG_2(ModifyIDCMP);

struct LibraryV0 *shallow_InitResident32(struct ResidentV0 *resident, BPTR segList, struct ExecBaseV0 *SysBaseV0);
BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);
struct ResidentV0 * findResident(BPTR seg, CONST_STRPTR name);
APTR abiv0_DOS_OpenLibrary(CONST_STRPTR name, ULONG version, struct ExecBaseV0 *SysBaseV0);

extern ULONG* segclassesinitlist;
extern ULONG *seginitlist;

void init_intuition(struct ExecBaseV0 *SysBaseV0)
{
    BPTR intuitionseg = LoadSeg32("SYS:Libs32/partial/intuition.library", DOSBase);
    struct ResidentV0 *intuitionres = findResident(intuitionseg, NULL);
    struct LibraryV0 *abiv0IntuitionBase = shallow_InitResident32(intuitionres, intuitionseg, SysBaseV0);
    Intuition_SysBaseV0 = SysBaseV0;

    /* Remove all vectors for now */
    const ULONG intuitionjmpsize = 165 * sizeof(APTR32);
    APTR32 *intuitionjmp = AllocMem(intuitionjmpsize, MEMF_CLEAR);
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
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 106, intuitionjmp[165 - 106]);  // NewObjectA
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
}
