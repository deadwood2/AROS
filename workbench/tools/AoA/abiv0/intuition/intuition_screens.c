#include <aros/debug.h>
#include <proto/intuition.h>

#include <string.h>

#include "../include/aros/cpu.h"
#include "../include/aros/proxy.h"
#include "../include/exec/functions.h"
#include "../include/exec/proxy_structures.h"
#include "../include/intuition/structures.h"
#include "../include/graphics/proxy_structures.h"

#include "../graphics/graphics_rastports.h"
#include "../support.h"

extern struct ExecBaseV0 *Intuition_SysBaseV0;

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

static struct IntScreenV0  *g_mainv0screen;
static struct Screen       *g_mainnativescreen;

static struct IntScreenV0  *g_additionalv0screen;
static struct Screen       *g_additionalnativescreen;
static TEXT                g_additionalscreenname[64];

struct Screen *screenRemapV02N(struct ScreenV0 *v0screen)
{
    if (v0screen == NULL) return NULL;
    if (v0screen == (struct ScreenV0 *)g_mainv0screen) return g_mainnativescreen;
    if (v0screen == (struct ScreenV0 *)g_additionalv0screen) return g_additionalnativescreen;

unhandledCodePath(__func__, "No match", 0, 0);
    return NULL;
}

struct ScreenV0 *screenRemapN2V0(struct Screen *nscreen)
{
    if (nscreen == NULL) return NULL;
    if (nscreen == g_mainnativescreen) return (struct ScreenV0 *)g_mainv0screen;
    if (nscreen == g_additionalnativescreen) return (struct ScreenV0 *)g_additionalv0screen;

unhandledCodePath(__func__, "No match", 0, 0);
    return NULL;
}

extern struct TextFontV0 *makeTextFontV0(struct TextFont *native, struct ExecBaseV0 *sysBaseV0);

