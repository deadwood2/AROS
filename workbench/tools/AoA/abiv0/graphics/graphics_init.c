#include <proto/exec.h>
#include <proto/dos.h>
#include <graphics/gfxbase.h>
#include <proto/graphics.h>
#include <proto/alib.h>
#include <aros/debug.h>
#include <exec/rawfmt.h>

#include <string.h>


#include "../include/exec/structures.h"
#include "../include/exec/functions.h"
#include "../include/aros/cpu.h"
#include "../include/aros/proxy.h"
#include "../include/graphics/structures.h"
#include "../include/graphics/proxy_structures.h"
#include "../include/utility/structures.h"

#include "graphics_rastports.h"
#include "graphics_regions.h"

struct ExecBaseV0 *Gfx_SysBaseV0;

void syncLayerV0(struct LayerProxy *proxy)
{
    proxy->base.Flags               = proxy->native->Flags;
}

struct LibraryV0 *abiv0_Gfx_OpenLib(ULONG version, struct LibraryV0 *GfxBaseV0)
{
    return GfxBaseV0;
}
MAKE_PROXY_ARG_2(Gfx_OpenLib)

ULONG abiv0_GetBitMapAttr(struct BitMapV0 *bitmap, ULONG attribute, struct LibraryV0 *GfxBaseV0)
{

    struct BitMapProxy *bmproxy = (struct BitMapProxy *)bitmap;

    if (bitmap == NULL)
    {
bug("abiv0_GetBitMapAttr: STUB\n");
        if (attribute == BMA_DEPTH)
        {
            return 24;
        }
asm("int3");
    }

    if (bmproxy->native)
    {
        return GetBitMapAttr(bmproxy->native, attribute);
    }
    else
    {
        if (attribute == BMA_FLAGS)
        {
            return bitmap->Flags;
        }
asm("int3");
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
    struct TextFontProxy *proxy = abiv0_AllocMem(sizeof(struct TextFontProxy), MEMF_CLEAR, sysBaseV0);
    struct TextFontV0 *v0tf = &proxy->base.ctf_TF;
    APTR buff;

    LONG namelen = strlen(native->tf_Message.mn_Node.ln_Name) + 1;
    buff = abiv0_AllocMem(namelen, MEMF_CLEAR, sysBaseV0);
    CopyMem(native->tf_Message.mn_Node.ln_Name, buff, namelen);
    v0tf->tf_Message.mn_Node.ln_Name = (APTR32)(IPTR)buff;

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

    if (native->tf_Style & FSF_COLORFONT)
    {
        struct ColorTextFont *ctfnative = (struct ColorTextFont *)native;
        if ((ctfnative->ctf_Flags & CT_COLORMASK) != CT_ANTIALIAS)
        {
asm("int3");
        }
        else
        {
            proxy->base.ctf_Flags = ctfnative->ctf_Flags;
        }
    }

    proxy->native = native;
    return (struct TextFontV0 *)proxy;
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

extern struct TagItem *CloneTagItemsV02Native(const struct TagItemV0 *tagList);
extern void FreeClonedV02NativeTagItems(struct TagItem *tagList);

LONG abiv0_ObtainBestPenA(struct ColorMapV0 *cm, ULONG r, ULONG g, ULONG b, struct TagItemV0 *tags, struct LibraryV0 *GfxBaseV0)
{
    struct ColorMapProxy *proxy = (struct ColorMapProxy *)cm;

    if (tags == NULL)
        return ObtainBestPenA(proxy->native, r, g, b, NULL);

    struct TagItem *tagListNative = CloneTagItemsV02Native(tags);

    struct TagItem *tagNative = tagListNative;
    LONG _ret = ObtainBestPenA(proxy->native, r, g, b, tagListNative);

    FreeClonedV02NativeTagItems(tagListNative);
    return _ret;
}
MAKE_PROXY_ARG_6(ObtainBestPenA)

LONG abiv0_ObtainPen(struct ColorMapV0 *cm, ULONG n, ULONG r, ULONG g, ULONG b, ULONG flags, struct LibraryV0 *GfxBaseV0)
{
    struct ColorMapProxy *proxy = (struct ColorMapProxy *)cm;
    return ObtainPen(proxy->native, n, r, g, b, flags);
}
MAKE_PROXY_ARG_12(ObtainPen)

void abiv0_ReleasePen(struct ColorMapV0 *cm, ULONG n, struct GfxBaseV0 *GfxBaseV0)
{
    struct ColorMapProxy *proxy = (struct ColorMapProxy *)cm;
    if (proxy) // ghostscript 8.50 passes NULL cm
        ReleasePen(proxy->native, n);
}
MAKE_PROXY_ARG_3(ReleasePen)

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

    for (LONG i = 0; i < bmv0->Depth; i++)
        bmtmp->Planes[i]= (APTR)(IPTR)bmv0->Planes[i];

    rptmp->BitMap       = bmtmp;
}

void abiv0_RectFill(struct RastPortV0 * rp, LONG xMin, LONG yMin, LONG xMax, LONG yMax, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    if (rpnative == NULL)
    {
        /* HFinder operates on locally created 1-bit RastPort/BitMap */
        struct RastPort rptmp;
        struct BitMap bmtmp;
        RecreteNativeRastPort(rp, &rptmp, &bmtmp);

        RectFill(&rptmp, xMin, yMin, xMax, yMax);
    }
    else
    {
        synchronize_SetAfPt(rp, rpnative);
        RectFill(rpnative, xMin, yMin, xMax, yMax);
    }
}
MAKE_PROXY_ARG_6(RectFill)

void abiv0_Move(struct RastPortV0 *rp, WORD x, WORD y, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);

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
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    Draw(rpnative, x, y);
}
MAKE_PROXY_ARG_4(Draw)

