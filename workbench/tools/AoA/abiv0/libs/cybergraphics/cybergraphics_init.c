/*
    Copyright (C) 2024-2026, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/cybergraphics.h>
#include <proto/graphics.h>
#include <aros/debug.h>
#include <exec/rawfmt.h>

#include <string.h>

#include "../../include/exec/structures.h"
#include "../../include/exec/functions.h"
#include "../../include/aros/cpu.h"
#include "../../include/aros/proxy.h"
#include "../../include/aros/call32.h"
#include "../../include/graphics/structures.h"
#include "../../include/graphics/proxy_structures.h"
#include "../../include/utility/structures.h"

#include "../exec/exec_libraries.h"
#include "../graphics/graphics_rastports.h"

#include "../../support.h"

struct ExecBaseV0 *CyberGfx_SysBaseV0;

BPTR abiv0_CyberGfx_ExpungeLib(struct LibraryV0 *extralhV0, struct LibraryV0 *CyberGfxBaseV0)
{
    /* Call Remove on library base */
    CALL32_ARG_2_NR(__AROS_GETVECADDRV0(CyberGfx_SysBaseV0, 42), CyberGfxBaseV0, (APTR32)(IPTR)CyberGfx_SysBaseV0);
    return BNULL;
}
MAKE_PROXY_ARG_2(CyberGfx_ExpungeLib)

ULONG abiv0_FillPixelArray(struct RastPortV0 *rp, UWORD destx, UWORD desty, UWORD width, UWORD height, ULONG pixel)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    return FillPixelArray(rpnative, destx, desty, width, height, pixel);
}
MAKE_PROXY_ARG_6(FillPixelArray)

ULONG abiv0_WritePixelArrayAlpha(APTR src, UWORD srcx, UWORD srcy, UWORD srcmod, struct RastPortV0 *rp,
    UWORD destx, UWORD desty, UWORD width, UWORD height, ULONG globalalpha, struct LibraryV0 *CyberGfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    ULONG _ret;
    BITMAPLAYERPRE

    if (rpnative->BitMap == NULL)
    {
        /* RNOTunes uses locally created RastPort */
        recreateNativeRastPortBitMap(rp, rpnative, &bmtmp);
        clearBM = TRUE;
    }

    if (rpnative->Layer == NULL && rp->Layer != (APTR32)(IPTR)NULL)
    {
        /* RNOTunes uses locally created RastPort */
        rpnative->Layer = ((struct LayerProxy *)(IPTR)rp->Layer)->native;
        clearL = TRUE;
    }

    _ret = WritePixelArrayAlpha(src, srcx, srcy, srcmod, rpnative, destx, desty, width, height, globalalpha);

    BITMAPLAYERPOST

    return _ret;
}
MAKE_PROXY_ARG_12(WritePixelArrayAlpha)

LONG abiv0_WriteLUTPixelArray(APTR srcRect, UWORD SrcX, UWORD SrcY, UWORD SrcMod, struct RastPortV0 *rp,
    APTR CTable, UWORD DestX, UWORD DestY, UWORD SizeX, UWORD SizeY, UBYTE CTabFormat, struct LibraryV0 *CyberGfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    LONG _ret;
    BITMAPLAYERPRE

    if (rpnative->BitMap == NULL)
    {
        /* RNOTunes when showing AboutMUI */
        recreateNativeRastPortBitMap(rp, rpnative, &bmtmp);
        clearBM = TRUE;
    }

    _ret = WriteLUTPixelArray(srcRect, SrcX, SrcY, SrcMod, rpnative, CTable, DestX, DestY, SizeX, SizeY, CTabFormat);

    BITMAPLAYERPOST

    return _ret;
}
MAKE_PROXY_ARG_12(WriteLUTPixelArray)

ULONG abiv0_WritePixelArray(APTR src, UWORD srcx, UWORD srcy, UWORD srcmod, struct RastPortV0 *rp,
    UWORD destx, UWORD desty, UWORD width, UWORD height, UBYTE srcformat, struct LibraryV0 *CyberGfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    ULONG _ret;
    BITMAPLAYERPRE

    if (rpnative == NULL)
    {
        /* Crossboard_Live drawing on custom public screen */
        rpnative = RastPortV0_createcompanion(rp);
    }

    if (rpnative->BitMap == NULL)
    {
        /* Soliton operates on locally created RastPort */
        /* picture.datatype uses locally created RastPort */
        recreateNativeRastPortBitMap(rp, rpnative, &bmtmp);
        clearBM = TRUE;
    }

    _ret = WritePixelArray(src, srcx, srcy, srcmod, rpnative, destx, desty, width, height, srcformat);

    BITMAPLAYERPOST

    return _ret;
}
MAKE_PROXY_ARG_12(WritePixelArray)

LONG abiv0_WriteRGBPixel(struct RastPortV0 *rp, UWORD x, UWORD y, ULONG pixel, struct LibraryV0 *CyberGfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    LONG _ret;

    _ret = WriteRGBPixel(rpnative, x, y, pixel);

    return _ret;
}
MAKE_PROXY_ARG_5(WriteRGBPixel)

