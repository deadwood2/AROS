/*
    Copyright (C) 2026, The AROS Development Team. All rights reserved.
*/

#include <proto/graphics.h>

#include "../include/aros/cpu.h"
#include "../include/aros/proxy.h"
#include "../include/graphics/structures.h"
#include "../include/graphics/proxy_structures.h"

#include "graphics_rastports.h"

static void recreteNativeRastPortPlanarBitMap(struct RastPortV0 *rpv0, struct RastPort *rpnative, struct BitMap *bmtmp)
{
    struct BitMapV0 *bmv0 = (struct BitMapV0 *)(IPTR)rpv0->BitMap;

    bmtmp->BytesPerRow  = bmv0->BytesPerRow;
    bmtmp->Depth        = bmv0->Depth;
    bmtmp->Flags        = bmv0->Flags;
    bmtmp->Rows         = bmv0->Rows;

    for (LONG i = 0; i < bmv0->Depth; i++)
        bmtmp->Planes[i]= (APTR)(IPTR)bmv0->Planes[i];

    rpnative->BitMap    = bmtmp;
}

void abiv0_RectFill(struct RastPortV0 * rp, LONG xMin, LONG yMin, LONG xMax, LONG yMax, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    struct BitMap bmtmp;

    if (rpnative->BitMap == NULL)
    {
         /* HFinder operates on locally created 1-bit RastPort/BitMap */
        recreteNativeRastPortPlanarBitMap(rp, rpnative, &bmtmp);
    }

    synchronize_SetAfPt(rp, rpnative);
    RectFill(rpnative, xMin, yMin, xMax, yMax);

    if (rpnative->BitMap == &bmtmp) rpnative->BitMap = NULL;
}
MAKE_PROXY_ARG_6(RectFill)

void abiv0_Move(struct RastPortV0 *rp, WORD x, WORD y, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    Move(rpnative, x, y);
}
MAKE_PROXY_ARG_4(Move)

void abiv0_Draw(struct RastPortV0 *rp, WORD x, WORD y, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    BOOL clear = FALSE;

    if (rpnative->BitMap == NULL)
    {
        /* RNOTunes uses locally created RastPort */
        rpnative->BitMap = ((struct BitMapProxy *)(IPTR)rp->BitMap)->native;
        rpnative->Layer = ((struct LayerProxy *)(IPTR)rp->Layer)->native;
        clear = TRUE;
    }

    Draw(rpnative, x, y);

    if (clear)
    {
        rpnative->BitMap = NULL;
        rpnative->Layer = NULL;
    }
}
MAKE_PROXY_ARG_4(Draw)

void abiv0_Text(struct RastPortV0 *rp, CONST_STRPTR string, ULONG count, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    struct BitMap bmtmp;

    if (rpnative->BitMap == NULL)
    {
         /* HFinder operates on locally created 1-bit RastPort/BitMap */
        recreteNativeRastPortPlanarBitMap(rp, rpnative, &bmtmp);
    }

    Text(rpnative, string, count);

    if (rpnative->BitMap == &bmtmp) rpnative->BitMap = NULL;
}
MAKE_PROXY_ARG_4(Text)

LONG abiv0_WritePixel(struct RastPortV0 *rp, LONG x, LONG y, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    return WritePixel(rpnative, x, y);
}
MAKE_PROXY_ARG_4(WritePixel)

void abiv0_BltTemplate(PLANEPTR source, LONG xSrc, LONG srcMod, struct RastPortV0 *destRP, LONG xDest, LONG yDest, LONG xSize,
    LONG ySize, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(destRP);
    struct BitMap bmtmp;

    if (rpnative->BitMap == NULL)
    {
         /* HFinder operates on locally created 1-bit RastPort/BitMap */
        recreteNativeRastPortPlanarBitMap(destRP, rpnative, &bmtmp);
    }

    BltTemplate(source, xSrc, srcMod, rpnative, xDest, yDest, xSize, ySize);

    if (rpnative->BitMap == &bmtmp) rpnative->BitMap = NULL;
}
MAKE_PROXY_ARG_12(BltTemplate)

LONG abiv0_WritePixelArray8(struct RastPortV0 *rp, ULONG xstart, ULONG ystart, ULONG xstop, ULONG ystop, UBYTE *array,
    struct RastPortV0 *temprp, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    struct BitMap bmtmp;
    LONG _ret;

    if (rpnative->BitMap == NULL)
    {
        /* HollyPaint operates on locally created RastPort/BitMap */
        recreteNativeRastPortPlanarBitMap(rp, rpnative, &bmtmp);
    }

    _ret = WritePixelArray8(rpnative, xstart, ystart, xstop, ystop, array, NULL);

    if (rpnative->BitMap == &bmtmp) rpnative->BitMap = NULL;

    return _ret;
}
MAKE_PROXY_ARG_12(WritePixelArray8)

