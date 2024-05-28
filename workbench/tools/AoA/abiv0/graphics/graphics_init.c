#include <proto/exec.h>
#include <proto/dos.h>
#include <graphics/rpattr.h>
#include <proto/graphics.h>
#include <proto/alib.h>
#include <aros/debug.h>
#include <exec/rawfmt.h>


#include "../include/exec/structures.h"
#include "../include/exec/functions.h"
#include "../include/aros/cpu.h"
#include "../include/aros/proxy.h"
#include "../include/graphics/structures.h"
#include "../include/graphics/proxy_structures.h"
#include "../include/utility/structures.h"

struct ExecBaseV0 *Gfx_SysBaseV0;

struct LibraryV0 *abiv0_Gfx_OpenLib(ULONG version, struct LibraryV0 *GfxBaseV0)
{
    return GfxBaseV0;
}
MAKE_PROXY_ARG_2(Gfx_OpenLib)

ULONG abiv0_GetBitMapAttr(struct BitMapV0 *bitmap, ULONG attribute, struct LibraryV0 *GfxBaseV0)
{
bug("abiv0_GetBitMapAttr: STUB\n");

    if (attribute == BMA_DEPTH)
    {
        return 24;
    } else if (attribute == BMA_WIDTH)
    {
        return 32;
    }
asm("int3");
}
MAKE_PROXY_ARG_3(GetBitMapAttr)

struct ExtSprite *abiv0_AllocSpriteDataA(struct BitMapV0 *bitmap, struct TagItemV0 *tagList, struct LibraryV0 *GfxBaseV0)
{
bug("abiv0_AllocSpriteDataA: STUB\n");
    return NULL;
}
MAKE_PROXY_ARG_3(AllocSpriteDataA)

void abiv0_GetRGB32(struct ColorMapV0 * cm, ULONG firstcolor, ULONG ncolors, ULONG *table, struct LibraryV0 *GfxBaseV0)
{
    struct ColorMapProxy *proxy = (struct ColorMapProxy *)cm;
    return GetRGB32(proxy->native, firstcolor, ncolors, table);
}
MAKE_PROXY_ARG_5(GetRGB32)

struct TextFontV0 *makeTextFontV0(struct TextFont *native, struct ExecBaseV0 *sysBaseV0)
{
    struct TextFontProxy *proxy = abiv0_AllocMem(sizeof(struct TextFontV0), MEMF_CLEAR, sysBaseV0);
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

    if (native->tf_CharLoc)
    {
        buff = abiv0_AllocMem(arrlen * sizeof(LONG), MEMF_CLEAR, sysBaseV0);
        CopyMem(native->tf_CharLoc, buff, arrlen * sizeof(LONG));
        v0tf->tf_CharLoc        = (APTR32)(IPTR)buff;
    }

    if (native->tf_CharSpace)
    {
        buff = abiv0_AllocMem(arrlen * sizeof(WORD), MEMF_CLEAR, sysBaseV0);
        CopyMem(native->tf_CharSpace, buff, arrlen * sizeof(WORD));
        v0tf->tf_CharSpace      = (APTR32)(IPTR)buff;
    }

    if (native->tf_CharKern)
    {
        buff = abiv0_AllocMem(arrlen * sizeof(WORD), MEMF_CLEAR, sysBaseV0);
        CopyMem(native->tf_CharKern, buff, arrlen * sizeof(WORD));
        v0tf->tf_CharKern       = (APTR32)(IPTR)buff;
    }

    proxy->native = native;
}

struct TextFontV0 *abiv0_OpenFont(struct TextAttrV0 *textAttr, struct LibraryV0 *GfxBaseV0)
{
    struct TextAttr tanative;
    tanative.ta_Flags   = textAttr->ta_Flags;
    tanative.ta_Style   = textAttr->ta_Style;
    tanative.ta_YSize   = textAttr->ta_YSize;
    tanative.ta_Name    = (STRPTR)(IPTR)textAttr->ta_Name;