ULONG abiv0_ReadPixelArray(APTR dst, UWORD dstx, UWORD dsty, UWORD dstmod, struct RastPortV0 *rp,
    UWORD srcx, UWORD srcy, UWORD width, UWORD height, UBYTE dstformat, struct LibraryV0 *CyberGfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    return ReadPixelArray(dst, dstx, dsty, dstmod, rpnative, srcx, srcy, width, height, dstformat);
}
MAKE_PROXY_ARG_12(ReadPixelArray)

VOID abiv0_ProcessPixelArray(struct RastPortV0 *rp, ULONG destX, ULONG destY, ULONG sizeX, ULONG sizeY, ULONG operation,
        LONG value, struct TagItemV0 *taglist, struct LibraryV0 *CyberGfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    BITMAPLAYERPRE

    if (rpnative->BitMap == NULL)
    {
        /* dtpic.mui uses locally created RastPort */
        recreateNativeRastPortBitMap(rp, rpnative, &bmtmp);
        clearBM = TRUE;
    }

    ProcessPixelArray(rpnative, destX, destY, sizeX, sizeY, operation, value, NULL);

    BITMAPLAYERPOST
}
MAKE_PROXY_ARG_12(ProcessPixelArray)

BOOL abiv0_IsCyberModeID(ULONG modeID, struct LibraryV0 *CyberGfxBaseV0)
{
    return IsCyberModeID(modeID);
}
MAKE_PROXY_ARG_2(IsCyberModeID)

ULONG abiv0_BestCModeIDTagList(struct TagItemV0 * tags, struct LibraryV0 *CyberGfxBaseV0)
{
    ULONG _ret;

    struct TagItem *tagListNative = CloneTagItemsV02Native(tags);

    struct TagItem *tagNative = tagListNative;

    _ret = BestCModeIDTagList(tagListNative);

    FreeClonedV02NativeTagItems(tagListNative);

    return _ret;
}
MAKE_PROXY_ARG_2(BestCModeIDTagList)

ULONG abiv0_GetCyberIDAttr(ULONG attribute, ULONG DisplayModeID, struct LibraryV0 *CyberGfxBaseV0)
{
    return GetCyberIDAttr(attribute, DisplayModeID);
}
MAKE_PROXY_ARG_3(GetCyberIDAttr)

ULONG abiv0_GetCyberMapAttr(struct BitMapV0 *bitMap, ULONG attribute, struct LibraryV0 *CyberGfxBaseV0)
{
    struct BitMapProxy *proxy = (struct BitMapProxy *)bitMap;
    return GetCyberMapAttr(proxy->native, attribute);
}
MAKE_PROXY_ARG_3(GetCyberMapAttr)

void CyberGfx_Unhandled_init(struct LibraryV0 *abiv0CyberGfxBase);
struct LibraryV0 *shallow_InitResident32(struct ResidentV0 *resident, BPTR segList, struct ExecBaseV0 *SysBaseV0);
BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);

static BPTR cgfxseg;

void init_cybergraphics(struct ExecBaseV0 *SysBaseV0)
{
    TEXT path[64];

    NewRawDoFmt("LIBSV0:partial/cybergraphics.library", RAWFMTFUNC_STRING, path);
    cgfxseg = LoadSeg32(path, DOSBase);
    struct ResidentV0 *cgfxres = findResident(cgfxseg, NULL);
    struct LibraryV0 *abiv0CyberGfxBase = shallow_InitResident32(cgfxres, cgfxseg, SysBaseV0);
    CyberGfx_SysBaseV0 = SysBaseV0;
    /* Remove all vectors for now (leave LibOpen/LibClose) */
    const ULONG cybergraphicsjmpsize = 38 * sizeof(APTR32);
    APTR32 *cybergraphicsjmp = AllocMem(cybergraphicsjmpsize, MEMF_CLEAR);
    CopyMem((APTR)abiv0CyberGfxBase - cybergraphicsjmpsize, cybergraphicsjmp, cybergraphicsjmpsize);
    for (int i = 3; i <= 38; i++) __AROS_SETVECADDRV0(abiv0CyberGfxBase, i, 0);
    /* Set all unhandled LVO addresses to a catch function */
    CyberGfx_Unhandled_init((struct LibraryV0 *)abiv0CyberGfxBase);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  3, (APTR32)(IPTR)proxy_CyberGfx_ExpungeLib);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase, 25, (APTR32)(IPTR)proxy_FillPixelArray);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase, 36, (APTR32)(IPTR)proxy_WritePixelArrayAlpha);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase, 33, (APTR32)(IPTR)proxy_WriteLUTPixelArray);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase, 21, (APTR32)(IPTR)proxy_WritePixelArray);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase, 38, (APTR32)(IPTR)proxy_ProcessPixelArray);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  9, (APTR32)(IPTR)proxy_IsCyberModeID);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase, 10, (APTR32)(IPTR)proxy_BestCModeIDTagList);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase, 17, (APTR32)(IPTR)proxy_GetCyberIDAttr);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase, 16, (APTR32)(IPTR)proxy_GetCyberMapAttr);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase, 20, (APTR32)(IPTR)proxy_ReadPixelArray);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase, 19, (APTR32)(IPTR)proxy_WriteRGBPixel);
}

void exit_cybergraphics()
{
    UnLoadSeg(cgfxseg);
}
