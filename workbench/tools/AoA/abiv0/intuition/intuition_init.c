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
#include "../include/aros/call32.h"
#include "../include/intuition/structures.h"
#include "../include/intuition/proxy_structures.h"
#include "../include/graphics/proxy_structures.h"
#include "../include/utility/structures.h"
#include "../include/input/structures.h"

#include "../graphics/graphics_rastports.h"
#include "../exec/exec_libraries.h"

#include "intuition_gadgets.h"
#include "intuition_screens.h"
#include "intuition_windows.h"

#include "../support.h"

struct ExecBaseV0 *Intuition_SysBaseV0;

struct LibraryV0 *abiv0_Intuition_OpenLib(ULONG version, struct LibraryV0 *IntuitionBaseV0)
{
    IntuitionBaseV0->lib_OpenCnt++;
    return IntuitionBaseV0;
}
MAKE_PROXY_ARG_2(Intuition_OpenLib)

BPTR abiv0_Intuition_CloseLib(struct LibraryV0 *IntuitionBaseV0)
{
    IntuitionBaseV0->lib_OpenCnt--;
    return BNULL;
}
MAKE_PROXY_ARG_1(Intuition_CloseLib)

BPTR abiv0_Intuition_ExpungeLib(struct LibraryV0 *extralhV0, struct LibraryV0 *IntuitionBaseV0)
{
    struct IntuitionBaseV0 *abiv0IntuitionBase = (struct IntuitionBaseV0 *)IntuitionBaseV0;
    abiv0_CloseLibrary((struct LibraryV0 *)(IPTR)abiv0IntuitionBase->KeymapBase, Intuition_SysBaseV0);
    abiv0_CloseLibrary((struct LibraryV0 *)(IPTR)abiv0IntuitionBase->LayersBase, Intuition_SysBaseV0);
    abiv0_CloseLibrary((struct LibraryV0 *)(IPTR)abiv0IntuitionBase->GfxBase, Intuition_SysBaseV0);
    abiv0_CloseLibrary((struct LibraryV0 *)(IPTR)abiv0IntuitionBase->UtilityBase, Intuition_SysBaseV0);

    /* Call Remove on library base */
    CALL32_ARG_2_NR(__AROS_GETVECADDRV0(Intuition_SysBaseV0, 42), IntuitionBaseV0, (APTR32)(IPTR)Intuition_SysBaseV0);
    return BNULL;
}
MAKE_PROXY_ARG_2(Intuition_ExpungeLib)

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
unhandledCodePath(__func__, "Not ITEMTEXT", 0, menuitem->Flags);
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
    if (itext->ITextFont != (APTR32)0) unhandledCodePath(__func__, "ITextFont != NULL", 0, 0);
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
        if (menuitem->SelectFill != (APTR32)0) unhandledCodePath(__func__, "SelectFill != NULL", 0, 0);
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

extern ULONG *segclassesinitlist;
extern ULONG *seginitlist;
extern ULONG _GlobalEditFunc;

static BPTR intuitionseg;

void Intuition_Unhandled_init(struct IntuitionBaseV0 *abiv0IntuitionBase);

void init_intuition(struct ExecBaseV0 *SysBaseV0, struct LibraryV0 *timerBase)
{
    TEXT path[64];
    NewRawDoFmt("LIBSV0:partial/intuition.library", RAWFMTFUNC_STRING, path);
    intuitionseg = LoadSeg32(path, DOSBase);
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

    /* Set all unhandled LVO addresses to a catch function */
    Intuition_Unhandled_init(abiv0IntuitionBase);

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
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 107, intuitionjmp[165 - 107]);  // DisposeObject
    __AROS_SETVECADDRV0(abiv0IntuitionBase,   9, (APTR32)(IPTR)proxy_ClearMenuStrip);
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
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  14, intuitionjmp[165 -  14]);  // CurrentTime
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  98, intuitionjmp[165 -  98]);  // EasyRequestArgs
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  99, intuitionjmp[165 -  99]);  // BuildEasyRequestArgs
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 100, intuitionjmp[165 - 100]);  // SysReqHandler
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  62, intuitionjmp[165 -  62]);  // FreeSysRequest
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  10, (APTR32)(IPTR)proxy_ClearPointer);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  55, intuitionjmp[165 -  55]);  // IntuiTextLength
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  24, intuitionjmp[165 -  24]);  // ItemAddress

    Intuition_Screens_init(abiv0IntuitionBase, intuitionjmp);
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
    abiv0_InitSemaphore(&abiv0IntuitionBase->PubScrListLock, SysBaseV0);
    NEWLISTV0(&abiv0IntuitionBase->PubScreenList);
    *(APTR32 *)((IPTR)abiv0IntuitionBase + 0x480) = (APTR32)(IPTR)NULL; // ReqFont

    abiv0IntuitionBase->GlobalEditHook = (APTR32)(IPTR)&abiv0IntuitionBase->DefaultEditHook;
    abiv0IntuitionBase->DefaultEditHook.h_Entry = (APTR32)(IPTR)(_GlobalEditFunc);
    abiv0IntuitionBase->DefaultEditHook.h_SubEntry = (APTR32)(IPTR)NULL;
    abiv0IntuitionBase->DefaultEditHook.h_Data = (APTR32)(IPTR)abiv0IntuitionBase;

    Intuition_Gadgets_init_GadgetWrapper_class();

    Intuition_Screens_init_first_screen(abiv0IntuitionBase);
}

void exit_intuition()
{
    UnLoadSeg(intuitionseg);
}