void abiv0_Text(struct RastPortV0 *rp, CONST_STRPTR string, ULONG count, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);

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
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    return WritePixel(rpnative, x, y);
}
MAKE_PROXY_ARG_4(WritePixel)

ULONG abiv0_GetVPModeID(struct ViewPortV0 * vp, struct GfxBaseV0 *GfxBaseV0)
{
    struct ViewPort *vpnative = (struct ViewPort *)*(IPTR *)&vp->DspIns;
    return GetVPModeID(vpnative);
}
MAKE_PROXY_ARG_2(GetVPModeID)

ULONG abiv0_GetDisplayInfoData(APTR32 handle, UBYTE *buf, ULONG size, ULONG tagID, ULONG ID, struct GfxBaseV0 *GfxBaseV0)
{
bug("abiv0_GetDisplayInfoData: STUB\n");
    return 0;
}
MAKE_PROXY_ARG_6(GetDisplayInfoData)

void abiv0_BltTemplate(PLANEPTR source, LONG xSrc, LONG srcMod, struct RastPortV0 *destRP, LONG xDest, LONG yDest, LONG xSize,
    LONG ySize, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(destRP);

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

LONG abiv0_WritePixelArray8(struct RastPortV0 *rp, ULONG xstart, ULONG ystart, ULONG xstop, ULONG ystop, UBYTE *array,
    struct RastPortV0 *temprp, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    if (rpnative == NULL)
    {
        /* HollyPaint operates on locally created RastPort/BitMap */
        struct RastPort rptmp;
        struct BitMap bmtmp;
        RecreteNativeRastPort(rp, &rptmp, &bmtmp);

        return WritePixelArray8(&rptmp, xstart, ystart, xstop, ystop, array, NULL);
    }
    else
        return WritePixelArray8(rpnative, xstart, ystart, xstop, ystop, array, NULL);
}
MAKE_PROXY_ARG_12(WritePixelArray8)

LONG abiv0_ReadPixelArray8(struct RastPortV0 *rp, LONG xstart, LONG ystart, LONG xstop, LONG ystop, UBYTE *array,
    struct RastPortV0 *temprp, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    if (rpnative == NULL)
    {
        /* Soliton operates on locally created RastPort/BitMap */
        struct RastPort rptmp;
        struct BitMap bmtmp;
        RecreteNativeRastPort(rp, &rptmp, &bmtmp);

        return ReadPixelArray8(&rptmp, xstart, ystart, xstop, ystop, array, NULL);
    }
    else
        return ReadPixelArray8(rpnative, xstart, ystart, xstop, ystop, array, NULL);
}
MAKE_PROXY_ARG_12(ReadPixelArray8)

void abiv0_SetRGB32(struct ViewPortV0 *vp, ULONG n, ULONG r, ULONG g, ULONG b, struct GfxBaseV0 *GfxBaseV0)
{
    struct ViewPort *vpnative = (struct ViewPort *)*(IPTR *)&vp->DspIns;
    SetRGB32(vpnative, n, r, g, b);
}
MAKE_PROXY_ARG_6(SetRGB32)

ULONG abiv0_NextDisplayInfo(ULONG last_ID, struct GfxBaseV0 *GfxBaseV0)
{
    return NextDisplayInfo(last_ID);
}
MAKE_PROXY_ARG_2(NextDisplayInfo)

DisplayInfoHandle abiv0_FindDisplayInfo(ULONG ID, struct GfxBaseV0 *GfxBaseV0)
{
bug("abiv0_FindDisplayInfo: STUB\n");
    return NULL;
}
MAKE_PROXY_ARG_2(FindDisplayInfo)

#include <proto/utility.h>

struct BitMapV0 *abiv0_AllocBitMap(ULONG sizex, ULONG sizey, ULONG depth, ULONG flags, struct BitMapV0 *friend_bitmap,
    struct GfxBaseV0 *GfxBaseV0)
{
    struct BitMapProxy *bmproxy = abiv0_AllocMem(sizeof(struct BitMapProxy), MEMF_CLEAR, Gfx_SysBaseV0);
    struct BitMap *nativefriendbm = NULL;
    ULONG plane;

    if (friend_bitmap != NULL)
    {
        nativefriendbm = ((struct BitMapProxy *)friend_bitmap)->native;
    }

    bmproxy->native = AllocBitMap(sizex, sizey, depth, flags, nativefriendbm);
    bmproxy->base.BytesPerRow   = bmproxy->native->BytesPerRow;
    bmproxy->base.Rows          = bmproxy->native->Rows;
    bmproxy->base.Flags         = bmproxy->native->Flags;
    bmproxy->base.Depth         = bmproxy->native->Depth;
    bmproxy->base.pad           = bmproxy->native->pad;

    for (plane=0; plane < bmproxy->native->Depth; plane++)
    {
        /* AllocRaster allocates MEMF_CHIP which is 24-bit memory so can be used on 32-bit side */
        bmproxy->base.Planes[plane] = (APTR32)(IPTR)bmproxy->native->Planes[plane];
    }

    return (struct BitMapV0 *)bmproxy;
}
MAKE_PROXY_ARG_6(AllocBitMap)

void  abiv0_FreeBitMap(struct BitMapV0 *bm, struct GfxBaseV0 *GfxBaseV0)
{
    struct BitMapProxy *bmproxy = (struct BitMapProxy *)bm;

    if (!bm) return;

    FreeBitMap(bmproxy->native);
    abiv0_FreeMem(bm, sizeof(struct BitMapProxy), Gfx_SysBaseV0);
}
MAKE_PROXY_ARG_2(FreeBitMap)

void abiv0_BltBitMapRastPort(struct BitMapV0 *srcBitMap, LONG xSrc, LONG ySrc, struct RastPortV0 *destRP,
    LONG xDest, LONG yDest, LONG xSize, LONG ySize, ULONG minterm, struct GfxBaseV0 *GfxBaseV0)
{
    struct BitMapProxy *bmproxy = (struct BitMapProxy *)srcBitMap;
    struct RastPort *rpnative = RastPortV0_getnative(destRP);
    struct RastPort rptmp;

    /* dtpic.mui uses locally created RastPort */
    if (rpnative == NULL)
    {
        InitRastPort(&rptmp);
        rptmp.FgPen     = destRP->FgPen;
        rptmp.BgPen     = destRP->BgPen;
        rptmp.DrawMode  = destRP->DrawMode;
        rptmp.linpatcnt = destRP->linpatcnt;
        rptmp.Flags     = destRP->Flags;
        rptmp.cp_x      = destRP->cp_x;
        rptmp.cp_y      = destRP->cp_y;

        rptmp.BitMap = ((struct BitMapProxy *)(IPTR)destRP->BitMap)->native;

        rpnative = &rptmp;
    }

    BltBitMapRastPort(bmproxy->native, xSrc, ySrc, rpnative, xDest, yDest, xSize, ySize, minterm);
}
MAKE_PROXY_ARG_12(BltBitMapRastPort)

void abiv0_BltMaskBitMapRastPort(struct BitMapV0 *srcBitMap, LONG xSrc, LONG ySrc, struct RastPortV0 * destRP,
    LONG xDest, LONG yDest, LONG xSize, LONG ySize, ULONG minterm, PLANEPTR bltMask, struct GfxBaseV0 *GfxBaseV0)
{
    struct BitMapProxy *bmproxy = (struct BitMapProxy *)srcBitMap;
    struct RastPort *rpnative = RastPortV0_getnative(destRP);
    struct RastPort rptmp;

    // /* dtpic.mui uses locally created RastPort */
    // if (rpnative == NULL)
    // {
    //     InitRastPort(&rptmp);
    //     rptmp.FgPen     = destRP->FgPen;
    //     rptmp.BgPen     = destRP->BgPen;
    //     rptmp.DrawMode  = destRP->DrawMode;
    //     rptmp.linpatcnt = destRP->linpatcnt;
    //     rptmp.Flags     = destRP->Flags;
    //     rptmp.cp_x      = destRP->cp_x;
    //     rptmp.cp_y      = destRP->cp_y;

    //     rptmp.BitMap = ((struct BitMapProxy *)(IPTR)destRP->BitMap)->native;

    //     rpnative = &rptmp;
    // }

    BltMaskBitMapRastPort(bmproxy->native, xSrc, ySrc, rpnative, xDest, yDest, xSize, ySize, minterm, bltMask);
}
MAKE_PROXY_ARG_12(BltMaskBitMapRastPort)

LONG abiv0_BltBitMap(struct BitMapV0 * srcBitMap, LONG xSrc, LONG ySrc, struct BitMapV0 *destBitMap, LONG xDest,
    LONG yDest, LONG xSize, LONG ySize, ULONG minterm, ULONG mask, PLANEPTR tempA, struct GfxBaseV0 *GfxBaseV0)
{
    struct BitMapProxy *srcproxy = (struct BitMapProxy *)srcBitMap;
    struct BitMapProxy *destproxy = (struct BitMapProxy *)destBitMap;

    return BltBitMap(srcproxy->native, xSrc, ySrc, destproxy->native, xDest, yDest, xSize, ySize, minterm, mask, tempA);
}
MAKE_PROXY_ARG_12(BltBitMap)

void abiv0_BltPattern(struct RastPortV0 *rp, PLANEPTR mask, LONG xMin, LONG yMin, LONG xMax, LONG yMax, ULONG byteCnt,
    struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);

    synchronize_SetAfPt(rp, rpnative);
    BltPattern(rpnative, mask, xMin, yMin, xMax, yMax, byteCnt);
}
MAKE_PROXY_ARG_12(BltPattern)

