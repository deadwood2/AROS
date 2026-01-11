/*
    Copyright (C) 2025-2026, The AROS Development Team. All rights reserved.
*/

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
#include "../include/intuition/proxy_structures.h"
#include "../include/graphics/proxy_structures.h"
#include "../include/utility/structures.h"
#include "../include/input/structures.h"

#include "../graphics/graphics_rastports.h"
#include "../exec/exec_libraries.h"

#include "intuition_gadgets.h"
#include "intuition_windows.h"

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

struct LibraryV0 *abiv0_Intuition_OpenLib(ULONG version, struct LibraryV0 *IntuitionBaseV0)
{
    return IntuitionBaseV0;
}
MAKE_PROXY_ARG_2(Intuition_OpenLib)

BPTR abiv0_Intuition_CloseLib(struct LibraryV0 *IntuitionBaseV0)
{
    return BNULL;
}
MAKE_PROXY_ARG_1(Intuition_CloseLib)

BPTR abiv0_Intuition_ExpungeLib(struct LibraryV0 *extralhV0, struct LibraryV0 *IntuitionBaseV0)
{
    return BNULL;
}
MAKE_PROXY_ARG_2(Intuition_ExpungeLib)

struct IntScreenV0  *g_mainv0screen;
struct Screen       *g_mainnativescreen;

struct IntScreenV0  *g_additionalv0screen;
struct Screen       *g_additionalnativescreen;
TEXT                g_additionalscreenname[64];

extern struct TextFontV0 *makeTextFontV0(struct TextFont *native, struct ExecBaseV0 *sysBaseV0);

struct ScreenV0 *abiv0_LockPubScreen(CONST_STRPTR name, struct LibraryV0 *IntuitionBaseV0)
{
    if (name != NULL && strcmp(name, "Workbench") != 0)
    {
        // non-Workbench screen requsted
        if (g_additionalscreenname[0] == '\0' ) ; // none yet opened, ok
        else asm("int3");
    }
    else
    {
         // Workbench screen requested, ok
    }

    struct Screen *native = LockPubScreen(name);

    if (native == NULL)
        return NULL;

bug("abiv0_LockPubScreen: STUB\n");

    if (native == g_mainnativescreen)
        return (struct ScreenV0 *)g_mainv0screen;
    else
        asm("int3"); // TODO: implement when additional screen found

    return NULL;
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
    v0dri->dri_Font     = (APTR32)(IPTR)makeTextFontV0(dri->dri_Font, Intuition_SysBaseV0);
    v0dri->dri_Screen   = (APTR32)(IPTR)&proxy->base;

    //MEMLEAK

bug("abiv0_GetScreenDrawInfo: STUB\n");
    return v0dri;
}
MAKE_PROXY_ARG_2(GetScreenDrawInfo)

void abiv0_FreeScreenDrawInfo(struct ScreenV0 *screen, struct DrawInfoV0 *drawInfo, struct LibraryV0 *IntuitionBaseV0)
{
bug("abiv0_FreeScreenDrawInfo: STUB\n");
}
MAKE_PROXY_ARG_3(FreeScreenDrawInfo)

struct TagItemV0 *LibNextTagItemV0(struct TagItemV0 **tagListPtr)
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

struct TagItem *CloneTagItemsV02Native(const struct TagItemV0 *tagList)
{
    struct TagItem *newList;
    LONG numTags = 1;

    struct TagItemV0 *tmp;

    tmp = (struct TagItemV0 *)tagList;
    while (LibNextTagItemV0 (&tmp) != NULL)
        numTags++;