    struct TextFont *tfnative = OpenFont(&tanative);
    if (tfnative == NULL)
        return NULL;
    else
        return makeTextFontV0(tfnative, Gfx_SysBaseV0);
}
MAKE_PROXY_ARG_2(OpenFont)

void abiv0_CloseFont(APTR textFont, struct GfxBaseV0 *GfxBaseV0)
{
bug("abiv0_CloseFont: STUB\n");
}
MAKE_PROXY_ARG_3(CloseFont)

LONG abiv0_ObtainBestPenA(struct ColorMapV0 *cm, ULONG r, ULONG g, ULONG b, struct TagItemV0 *tags, struct LibraryV0 *GfxBaseV0)
{
    struct ColorMapProxy *proxy = (struct ColorMapProxy *)cm;

    if (tags == NULL)
    {
        return ObtainBestPenA(proxy->native, r, g, b, NULL);
    }
    else
    {
        if (tags[0].ti_Tag == OBP_FailIfBad)
        {
            struct TagItem tagtmp[] =
            {
                { OBP_FailIfBad, FALSE },
                { TAG_DONE, 0L}
            };

            tagtmp[0].ti_Data = tags[0].ti_Data;
            return ObtainBestPenA(proxy->native, r, g, b, tagtmp);
        }
        else
        {
asm("int3");
        }
    }
    return 0;
}
MAKE_PROXY_ARG_6(ObtainBestPenA)

void abiv0_ReleasePen(struct ColorMapV0 *cm, ULONG n, struct GfxBaseV0 *GfxBaseV0)
{
    struct ColorMapProxy *proxy = (struct ColorMapProxy *)cm;
    ReleasePen(proxy->native, n);
}
MAKE_PROXY_ARG_3(ReleasePen)

void abiv0_SetDrMd(struct RastPortV0 *rp, ULONG drawMode, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = (struct RastPort *)*(IPTR *)&rp->longreserved;
    SetDrMd(rpnative, drawMode);
}
MAKE_PROXY_ARG_3(SetDrMd)

void abiv0_SetAPen(struct RastPortV0 *rp, ULONG pen, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = (struct RastPort *)*(IPTR *)&rp->longreserved;
    SetAPen(rpnative, pen);
}
MAKE_PROXY_ARG_3(SetAPen)

void abiv0_SetBPen(struct RastPortV0 *rp, ULONG pen, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = (struct RastPort *)*(IPTR *)&rp->longreserved;
    SetBPen(rpnative, pen);
}
MAKE_PROXY_ARG_3(SetBPen)

extern struct TagItem *CloneTagItemsV02Native(const struct TagItemV0 *tagList);
extern void FreeClonedV02NativeTagItems(struct TagItem *tagList);

void abiv0_SetABPenDrMd(struct RastPortV0 *rp, ULONG apen, ULONG bpen, ULONG drawMode, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = (struct RastPort *)*(IPTR *)&rp->longreserved;
    if (rpnative == NULL)
    {
        /* HFinder operates on locally created 1-bit RastPort/BitMap */
        /* TODO: call 32bit code? */
        rp->FgPen     = apen;
        rp->BgPen     = bpen;
        rp->DrawMode  = drawMode;
        rp->linpatcnt = 15;
#define RPF_NO_PENS	    	(1L << 14)	/* Are pens disabled?				*/
        rp->Flags    &= ~RPF_NO_PENS;
    }
    else
        SetABPenDrMd(rpnative, apen, bpen, drawMode);
}
MAKE_PROXY_ARG_5(SetABPenDrMd)