struct ScreenV0 *abiv0_LockPubScreen(CONST_STRPTR name, struct LibraryV0 *IntuitionBaseV0)
{
    if (name != NULL && strcmp(name, "Workbench") != 0)
    {
        // non-Workbench screen requsted
        if (g_additionalscreenname[0] == '\0' ) ; // none yet opened, ok
        else
unhandledCodePath(__func__, "Already opened", 0, 0);
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
unhandledCodePath(__func__, "Not main screen", 0, 0);

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
    ScreenDepth(screenRemapV02N(screen), flags, reserved);
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

/* TODO: should use proxy structures like window and layer */
void syncScreenV0()
{
    g_mainv0screen->Screen.MouseX   = g_mainnativescreen->MouseX;
    g_mainv0screen->Screen.MouseY   = g_mainnativescreen->MouseY;

    if (g_additionalnativescreen && g_additionalv0screen)
    {
        g_additionalv0screen->Screen.MouseX   = g_additionalnativescreen->MouseX;
        g_additionalv0screen->Screen.MouseY   = g_additionalnativescreen->MouseY;
    }
}

struct NewScreenV0
{

};

struct IClassV0;

APTR abiv0_NewObjectA(struct IClassV0  *classPtr, UBYTE *classID, struct TagItemV0 * tagList, struct LibraryV0 *IntuitionBaseV0);

static struct ScreenProxy *makeScreenProxy(struct Screen *native, struct LibraryV0 *IntuitionBaseV0)
{
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
    bmproxy->key    = BITMAPPROXYKEY;
    bmproxy->native = native->RastPort.BitMap;
    proxy->base.Screen.RastPort.BitMap = (APTR32)(IPTR)bmproxy;
    RastPortV0_attachnative(&proxy->base.Screen.RastPort, &native->RastPort);

    /* TODO: this should be a proxy to native intuition class */
    proxy->base.WinDecorObj = (APTR32)(IPTR)abiv0_NewObjectA(NULL, WINDECORCLASS, NULL, IntuitionBaseV0);

    return proxy;
}

static void addToPubScreenList(struct ScreenProxy *proxy, struct IntuitionBaseV0 *IntuitionBaseV0)
{
    /* PubScreenNode for that screen */
    struct PubScreenNodeV0 *psn = abiv0_AllocMem(sizeof(struct PubScreenNodeV0), MEMF_CLEAR, Intuition_SysBaseV0);
    psn->psn_Node.ln_Name = (APTR32)(IPTR)abiv0_AllocMem(MAXPUBSCREENNAME + 1, MEMF_ANY, Intuition_SysBaseV0);
    psn->psn_Screen = (APTR32)(IPTR)&proxy->base;

    struct List *plist = LockPubScreenList();
    struct PubScreenNode *pnode;
    ForeachNode(plist, pnode)
    {
        if (pnode->psn_Screen == proxy->native)
        {
            strcpy((char *)(IPTR)psn->psn_Node.ln_Name, pnode->psn_Node.ln_Name);
            psn->psn_Flags = pnode->psn_Flags;
            psn->psn_VisitorCount = pnode->psn_VisitorCount;
        }
    }

    UnlockPubScreenList();
    ADDTAILV0(&IntuitionBaseV0->PubScreenList, psn);
}

struct ScreenV0 * abiv0_OpenScreenTagList(struct NewScreenV0 *newScreen, struct TagItemV0 *tagList, struct LibraryV0 *IntuitionBaseV0)
{
    if (newScreen != NULL) unhandledCodePath(__func__, "newScreen != NULL", 0, 0);

    struct TagItem *tagListNative = CloneTagItemsV02Native(tagList);

    struct TagItem *tagNative = tagListNative;

    while (tagNative->ti_Tag != TAG_DONE)
    {
        switch (tagNative->ti_Tag)
        {
            case (SA_ShowTitle):
            case (SA_Type):
            case (SA_Depth):
            case (SA_PubName):
            case (SA_DisplayID):
            case (SA_SharePens):
            case (SA_Pens):
            case (SA_Quiet):
            case (SA_Title):
                break;
            case (SA_BackFill):
            {
bug("abiv0_OpenScreenTagList: Removing SA_BackFill\n");
                tagNative->ti_Tag = TAG_IGNORE;
                break;
            }
            default:
                unhandledCodePath(__func__, "Tags", 0, tagNative->ti_Tag);
                break;
        }

        tagNative++;
    }

bug("abiv0_OpenScreenTagList: STUB\n");
    struct Screen *scrnative = OpenScreenTagList(NULL, tagListNative);

    FreeClonedV02NativeTagItems(tagListNative);

    if (scrnative)
    {
        struct ScreenProxy *proxy = makeScreenProxy(scrnative, IntuitionBaseV0);
        addToPubScreenList(proxy, (struct IntuitionBaseV0 *)IntuitionBaseV0);

        g_additionalnativescreen = proxy->native;
        g_additionalv0screen = &proxy->base;

        return (struct ScreenV0 *)&proxy->base;
    }

    return NULL;
}
MAKE_PROXY_ARG_3(OpenScreenTagList)

UWORD abiv0_PubScreenStatus(struct ScreenV0 *Scr, UWORD StatusFlags, struct LibraryV0 *IntuitionBaseV0)
{
    struct ScreenProxy *proxy = (struct ScreenProxy *)Scr;
/* TODO: synchronize pubScrNode->psn_Flags */
bug("abiv0_PubScreenStatus: STUB\n");
    return PubScreenStatus(proxy->native, StatusFlags);
}
MAKE_PROXY_ARG_3(PubScreenStatus)

BOOL abiv0_CloseScreen(struct ScreenV0 *screen, struct LibraryV0 *IntuitionBaseV0)
{
    struct ScreenProxy *proxy = (struct ScreenProxy *)screen;
    BOOL _ret = CloseScreen(proxy->native);
    if (_ret)
    {
        g_additionalnativescreen = NULL;
        g_additionalv0screen = NULL;
        // MEMLEAK freeproxy and screenV0
    }
    return _ret;
}
MAKE_PROXY_ARG_2(CloseScreen)

struct ListV0 *abiv0_LockPubScreenList(struct IntuitionBaseV0 *IntuitionBaseV0)
{
    return (struct ListV0 *)&IntuitionBaseV0->PubScreenList;
}
MAKE_PROXY_ARG_1(LockPubScreenList)

void abiv0_UnlockPubScreenList(struct LibraryV0 *IntuitionBaseV0)
{
bug("abiv0_UnlockPubScreenList: STUB\n");
}
MAKE_PROXY_ARG_1(UnlockPubScreenList)

void Intuition_Screens_init_first_screen(struct IntuitionBaseV0 *IntuitionBaseV0)
{
    struct Screen *native = LockPubScreen(NULL);

    struct ScreenProxy *proxy = makeScreenProxy(native, (struct LibraryV0 *)IntuitionBaseV0);

    addToPubScreenList(proxy, IntuitionBaseV0);

    g_mainnativescreen = proxy->native;
    g_mainv0screen = &proxy->base;

    UnlockPubScreen(NULL, native);

    IntuitionBaseV0->ActiveScreen = (APTR32)(IPTR)g_mainv0screen;
    IntuitionBaseV0->FirstScreen = (APTR32)(IPTR)g_mainv0screen;
}

void Intuition_Screens_init(struct IntuitionBaseV0 *abiv0IntuitionBase, APTR32 *intuitionjmp)
{
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  85, (APTR32)(IPTR)proxy_LockPubScreen);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  42, intuitionjmp[165 -  42]);  // ScreenToFront
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 131, (APTR32)(IPTR)proxy_ScreenDepth);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 115, (APTR32)(IPTR)proxy_GetScreenDrawInfo);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 116, (APTR32)(IPTR)proxy_FreeScreenDrawInfo);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  86, (APTR32)(IPTR)proxy_UnlockPubScreen);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  87, (APTR32)(IPTR)proxy_LockPubScreenList);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  41, intuitionjmp[165 -  41]);  // ScreenToBack
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  88, (APTR32)(IPTR)proxy_UnlockPubScreenList);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 102, (APTR32)(IPTR)proxy_OpenScreenTagList);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  92, (APTR32)(IPTR)proxy_PubScreenStatus);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  11, (APTR32)(IPTR)proxy_CloseScreen);
}