void abiv0_ScrollRaster(struct RastPortV0 *rp, LONG dx, LONG dy, LONG xMin, LONG yMin, LONG xMax, LONG yMax, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    ScrollRaster(rpnative, dx, dy, xMin, yMin, xMax,yMax);
}
MAKE_PROXY_ARG_12(ScrollRaster)

void abiv0_WriteChunkyPixels(struct RastPortV0 *rp, LONG xstart, LONG ystart, LONG xstop, LONG ystop, UBYTE *array, LONG bytesperrow,
    struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    struct RastPort rptmp;

    /* RNOTunes uses locally created RastPort */
    if (rpnative == NULL)
    {
        InitRastPort(&rptmp);
        rptmp.FgPen     = rp->FgPen;
        rptmp.BgPen     = rp->BgPen;
        rptmp.DrawMode  = rp->DrawMode;
        rptmp.linpatcnt = rp->linpatcnt;
        rptmp.Flags     = rp->Flags;
        rptmp.cp_x      = rp->cp_x;
        rptmp.cp_y      = rp->cp_y;

        rptmp.BitMap = ((struct BitMapProxy *)(IPTR)rp->BitMap)->native;

        rpnative = &rptmp;
    }
    WriteChunkyPixels(rpnative, xstart, ystart, xstop, ystop, array, bytesperrow);
}
MAKE_PROXY_ARG_12(WriteChunkyPixels)