void  abiv0_SetRPAttrsA(struct RastPortV0 *rp, struct TagItemV0 *tags, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = (struct RastPort *)*(IPTR *)&rp->longreserved;
   struct TagItem *tagListNative = CloneTagItemsV02Native(tags);

    struct TagItem *tagNative = tagListNative;

    while (tagNative->ti_Tag != TAG_DONE)
    {
        if (tagNative->ti_Tag == RPTAG_Font)
        {
            asm("int3");
        }

        if (tagNative->ti_Tag == RPTAG_ClipRectangle)
        {
            asm("int3");
        }

        tagNative++;
    }

    SetRPAttrsA(rpnative, tagListNative);

    FreeClonedV02NativeTagItems(tagListNative);
}
MAKE_PROXY_ARG_3(SetRPAttrsA)

static void RecreteNativeRastPort(struct RastPortV0 *rpv0, struct RastPort *rptmp, struct BitMap *bmtmp)
{
    struct BitMapV0 *bmv0 = (struct BitMapV0 *)(APTR)(IPTR)rpv0->BitMap;
    InitRastPort(rptmp);

    rptmp->FgPen        = rpv0->FgPen;
    rptmp->BgPen        = rpv0->BgPen;
    rptmp->DrawMode     = rpv0->DrawMode;
    rptmp->linpatcnt    = rpv0->linpatcnt;
    rptmp->Flags        = rpv0->Flags;
    rptmp->cp_x         = rpv0->cp_x;
    rptmp->cp_y         = rpv0->cp_y;

    bmtmp->BytesPerRow  = bmv0->BytesPerRow;
    bmtmp->Depth        = bmv0->Depth;
    bmtmp->Flags        = bmv0->Flags;
    bmtmp->Rows         = bmv0->Rows;
    bmtmp->Planes[0]    = (APTR)(IPTR)bmv0->Planes[0];

    rptmp->BitMap       = bmtmp;
}

void abiv0_RectFill(struct RastPortV0 * rp, LONG xMin, LONG yMin, LONG xMax, LONG yMax, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = (struct RastPort *)*(IPTR *)&rp->longreserved;
    if (rpnative == NULL)
    {
        /* HFinder operates on locally created 1-bit RastPort/BitMap */
        struct RastPort rptmp;
        struct BitMap bmtmp;
        RecreteNativeRastPort(rp, &rptmp, &bmtmp);

        RectFill(&rptmp, xMin, yMin, xMax, yMax);
    }
    else
        RectFill(rpnative, xMin, yMin, xMax, yMax);
}
MAKE_PROXY_ARG_6(RectFill)

void abiv0_Move(struct RastPortV0 *rp, WORD x, WORD y, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = (struct RastPort *)*(IPTR *)&rp->longreserved;

    if (rpnative == NULL)
    {
        /* HFinder operates on locally created 1-bit RastPort/BitMap */
        /* TODO: call 32bit code? */
        rp->cp_x        = x;
        rp->cp_y        = y;
        rp->linpatcnt   = 15;
    }
    else
        Move(rpnative, x, y);
}
MAKE_PROXY_ARG_4(Move)

void abiv0_Draw(struct RastPortV0 *rp, WORD x, WORD y, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = (struct RastPort *)*(IPTR *)&rp->longreserved;
    Draw(rpnative, x, y);
}
MAKE_PROXY_ARG_4(Draw)

void abiv0_Text(struct RastPortV0 *rp, CONST_STRPTR string, ULONG count, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = (struct RastPort *)*(IPTR *)&rp->longreserved;

    if (rpnative == NULL)
    {
        /* HFinder operates on locally created 1-bit RastPort/BitMap */
        struct RastPort rptmp;
        struct BitMap bmtmp;
        RecreteNativeRastPort(rp, &rptmp, &bmtmp);

        /* TODO: actually call 32-bit code without creating native objects ? */
        Text(&rptmp, string, count);
    }
    else
        Text(rpnative, string, count);
}
MAKE_PROXY_ARG_4(Text)

LONG abiv0_WritePixel(struct RastPortV0 *rp, LONG x, LONG y, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = (struct RastPort *)*(IPTR *)&rp->longreserved;
    return WritePixel(rpnative, x, y);
}
MAKE_PROXY_ARG_4(WritePixel)