    newList = AllocVec(sizeof(struct TagItem) * numTags, MEMF_CLEAR);

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

void FreeClonedV02NativeTagItems(struct TagItem *tagList)
{
    FreeVec(tagList);
}

/* TODO: should use proxy structures like window and layer */
void syncScreenV0()
{
    g_mainv0screen->Screen.MouseX   = g_mainnativescreen->MouseX;
    g_mainv0screen->Screen.MouseY   = g_mainnativescreen->MouseY;

    // TODO: same for additional
}

struct NewScreenV0
{

};

struct ScreenV0 * abiv0_OpenScreenTagList(struct NewScreenV0 *newScreen, struct TagItemV0 *tagList, struct LibraryV0 *IntuitionBaseV0)
{
bug("abiv0_OpenScreenTagList: STUB\n");
    return NULL;
}
MAKE_PROXY_ARG_3(OpenScreenTagList)

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

static void freeMenuItemTree(struct MenuItem *menuitem)
{
    while (menuitem != NULL)
    {
        freeMenuItemTree(menuitem->SubItem);
        if (menuitem->Flags & ITEMTEXT)
        {
            struct IntuiText *itext = (struct IntuiText *)menuitem->ItemFill;
            while (itext != NULL)
            {
                struct IntuiText *p = itext;
                itext = itext->NextText;
                FreeMem(p, sizeof(struct IntuiText));
            }
        }
        else
        {
            asm("int3");
        }

        struct MenuItem *p = menuitem;
        menuitem = menuitem->NextItem;
        FreeMem(p, sizeof(struct MenuItem));
    }
}

void abiv0_ClearMenuStrip(struct WindowV0 *window, struct LibraryV0 *IntuitionBaseV0)
{
    struct WindowProxy *proxy = (struct WindowProxy *)window;
    struct Menu *menu = proxy->native->MenuStrip;
    ClearMenuStrip(proxy->native);

    while (menu != NULL)
    {
        freeMenuItemTree(menu->FirstItem);
        struct Menu *p = menu;
        menu = menu->NextMenu;
        FreeMem(p, sizeof(struct Menu));
    }
}
MAKE_PROXY_ARG_2(ClearMenuStrip)

static struct IntuiText * makeIntuiText(struct IntuiTextV0 *itext)
{
    if (itext->ITextFont != (APTR32)0) asm("int3");
    struct IntuiText *itextnative = AllocMem(sizeof(struct IntuiText), MEMF_ANY);
    itextnative->FrontPen   = itext->FrontPen;
    itextnative->BackPen    = itext->BackPen;
    itextnative->DrawMode   = itext->DrawMode;
    itextnative->LeftEdge   = itext->LeftEdge;
    itextnative->TopEdge    = itext->TopEdge;
    itextnative->ITextFont  = NULL;
    itextnative->IText      = (UBYTE *)(IPTR)itext->IText;
    itextnative->NextText   = NULL;
    if (itext->NextText != (APTR32)(IPTR)NULL)
        itextnative->NextText = makeIntuiText((struct IntuiTextV0 *)(IPTR)itext->NextText);
    return itextnative;
}

static struct MenuItem * makeMenuItemTree(struct MenuItemV0 *menuitem)
{
    struct MenuItem *menuitemnative = NULL;
    struct MenuItem *prevmanuitemnative = NULL;
    while (menuitem != NULL)
    {
        struct MenuItem *pi = AllocMem(sizeof(struct MenuItem), MEMF_ANY);
        if (menuitemnative == NULL) menuitemnative = pi;
        if (prevmanuitemnative != NULL) prevmanuitemnative->NextItem = pi;
        pi->NextItem    = NULL;
        pi->LeftEdge    = menuitem->LeftEdge;
        pi->TopEdge     = menuitem->TopEdge;
        pi->Width       = menuitem->Width;
        pi->Height      = menuitem->Height;
        pi->Flags       = menuitem->Flags;
        pi->MutualExclude = menuitem->MutualExclude;
        pi->ItemFill    = NULL;
        if (menuitem->Flags & ITEMTEXT)
        {
            struct IntuiTextV0 *itext = (struct IntuiTextV0 *)(IPTR)menuitem->ItemFill;
            pi->ItemFill = makeIntuiText(itext);
        }
        else
        {
            struct IntuiText *pfake = AllocMem(sizeof (struct IntuiText), MEMF_ANY | MEMF_CLEAR);
            pfake->IText = "------";
            pi->ItemFill = pfake;
            pi->Flags |= ITEMTEXT;
bug("abiv0_SetMenuStrip: STUB\n");
        }
        pi->SelectFill  = NULL;
        if (menuitem->SelectFill != (APTR32)0) asm("int3");
        pi->Command     = menuitem->Command;
        pi->SubItem     = makeMenuItemTree((struct MenuItemV0 *)(IPTR)menuitem->SubItem);
        pi->NextSelect  = menuitem->NextSelect;

        prevmanuitemnative = pi;
        menuitem = (struct MenuItemV0 *)(IPTR)menuitem->NextItem;
    }