LONG abiv0_ReadPixelArray8(struct RastPortV0 *rp, LONG xstart, LONG ystart, LONG xstop, LONG ystop, UBYTE *array,
    struct RastPortV0 *temprp, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    struct BitMap bmtmp;
    LONG _ret;

    if (rpnative->BitMap == NULL)
    {
        /* Soliton operates on locally created RastPort/BitMap */
        recreteNativeRastPortPlanarBitMap(rp, rpnative, &bmtmp);
    }

    _ret = ReadPixelArray8(rpnative, xstart, ystart, xstop, ystop, array, NULL);

    if (rpnative->BitMap == &bmtmp) rpnative->BitMap = NULL;

    return _ret;
}
MAKE_PROXY_ARG_12(ReadPixelArray8)

void abiv0_BltBitMapRastPort(struct BitMapV0 *srcBitMap, LONG xSrc, LONG ySrc, struct RastPortV0 *destRP,
    LONG xDest, LONG yDest, LONG xSize, LONG ySize, ULONG minterm, struct GfxBaseV0 *GfxBaseV0)
{
    struct BitMapProxy *bmproxy = (struct BitMapProxy *)srcBitMap;
    struct RastPort *rpnative = RastPortV0_getnative(destRP);
    BOOL clear = FALSE;

    if (rpnative->BitMap == NULL)
    {
        /* Soliton operates on locally created RastPort */
        /* dtpic.mui uses locally created RastPort */
        rpnative->BitMap = ((struct BitMapProxy *)(IPTR)destRP->BitMap)->native;
        clear = TRUE;
    }

    BltBitMapRastPort(bmproxy->native, xSrc, ySrc, rpnative, xDest, yDest, xSize, ySize, minterm);

    if (clear) rpnative->BitMap = NULL;
}
MAKE_PROXY_ARG_12(BltBitMapRastPort)

void abiv0_BltMaskBitMapRastPort(struct BitMapV0 *srcBitMap, LONG xSrc, LONG ySrc, struct RastPortV0 * destRP,
    LONG xDest, LONG yDest, LONG xSize, LONG ySize, ULONG minterm, PLANEPTR bltMask, struct GfxBaseV0 *GfxBaseV0)
{
    struct BitMapProxy *bmproxy = (struct BitMapProxy *)srcBitMap;
    struct RastPort *rpnative = RastPortV0_getnative(destRP);

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

void abiv0_WriteChunkyPixels(struct RastPortV0 *rp, LONG xstart, LONG ystart, LONG xstop, LONG ystop, UBYTE *array, LONG bytesperrow,
    struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
#if 0
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
#endif
    WriteChunkyPixels(rpnative, xstart, ystart, xstop, ystop, array, bytesperrow);
}
MAKE_PROXY_ARG_12(WriteChunkyPixels)

void Graphics_Operations_init(struct GfxBaseV0 *abiv0GfxBase, APTR32 *graphicsjmp)
{
    __AROS_SETVECADDRV0(abiv0GfxBase,  51, (APTR32)(IPTR)proxy_RectFill);
    __AROS_SETVECADDRV0(abiv0GfxBase,  40, (APTR32)(IPTR)proxy_Move);
    __AROS_SETVECADDRV0(abiv0GfxBase,  41, (APTR32)(IPTR)proxy_Draw);
    __AROS_SETVECADDRV0(abiv0GfxBase,  10, (APTR32)(IPTR)proxy_Text);
    __AROS_SETVECADDRV0(abiv0GfxBase,  54, (APTR32)(IPTR)proxy_WritePixel);
    __AROS_SETVECADDRV0(abiv0GfxBase,   6, (APTR32)(IPTR)proxy_BltTemplate);
    __AROS_SETVECADDRV0(abiv0GfxBase, 101, (APTR32)(IPTR)proxy_BltBitMapRastPort);
    __AROS_SETVECADDRV0(abiv0GfxBase, 129, graphicsjmp[202 - 129]);  // WritePixelLine8
    __AROS_SETVECADDRV0(abiv0GfxBase, 131, (APTR32)(IPTR)proxy_WritePixelArray8);
    __AROS_SETVECADDRV0(abiv0GfxBase, 128, graphicsjmp[202 - 128]);  // ReadPixelLine8
    __AROS_SETVECADDRV0(abiv0GfxBase, 130, (APTR32)(IPTR)proxy_ReadPixelArray8);
    __AROS_SETVECADDRV0(abiv0GfxBase, 106, (APTR32)(IPTR)proxy_BltMaskBitMapRastPort);
    __AROS_SETVECADDRV0(abiv0GfxBase,   5, (APTR32)(IPTR)proxy_BltBitMap);
    __AROS_SETVECADDRV0(abiv0GfxBase, 176, (APTR32)(IPTR)proxy_WriteChunkyPixels);
    __AROS_SETVECADDRV0(abiv0GfxBase,  47, graphicsjmp[202 -  47]);  // InitArea
    __AROS_SETVECADDRV0(abiv0GfxBase,  42, graphicsjmp[202 -  42]);  // AreaMove
    __AROS_SETVECADDRV0(abiv0GfxBase,  43, graphicsjmp[202 -  43]);  // AreaDraw
    __AROS_SETVECADDRV0(abiv0GfxBase,  44, graphicsjmp[202 -  44]);  // AreaEnd
    __AROS_SETVECADDRV0(abiv0GfxBase,  52, (APTR32)(IPTR)proxy_BltPattern);
}