struct RegionV0 *abiv0_NewRegion(struct GfxBaseV0 *GfxBaseV0)
{
    struct RegionProxy *proxy = abiv0_AllocMem(sizeof(struct RegionProxy), MEMF_CLEAR, Gfx_SysBaseV0);
    proxy->native = NewRegion();
    return (struct RegionV0 *)proxy;
}
MAKE_PROXY_ARG_1(NewRegion)

struct RegionV0 *abiv0_NewRectRegion(WORD MinX, WORD MinY, WORD MaxX, WORD MaxY, struct GfxBaseV0 *GfxBaseV0)
{
    struct RegionProxy *proxy = abiv0_AllocMem(sizeof(struct RegionProxy), MEMF_CLEAR, Gfx_SysBaseV0);
    proxy->native = NewRectRegion(MinX, MinY, MaxX, MaxY);
    return (struct RegionV0 *)proxy;
}
MAKE_PROXY_ARG_5(NewRectRegion)

BOOL abiv0_OrRectRegion(struct RegionV0 *Reg, struct Rectangle *Rect, struct GfxBaseV0 *GfxBaseV0)
{
    struct RegionProxy *proxy = (struct RegionProxy *)Reg;
    return OrRectRegion(proxy->native, Rect);
}
MAKE_PROXY_ARG_3(OrRectRegion)

void abiv0_DisposeRegion(struct RegionV0 *region, struct GfxBaseV0 *GfxBaseV0)
{
    struct RegionProxy *proxy = (struct RegionProxy *)region;
    return DisposeRegion(proxy->native);
}
MAKE_PROXY_ARG_2(DisposeRegion)

BOOL abiv0_ClearRectRegion(struct RegionV0 *Reg, struct Rectangle *Rect, struct GfxBaseV0 *GfxBaseV0)
{
    struct RegionProxy *proxy = (struct RegionProxy *)Reg;
    return ClearRectRegion(proxy->native, Rect);
}
MAKE_PROXY_ARG_3(ClearRectRegion)

ULONG abiv0_GetVPModeID(struct ViewPortV0 * vp, struct GfxBaseV0 *GfxBaseV0)
{
    struct ViewPort *vpnative = (struct ViewPort *)*(IPTR *)&vp->DspIns;
    return INVALID_ID;
}
MAKE_PROXY_ARG_2(GetVPModeID)

ULONG abiv0_GetDisplayInfoData(APTR32 handle, UBYTE *buf, ULONG size, ULONG tagID, ULONG ID, struct GfxBaseV0 *GfxBaseV0)
{
bug("abiv0_GetDisplayInfoData: STUB\n");
    return 0;
}
MAKE_PROXY_ARG_6(GetDisplayInfoData)

BOOL abiv0_AndRegionRegion(struct RegionV0 *R1, struct RegionV0 *R2, struct GfxBaseV0 *GfxBaseV0)
{
    struct RegionProxy *proxy1 = (struct RegionProxy *)R1;
    struct RegionProxy *proxy2 = (struct RegionProxy *)R2;
    return AndRegionRegion(proxy1->native, proxy2->native);
}
MAKE_PROXY_ARG_3(AndRegionRegion)

void abiv0_BltTemplate(PLANEPTR source, LONG xSrc, LONG srcMod, struct RastPortV0 *destRP, LONG xDest, LONG yDest, LONG xSize,
    LONG ySize, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = (struct RastPort *)*(IPTR *)&destRP->longreserved;

    if (rpnative == NULL)
    {
        /* HFinder operates on locally created 1-bit RastPort/BitMap */
        struct RastPort rptmp;
        struct BitMap bmtmp;
        RecreteNativeRastPort(destRP, &rptmp, &bmtmp);

        /* TODO: actually call 32-bit code without creating native objects ? */
        BltTemplate(source, xSrc, srcMod, &rptmp, xDest, yDest, xSize, ySize);
    }
    else
        BltTemplate(source, xSrc, srcMod, rpnative, xDest, yDest, xSize, ySize);
}
MAKE_PROXY_ARG_12(BltTemplate)