    return menuitemnative;
}

BOOL abiv0_SetMenuStrip(struct WindowV0 *window, struct MenuV0 *menu, struct LibraryV0 *IntuitionBaseV0)
{
    struct WindowProxy *proxy = (struct WindowProxy *)window;

    struct Menu *menunative = NULL;
    struct Menu *prevmenunative = NULL;

    while (menu != NULL)
    {
        struct Menu *p = AllocMem(sizeof(struct Menu), MEMF_ANY);
        if (menunative == NULL) menunative = p;
        if (prevmenunative != NULL) prevmenunative->NextMenu = p;
        p->NextMenu = NULL;
        p->LeftEdge = menu->LeftEdge;
        p->TopEdge  = menu->TopEdge;
        p->Width    = menu->Width;
        p->Height   = menu->Height;
        p->Flags    = menu->Flags;
        p->MenuName = (BYTE *)(IPTR)menu->MenuName;
        p->FirstItem = NULL;
        p->JazzX    = menu->JazzX;
        p->JazzY    = menu->JazzY;
        p->BeatX    = menu->BeatX;
        p->BeatY    = menu->BeatY;

        struct MenuItemV0 *menuitem = (struct MenuItemV0 *)(IPTR)menu->FirstItem;
        p->FirstItem = makeMenuItemTree(menuitem);

        prevmenunative = p;
        menu = (struct MenuV0 *)(IPTR)menu->NextMenu;
    }

    return SetMenuStrip(proxy->native, menunative);
}
MAKE_PROXY_ARG_3(SetMenuStrip)

void abiv0_SetWindowPointerA(struct WindowV0 *window, struct TagItemV0 *taglist, struct LibraryV0 *IntuitionBaseV0)
{
bug("abiv0_SetWindowPointerA: STUB\n");
}
MAKE_PROXY_ARG_3(SetWindowPointerA)

void abiv0_ClearPointer(struct WindowV0 *window, struct LibraryV0 *IntuitionBaseV0)
{
bug("abiv0_ClearPointer: STUB\n");
}
MAKE_PROXY_ARG_2(ClearPointer)

BOOL abiv0_DoubleClick(ULONG sSeconds, ULONG sMicros, ULONG cSeconds, ULONG cMicros, struct LibraryV0 *IntuitionBaseV0)
{
    return DoubleClick(sSeconds, sMicros, cSeconds, cMicros);
}
MAKE_PROXY_ARG_5(DoubleClick)

APTR32 *intuitionjmp;

struct IClassV0
{
    struct HookV0  cl_Dispatcher;
    ULONG          cl_Reserved;
    APTR32         cl_Super;         /* Super-Class */
    APTR32         cl_ID;
    UWORD          cl_InstOffset;
    UWORD          cl_InstSize;
    APTR32         cl_UserData;      /* application specific */
    ULONG          cl_SubclassCount; /* # of direct subclasses */
    ULONG          cl_ObjectCount;   /* # of objects, made from this class
                                        must be 0, if the class is to be
                                        deleted */
    ULONG          cl_Flags;         /* see below */
    ULONG          cl_ObjectSize;    /* cl_InstOffset + cl_InstSize + sizeof(struct _Object) */
    APTR32         cl_MemoryPool;
};

struct _ObjectV0
{
    struct MinNodeV0  o_Node;  /* PRIVATE */
    APTR32 o_Class;
};

struct _struct_MsgV0
{
    STACKED ULONG MethodID;
};

#define _OBJV0(obj) ((struct _ObjectV0 *)(obj))
#define _OBJECTV0(obj) (_OBJV0(obj) - 1)

#define OCLASSV0(obj) ((_OBJECTV0(obj))->o_Class)

static APTR call_orig_NewObjectA(struct IClassV0  *classPtr, UBYTE *classID, struct TagItemV0 * tagList, struct LibraryV0 *IntuitionBaseV0)
{
    APTR ret = NULL;
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
        :"=m"(ret):"m"(intuitionjmp[165 - 106]), "m"(classPtr), "m"(classID), "m"(tagList), "m"(IntuitionBaseV0)
        : SCRATCH_REGS_64_TO_32 );
}

struct IClassV0 *pointerclass;
BOOL pointerclassinited = FALSE;