struct LibraryV0 *shallow_InitResident32(struct ResidentV0 *resident, BPTR segList, struct ExecBaseV0 *SysBaseV0);
BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);
struct ResidentV0 * findResident(BPTR seg, CONST_STRPTR name);
APTR abiv0_DOS_OpenLibrary(CONST_STRPTR name, ULONG version, struct ExecBaseV0 *SysBaseV0);

#define TFE_HASHTABSIZE   	16

struct GfxBaseV0_intern
{
    struct GfxBaseV0 	 	gfxbase;

    ULONG			displays;	     /* Number of display drivers installed in the system	 */
    BYTE	shared_driverdata[260];   /* Driver data shared between all monitors (allocated once) */
    struct SignalSemaphoreV0	monitors_sema;	     /* Monitor list semaphore					 */
    struct SignalSemaphoreV0	hashtab_sema;	     /* hash_table arbitration semaphore			 */
    struct SignalSemaphoreV0	view_sema;	     /* ActiView arbitration semaphore				 */

    /* TextFontExtension pool */
    APTR32 tfe_hashtab[TFE_HASHTABSIZE];
    struct SignalSemaphoreV0  	tfe_hashtab_sema;
    struct SignalSemaphoreV0  	fontsem;
};

void init_graphics(struct ExecBaseV0 *SysBaseV0)
{
    TEXT path[64];
    NewRawDoFmt("LIBSV0:partial/graphics.library", RAWFMTFUNC_STRING, path);
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
    abiv0GfxBase->DefaultFont = (APTR32)(IPTR)makeTextFontV0(GfxBase->DefaultFont, SysBaseV0);

    struct GfxBaseV0_intern *abiv0IntGfxBase = (struct GfxBaseV0_intern *)abiv0GfxBase;
    abiv0_InitSemaphore(&abiv0IntGfxBase->fontsem, SysBaseV0);
    abiv0_InitSemaphore(&abiv0IntGfxBase->tfe_hashtab_sema, SysBaseV0);

    /* Set all LVO addresses to their number so that code jumps to "number" of the LVO and crashes */
    for (LONG i = 5; i <= 201; i++)
        __AROS_SETVECADDRV0(abiv0GfxBase,   i, (APTR32)(IPTR)i + 200 + 300);

    /* Set all working LVOs */
    __AROS_SETVECADDRV0(abiv0GfxBase,   1, (APTR32)(IPTR)proxy_Gfx_OpenLib);
    __AROS_SETVECADDRV0(abiv0GfxBase,  12, (APTR32)(IPTR)proxy_OpenFont);
    __AROS_SETVECADDRV0(abiv0GfxBase, 160, (APTR32)(IPTR)proxy_GetBitMapAttr);
    __AROS_SETVECADDRV0(abiv0GfxBase, 150, (APTR32)(IPTR)proxy_GetRGB32);
    __AROS_SETVECADDRV0(abiv0GfxBase, 140, (APTR32)(IPTR)proxy_ObtainBestPenA);
    __AROS_SETVECADDRV0(abiv0GfxBase, 134, graphicsjmp[202 - 134]);  // WeighTAMatch
    __AROS_SETVECADDRV0(abiv0GfxBase, 136, graphicsjmp[202 - 136]);  // ExtendFont
    __AROS_SETVECADDRV0(abiv0GfxBase,  80, graphicsjmp[202 -  80]);  // AddFont
    __AROS_SETVECADDRV0(abiv0GfxBase,   9, graphicsjmp[202 -   9]);  // TextLength
    __AROS_SETVECADDRV0(abiv0GfxBase,  51, (APTR32)(IPTR)proxy_RectFill);
    __AROS_SETVECADDRV0(abiv0GfxBase,  40, (APTR32)(IPTR)proxy_Move);
    __AROS_SETVECADDRV0(abiv0GfxBase,  41, (APTR32)(IPTR)proxy_Draw);
    __AROS_SETVECADDRV0(abiv0GfxBase,  10, (APTR32)(IPTR)proxy_Text);
    __AROS_SETVECADDRV0(abiv0GfxBase,  54, (APTR32)(IPTR)proxy_WritePixel);
    __AROS_SETVECADDRV0(abiv0GfxBase, 158, (APTR32)(IPTR)proxy_ReleasePen);
    __AROS_SETVECADDRV0(abiv0GfxBase,  13, (APTR32)(IPTR)proxy_CloseFont);
    __AROS_SETVECADDRV0(abiv0GfxBase, 170, (APTR32)(IPTR)proxy_AllocSpriteDataA);
    __AROS_SETVECADDRV0(abiv0GfxBase, 127, graphicsjmp[202 - 127]);  // FontExtent
    __AROS_SETVECADDRV0(abiv0GfxBase, 115, graphicsjmp[202 - 115]);  // TextExtent
    __AROS_SETVECADDRV0(abiv0GfxBase, 116, graphicsjmp[202 - 116]);  // TextFit
    __AROS_SETVECADDRV0(abiv0GfxBase, 132, (APTR32)(IPTR)proxy_GetVPModeID);
    __AROS_SETVECADDRV0(abiv0GfxBase, 126, (APTR32)(IPTR)proxy_GetDisplayInfoData);
    __AROS_SETVECADDRV0(abiv0GfxBase,  65, graphicsjmp[202 -  65]);  // InitBitmap
    __AROS_SETVECADDRV0(abiv0GfxBase,   6, (APTR32)(IPTR)proxy_BltTemplate);
    __AROS_SETVECADDRV0(abiv0GfxBase,  38, graphicsjmp[202 -  38]);  // WaitBlit
    __AROS_SETVECADDRV0(abiv0GfxBase,  45, (APTR32)(IPTR)proxy_WaitTOF);
    __AROS_SETVECADDRV0(abiv0GfxBase, 153, (APTR32)(IPTR)proxy_AllocBitMap);
    __AROS_SETVECADDRV0(abiv0GfxBase, 101, (APTR32)(IPTR)proxy_BltBitMapRastPort);
    __AROS_SETVECADDRV0(abiv0GfxBase, 154, (APTR32)(IPTR)proxy_FreeBitMap);
    __AROS_SETVECADDRV0(abiv0GfxBase, 129, graphicsjmp[202 - 129]);  // WritePixelLine8
    __AROS_SETVECADDRV0(abiv0GfxBase, 131, (APTR32)(IPTR)proxy_WritePixelArray8);
    __AROS_SETVECADDRV0(abiv0GfxBase, 159, (APTR32)(IPTR)proxy_ObtainPen);
    __AROS_SETVECADDRV0(abiv0GfxBase, 142, (APTR32)(IPTR)proxy_SetRGB32);
    __AROS_SETVECADDRV0(abiv0GfxBase, 122, (APTR32)(IPTR)proxy_NextDisplayInfo);
    __AROS_SETVECADDRV0(abiv0GfxBase, 121, (APTR32)(IPTR)proxy_FindDisplayInfo);
    __AROS_SETVECADDRV0(abiv0GfxBase,  82, graphicsjmp[202 -  82]);  // AllocRaster
    __AROS_SETVECADDRV0(abiv0GfxBase, 128, graphicsjmp[202 - 128]);  // ReadPixelLine8
    __AROS_SETVECADDRV0(abiv0GfxBase, 130, (APTR32)(IPTR)proxy_ReadPixelArray8);
    __AROS_SETVECADDRV0(abiv0GfxBase, 106, (APTR32)(IPTR)proxy_BltMaskBitMapRastPort);
    __AROS_SETVECADDRV0(abiv0GfxBase,  83, graphicsjmp[202 -  83]);  // FreeRaster
    __AROS_SETVECADDRV0(abiv0GfxBase,   5, (APTR32)(IPTR)proxy_BltBitMap);
    __AROS_SETVECADDRV0(abiv0GfxBase,  66, (APTR32)(IPTR)proxy_ScrollRaster);
    __AROS_SETVECADDRV0(abiv0GfxBase, 193, graphicsjmp[202 - 193]);  // AndRectRect
    __AROS_SETVECADDRV0(abiv0GfxBase, 176, (APTR32)(IPTR)proxy_WriteChunkyPixels);
    __AROS_SETVECADDRV0(abiv0GfxBase,  78, graphicsjmp[202 -  78]);  // InitTmpRas
    __AROS_SETVECADDRV0(abiv0GfxBase,  47, graphicsjmp[202 -  47]);  // InitArea
    __AROS_SETVECADDRV0(abiv0GfxBase,  42, graphicsjmp[202 -  42]);  // AreaMove
    __AROS_SETVECADDRV0(abiv0GfxBase,  43, graphicsjmp[202 -  43]);  // AreaDraw
    __AROS_SETVECADDRV0(abiv0GfxBase,  44, graphicsjmp[202 -  44]);  // AreaEnd
    __AROS_SETVECADDRV0(abiv0GfxBase,  52, (APTR32)(IPTR)proxy_BltPattern);

    Graphics_Rastports_init(abiv0GfxBase, graphicsjmp);
    Graphics_Regions_init(abiv0GfxBase);
}