VOID abiv0_WaitTOF(struct GfxBaseV0 *GfxBaseV0)
{
    WaitTOF();
}
MAKE_PROXY_ARG_1(WaitTOF)

struct LibraryV0 *shallow_InitResident32(struct ResidentV0 *resident, BPTR segList, struct ExecBaseV0 *SysBaseV0);
BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);
struct ResidentV0 * findResident(BPTR seg, CONST_STRPTR name);
APTR abiv0_DOS_OpenLibrary(CONST_STRPTR name, ULONG version, struct ExecBaseV0 *SysBaseV0);

extern CONST_STRPTR SYSNAME;

void init_graphics(struct ExecBaseV0 *SysBaseV0)
{
    TEXT path[64];
    NewRawDoFmt("%s:Libs32/partial/graphics.library", RAWFMTFUNC_STRING, path, SYSNAME);
    BPTR graphicsseg = LoadSeg32(path, DOSBase);
    struct ResidentV0 *graphicsres = findResident(graphicsseg, NULL);
    struct GfxBaseV0 *abiv0GfxBase = (struct GfxBaseV0 *)shallow_InitResident32(graphicsres, graphicsseg, SysBaseV0);
    Gfx_SysBaseV0 = SysBaseV0;
    /* Remove all vectors for now */
    const ULONG graphicsjmpsize = 202 * sizeof(APTR32);
    APTR32 *graphicsjmp = AllocMem(graphicsjmpsize, MEMF_CLEAR);
    CopyMem((APTR)abiv0GfxBase - graphicsjmpsize, graphicsjmp, graphicsjmpsize);
    for (int i = 1; i <= 201; i++) __AROS_SETVECADDRV0(abiv0GfxBase, i, 0);
    abiv0GfxBase->ExecBase = (APTR32)(IPTR)SysBaseV0;
    *(ULONG *)((IPTR)abiv0GfxBase + 0x4b0) = (APTR32)(IPTR)abiv0_DOS_OpenLibrary("utility.library", 0L, SysBaseV0);
    NEWLISTV0(&abiv0GfxBase->TextFonts);

    __AROS_SETVECADDRV0(abiv0GfxBase,   1, (APTR32)(IPTR)proxy_Gfx_OpenLib);
    __AROS_SETVECADDRV0(abiv0GfxBase,  12, (APTR32)(IPTR)proxy_OpenFont);
    __AROS_SETVECADDRV0(abiv0GfxBase, 160, (APTR32)(IPTR)proxy_GetBitMapAttr);
    __AROS_SETVECADDRV0(abiv0GfxBase, 150, (APTR32)(IPTR)proxy_GetRGB32);
    __AROS_SETVECADDRV0(abiv0GfxBase, 140, (APTR32)(IPTR)proxy_ObtainBestPenA);
    __AROS_SETVECADDRV0(abiv0GfxBase,  33, graphicsjmp[202 -  33]);  // InitRastPort
    __AROS_SETVECADDRV0(abiv0GfxBase,  11, graphicsjmp[202 -  11]);  // SetFont
    __AROS_SETVECADDRV0(abiv0GfxBase, 134, graphicsjmp[202 - 134]);  // WeighTAMatch
    __AROS_SETVECADDRV0(abiv0GfxBase, 136, graphicsjmp[202 - 136]);  // ExtendFont
    __AROS_SETVECADDRV0(abiv0GfxBase,  80, graphicsjmp[202 -  80]);  // AddFont
    __AROS_SETVECADDRV0(abiv0GfxBase,   9, graphicsjmp[202 -   9]);  // TextLength
    __AROS_SETVECADDRV0(abiv0GfxBase,  59, (APTR32)(IPTR)proxy_SetDrMd);
    __AROS_SETVECADDRV0(abiv0GfxBase,  57, (APTR32)(IPTR)proxy_SetAPen);
    __AROS_SETVECADDRV0(abiv0GfxBase,  51, (APTR32)(IPTR)proxy_RectFill);
    __AROS_SETVECADDRV0(abiv0GfxBase,  86, (APTR32)(IPTR)proxy_NewRegion);
    __AROS_SETVECADDRV0(abiv0GfxBase,  85, (APTR32)(IPTR)proxy_OrRectRegion);
    __AROS_SETVECADDRV0(abiv0GfxBase,  89, (APTR32)(IPTR)proxy_DisposeRegion);
    __AROS_SETVECADDRV0(abiv0GfxBase,  40, (APTR32)(IPTR)proxy_Move);
    __AROS_SETVECADDRV0(abiv0GfxBase,  41, (APTR32)(IPTR)proxy_Draw);
    __AROS_SETVECADDRV0(abiv0GfxBase,  14, graphicsjmp[202 -  14]);  // AskSoftStyle
    __AROS_SETVECADDRV0(abiv0GfxBase,  15, graphicsjmp[202 -  15]);  // SetSoftStyle
    __AROS_SETVECADDRV0(abiv0GfxBase,  10, (APTR32)(IPTR)proxy_Text);
    __AROS_SETVECADDRV0(abiv0GfxBase,  87, (APTR32)(IPTR)proxy_ClearRectRegion);
    __AROS_SETVECADDRV0(abiv0GfxBase,  54, (APTR32)(IPTR)proxy_WritePixel);
    __AROS_SETVECADDRV0(abiv0GfxBase, 158, (APTR32)(IPTR)proxy_ReleasePen);
    __AROS_SETVECADDRV0(abiv0GfxBase,  13, (APTR32)(IPTR)proxy_CloseFont);
    __AROS_SETVECADDRV0(abiv0GfxBase, 170, (APTR32)(IPTR)proxy_AllocSpriteDataA);
    __AROS_SETVECADDRV0(abiv0GfxBase, 127, graphicsjmp[202 - 127]);  // FontExtent
    __AROS_SETVECADDRV0(abiv0GfxBase, 115, graphicsjmp[202 - 115]);  // TextExtent
    __AROS_SETVECADDRV0(abiv0GfxBase, 149, (APTR32)(IPTR)proxy_SetABPenDrMd);
    __AROS_SETVECADDRV0(abiv0GfxBase,  58, (APTR32)(IPTR)proxy_SetBPen);
    __AROS_SETVECADDRV0(abiv0GfxBase, 116, graphicsjmp[202 - 116]);  // TextFit
    __AROS_SETVECADDRV0(abiv0GfxBase, 194, (APTR32)(IPTR)proxy_NewRectRegion);
    __AROS_SETVECADDRV0(abiv0GfxBase, 132, (APTR32)(IPTR)proxy_GetVPModeID);
    __AROS_SETVECADDRV0(abiv0GfxBase, 126, (APTR32)(IPTR)proxy_GetDisplayInfoData);
    __AROS_SETVECADDRV0(abiv0GfxBase, 173, (APTR32)(IPTR)proxy_SetRPAttrsA);
    __AROS_SETVECADDRV0(abiv0GfxBase, 104, (APTR32)(IPTR)proxy_AndRegionRegion);
    __AROS_SETVECADDRV0(abiv0GfxBase,  65, graphicsjmp[202 -  65]);  // InitBitmap
    __AROS_SETVECADDRV0(abiv0GfxBase,   6, (APTR32)(IPTR)proxy_BltTemplate);
    __AROS_SETVECADDRV0(abiv0GfxBase,  38, graphicsjmp[202 -  38]);  // WaitBlit
    __AROS_SETVECADDRV0(abiv0GfxBase,  45, (APTR32)(IPTR)proxy_WaitTOF);
}