static ULONG abiv0_pointerclass_dispatcher(struct IClassV0* cl, struct _ObjectV0 *self, struct _struct_MsgV0* msg)
{
    switch(msg->MethodID)
    {
        case(OM_NEW):
        {
            struct _ObjectV0 *obj = (struct _ObjectV0 *)abiv0_AllocMem(sizeof(struct _ObjectV0) + 12, MEMF_CLEAR, Intuition_SysBaseV0);
            obj->o_Class = (APTR32)(IPTR)cl;
            return (APTR32)(IPTR)(obj + 1);
        }
        default:
bug("abiv0_pointerclass_dispatcher: STUB\n");
    }
    return 0;
}
MAKE_PROXY_ARG_3(pointerclass_dispatcher)

APTR abiv0_NewObjectA(struct IClassV0  *classPtr, UBYTE *classID, struct TagItemV0 * tagList, struct LibraryV0 *IntuitionBaseV0)
{
    /* pointerclass needs internal support, return fake object for now */
    if (classID && classID[0] == 'p' && classID[6] == 'r')
    {
        if (!pointerclassinited)
        {
            pointerclass = (struct IClassV0 *)abiv0_AllocMem(sizeof(struct IClassV0), MEMF_CLEAR, Intuition_SysBaseV0);
            pointerclass->cl_Dispatcher.h_Entry = (APTR32)(IPTR)proxy_pointerclass_dispatcher;
            pointerclass->cl_Dispatcher.h_SubEntry = (APTR32)(IPTR)NULL;
            pointerclassinited = TRUE;
        }
bug("abiv0_NewObjectA: returning fake pointerclass object\n");
        return call_orig_NewObjectA(pointerclass, NULL, tagList, IntuitionBaseV0);;
    }

    return call_orig_NewObjectA(classPtr, classID, tagList, IntuitionBaseV0);
}
MAKE_PROXY_ARG_4(NewObjectA)

struct ListV0 *abiv0_LockPubScreenList(struct LibraryV0 *IntuitionBaseV0)
{
bug("abiv0_LockPubScreenList: STUB\n");
    return (struct ListV0 *)((IPTR)IntuitionBaseV0 + 0x12C); // PubScreenList
}
MAKE_PROXY_ARG_1(LockPubScreenList)

void abiv0_UnlockPubScreenList(struct LibraryV0 *IntuitionBaseV0)
{
bug("abiv0_UnlockPubScreenList: STUB\n");
}
MAKE_PROXY_ARG_1(UnlockPubScreenList)

struct LibraryV0 *shallow_InitResident32(struct ResidentV0 *resident, BPTR segList, struct ExecBaseV0 *SysBaseV0);
BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);
APTR abiv0_DOS_OpenLibrary(CONST_STRPTR name, ULONG version, struct ExecBaseV0 *SysBaseV0);

ULONG abiv0_DoMethodA(APTR object, APTR message)
{
    struct HookV0 *clhook = (struct HookV0 *)(IPTR)OCLASSV0(object);
    ULONG ret;

    __asm__ volatile (
        "pushq %%rbx\n"
        "subq $12, %%rsp\n"
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
        "addq $12, %%rsp\n"
        "popq %%rbx\n"
        "movl %%eax, %0\n"
        :"=m"(ret) : "m"(clhook->h_Entry), "m"(clhook), "m"(object), "m"(message)
        : SCRATCH_REGS_64_TO_32 );

    return ret;
}

static void init_first_screen(struct LibraryV0 *IntuitionBaseV0)
{
    struct Screen *native = LockPubScreen(NULL);

    struct ScreenProxy *proxy = abiv0_AllocMem(sizeof(struct ScreenProxy), MEMF_CLEAR, Intuition_SysBaseV0);

    proxy->base.Screen.Width        = native->Width;
    proxy->base.Screen.Height       = native->Height;
    proxy->base.Screen.WBorLeft     = native->WBorLeft;
    proxy->base.Screen.WBorTop      = native->WBorTop;
    proxy->base.Screen.WBorRight    = native->WBorRight;
    proxy->base.Screen.WBorBottom   = native->WBorBottom;
    proxy->native                   = native;

    struct ColorMapProxy *cmproxy = abiv0_AllocMem(sizeof(struct ColorMapProxy), MEMF_CLEAR, Intuition_SysBaseV0);
    cmproxy->base.VPModeID = native->ViewPort.ColorMap->VPModeID;
    cmproxy->native = native->ViewPort.ColorMap;
    proxy->base.Screen.ViewPort.ColorMap    = (APTR32)(IPTR)cmproxy;

    *((IPTR *)&proxy->base.Screen.ViewPort.DspIns) = (IPTR)&native->ViewPort;
    proxy->base.Screen.ViewPort.DWidth      = native->ViewPort.DWidth;
    proxy->base.Screen.ViewPort.DHeight     = native->ViewPort.DHeight;
    proxy->base.Screen.ViewPort.DxOffset    = native->ViewPort.DxOffset;
    proxy->base.Screen.ViewPort.DyOffset    = native->ViewPort.DyOffset;

    struct TextAttrV0 * v0font = abiv0_AllocMem(sizeof(struct TextAttrV0), MEMF_CLEAR, Intuition_SysBaseV0);
    v0font->ta_YSize    = native->Font->ta_YSize;
    v0font->ta_Flags    = native->Font->ta_Flags;
    v0font->ta_Style    = native->Font->ta_Style;
    STRPTR v0font_name = abiv0_AllocMem(strlen(native->Font->ta_Name) + 1, MEMF_CLEAR, Intuition_SysBaseV0);
    CopyMem(native->Font->ta_Name, v0font_name, strlen(native->Font->ta_Name) + 1);
    v0font->ta_Name     = (APTR32)(IPTR)v0font_name;

    proxy->base.Screen.Font     = (APTR32)(IPTR)v0font;

    *(IPTR *)(&proxy->base.Screen.LayerInfo.PrivateReserve1)    = (IPTR)&native->LayerInfo;

    proxy->base.Screen.RastPort.Font = (APTR32)(IPTR)makeTextFontV0(native->RastPort.Font, Intuition_SysBaseV0);
    struct BitMapProxy *bmproxy = abiv0_AllocMem(sizeof(struct BitMapProxy), MEMF_CLEAR, Intuition_SysBaseV0);
    bmproxy->native = native->RastPort.BitMap;
    proxy->base.Screen.RastPort.BitMap = (APTR32)(IPTR)bmproxy;
    RastPortV0_attachnative(&proxy->base.Screen.RastPort, &native->RastPort);

    /* TODO: this should be a proxy to native intuition class */
    proxy->base.WinDecorObj = (APTR32)(IPTR)abiv0_NewObjectA(NULL, WINDECORCLASS, NULL, IntuitionBaseV0);

    g_mainnativescreen = native;
    g_mainv0screen = &proxy->base;

    UnlockPubScreen(NULL, native);
}

extern ULONG *segclassesinitlist;
extern ULONG *seginitlist;
extern ULONG _GlobalEditFunc;

void init_intuition(struct ExecBaseV0 *SysBaseV0, struct LibraryV0 *timerBase)
{
    TEXT path[64];
    NewRawDoFmt("LIBSV0:partial/intuition.library", RAWFMTFUNC_STRING, path);
    BPTR intuitionseg = LoadSeg32(path, DOSBase);
    struct ResidentV0 *intuitionres = findResident(intuitionseg, NULL);
    struct IntuitionBaseV0 *abiv0IntuitionBase = (struct IntuitionBaseV0 *)shallow_InitResident32(intuitionres, intuitionseg, SysBaseV0);
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
        ::"m"(Intuition_SysBaseV0), "m"(seginitlist[1])
        : SCRATCH_REGS_64_TO_32 );

    /* Set all LVO addresses to their number so that code jumps to "number" of the LVO and crashes */
    for (LONG i = 5; i <= 164; i++)
        __AROS_SETVECADDRV0(abiv0IntuitionBase,   i, (APTR32)(IPTR)i + 200 + 300 + 200);

    /* Set all working LVOs */
    __AROS_SETVECADDRV0(abiv0IntuitionBase,   1, (APTR32)(IPTR)proxy_Intuition_OpenLib);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,   2, (APTR32)(IPTR)proxy_Intuition_CloseLib);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,   3, (APTR32)(IPTR)proxy_Intuition_ExpungeLib);
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
    __AROS_SETVECADDRV0(abiv0IntuitionBase,   9, (APTR32)(IPTR)proxy_ClearMenuStrip);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  86, (APTR32)(IPTR)proxy_UnlockPubScreen);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 119, intuitionjmp[165 - 119]);  // FreeClass
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 118, intuitionjmp[165 - 118]);  // RemoveClass
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  44, (APTR32)(IPTR)proxy_SetMenuStrip);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 136, (APTR32)(IPTR)proxy_SetWindowPointerA);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  17, (APTR32)(IPTR)proxy_DoubleClick);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 145, intuitionjmp[165 - 145]);  // DoNotify
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  69, (APTR32)(IPTR)proxy_LockIBase);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  70, (APTR32)(IPTR)proxy_UnlockIBase);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 103, intuitionjmp[165 - 103]);  // DrawImageState
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 146, intuitionjmp[165 - 146]);  // FreeICData
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  41, intuitionjmp[165 -  41]);  // ScreenToBack
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  14, intuitionjmp[165 -  14]);  // CurrentTime
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  98, intuitionjmp[165 -  98]);  // EasyRequestArgs
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  99, intuitionjmp[165 -  99]);  // BuildEasyRequestArgs
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 100, intuitionjmp[165 - 100]);  // SysReqHandler
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  87, (APTR32)(IPTR)proxy_LockPubScreenList);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  88, (APTR32)(IPTR)proxy_UnlockPubScreenList);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 102, (APTR32)(IPTR)proxy_OpenScreenTagList);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  62, intuitionjmp[165 -  62]);  // FreeSysRequest
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  10, (APTR32)(IPTR)proxy_ClearPointer);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  55, intuitionjmp[165 -  55]);  // IntuiTextLength
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  24, intuitionjmp[165 -  24]);  // ItemAddress

    Intuition_Gadgets_init(abiv0IntuitionBase, intuitionjmp);
    Intuition_Windows_init(abiv0IntuitionBase, intuitionjmp);

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
            ::"m"(abiv0IntuitionBase), "m"(func)
            : SCRATCH_REGS_64_TO_32 );
        pos++;
        func = segclassesinitlist[pos];
    }

    /* Set internal Intuition pointer of utility, graphics and timer */
    abiv0IntuitionBase->UtilityBase = (APTR32)(IPTR)abiv0_DOS_OpenLibrary("utility.library", 0L, SysBaseV0);
    abiv0IntuitionBase->GfxBase     = (APTR32)(IPTR)abiv0_DOS_OpenLibrary("graphics.library", 0L, SysBaseV0);
    abiv0IntuitionBase->LayersBase  = (APTR32)(IPTR)abiv0_DOS_OpenLibrary("layers.library", 0L, SysBaseV0);
    abiv0IntuitionBase->KeymapBase  = (APTR32)(IPTR)abiv0_DOS_OpenLibrary("keymap.library", 0L, SysBaseV0);
    abiv0IntuitionBase->TimerBase   = (APTR32)(IPTR)timerBase;
    abiv0_InitSemaphore((struct SignalSemaphoreV0 *)((IPTR)abiv0IntuitionBase + 0x180), SysBaseV0); // GadgetLock
    abiv0_InitSemaphore((struct SignalSemaphoreV0 *)((IPTR)abiv0IntuitionBase + 0x0F8), SysBaseV0); // PubScrListLock
    NEWLISTV0((struct MinListV0 *)((IPTR)abiv0IntuitionBase + 0x12C)); // PubScreenList
    *(APTR32 *)((IPTR)abiv0IntuitionBase + 0x480) = (APTR32)(IPTR)NULL; // ReqFont

    abiv0IntuitionBase->GlobalEditHook = (APTR32)(IPTR)&abiv0IntuitionBase->DefaultEditHook;
    abiv0IntuitionBase->DefaultEditHook.h_Entry = (APTR32)(IPTR)(_GlobalEditFunc);
    abiv0IntuitionBase->DefaultEditHook.h_SubEntry = (APTR32)(IPTR)NULL;
    abiv0IntuitionBase->DefaultEditHook.h_Data = (APTR32)(IPTR)abiv0IntuitionBase;

    Intuition_Gadgets_init_GadgetWrapper_class();

    init_first_screen((struct LibraryV0 *)abiv0IntuitionBase);
    abiv0IntuitionBase->ActiveScreen = (APTR32)(IPTR)g_mainv0screen;
}
