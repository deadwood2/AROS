/*
    Copyright (C) 2025-2026, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <graphics/gfxbase.h>
#include <graphics/videocontrol.h>
#include <graphics/displayinfo.h>
#include <proto/graphics.h>
#include <proto/alib.h>
#include <aros/debug.h>
#include <exec/rawfmt.h>

#include <string.h>


#include "../include/exec/structures.h"
#include "../include/exec/functions.h"
#include "../include/aros/cpu.h"
#include "../include/aros/proxy.h"
#include "../include/aros/call32.h"
#include "../include/graphics/structures.h"
#include "../include/graphics/proxy_structures.h"
#include "../include/utility/structures.h"

#include "../exec/exec_libraries.h"
#include "graphics_operations.h"
#include "graphics_rastports.h"
#include "graphics_regions.h"

struct ExecBaseV0 *Gfx_SysBaseV0;

void syncLayerV0(struct LayerProxy *proxy)
{
    proxy->base.Flags               = proxy->native->Flags;
}

struct LibraryV0 *abiv0_Gfx_OpenLib(ULONG version, struct LibraryV0 *GfxBaseV0)
{
    GfxBaseV0->lib_OpenCnt++;
    return GfxBaseV0;
}
MAKE_PROXY_ARG_2(Gfx_OpenLib)

BPTR abiv0_Gfx_CloseLib(struct LibraryV0 *GfxBaseV0)
{
    GfxBaseV0->lib_OpenCnt--;
    return BNULL;
}
MAKE_PROXY_ARG_1(Gfx_CloseLib)

BPTR abiv0_Gfx_ExpungeLib(struct LibraryV0 *extralhV0, struct LibraryV0 *GfxBaseV0)
{
    abiv0_CloseLibrary((struct LibraryV0 *)(IPTR)(*(ULONG *)((IPTR)GfxBaseV0 + 0x4b0)), Gfx_SysBaseV0);

    /* Call Remove on library base */
    CALL32_ARG_2_NR(__AROS_GETVECADDRV0(Gfx_SysBaseV0, 42), GfxBaseV0, (APTR32)(IPTR)Gfx_SysBaseV0);
    return BNULL;
}
MAKE_PROXY_ARG_2(Gfx_ExpungeLib)

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
struct TagItemV0 *LibNextTagItemV0(struct TagItemV0 **tagListPtr);

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

ULONG abiv0_GetVPModeID(struct ViewPortV0 * vp, struct GfxBaseV0 *GfxBaseV0)
{
    struct ViewPort *vpnative = (struct ViewPort *)*(IPTR *)&vp->DspIns;
    return GetVPModeID(vpnative);
}
MAKE_PROXY_ARG_2(GetVPModeID)

VOID abiv0_WaitTOF(struct GfxBaseV0 *GfxBaseV0)
{
    WaitTOF();
}
MAKE_PROXY_ARG_1(WaitTOF)

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

struct DisplayInfoHandleProxy
{
    DisplayInfoHandle native;
};

struct DimensionInfoV0
{
    struct QueryHeader Header;

    UWORD MaxDepth;
    UWORD MinRasterWidth;
    UWORD MinRasterHeight;
    UWORD MaxRasterWidth;
    UWORD MaxRasterHeight;

    struct Rectangle Nominal;
    struct Rectangle MaxOScan;
    struct Rectangle VideoOScan;
    struct Rectangle TxtOScan;
    struct Rectangle StdOScan;

    UBYTE pad[14];
    APTR32 reserved[2];
};

struct MonitorInfoV0
{
    struct QueryHeader Header;

    APTR32               Mspc;
    Point                ViewPosition;
    Point                ViewResolution;
    struct Rectangle     ViewPositionRange;
    UWORD                TotalRows;
    UWORD                TotalColorClocks;
    UWORD                MinRow;
    WORD                 Compatibility;       /* see below */
    UBYTE                pad[32];
    Point                MouseTicks;
    Point                DefaultViewPosition;
    ULONG                PreferredModeID;
    APTR32               reserved[2];
};

DisplayInfoHandle abiv0_FindDisplayInfo(ULONG ID, struct GfxBaseV0 *GfxBaseV0)
{
    struct DisplayInfoHandleProxy *handle = NULL;

    DisplayInfoHandle native = FindDisplayInfo(ID);
    if (native != NULL)
    {
        handle = abiv0_AllocMem(sizeof(struct DisplayInfoHandleProxy), MEMF_CLEAR, Gfx_SysBaseV0); // MEMLEAK
        handle->native = native;
    }

    return handle;
}
MAKE_PROXY_ARG_2(FindDisplayInfo)

ULONG abiv0_GetDisplayInfoData(APTR32 handle, UBYTE *buf, ULONG size, ULONG tagID, ULONG ID, struct GfxBaseV0 *GfxBaseV0)
{
    struct DisplayInfoHandleProxy *hp = (struct DisplayInfoHandleProxy *)(IPTR)handle;
    APTR hnative = hp != NULL ? hp->native : NULL;

    switch (tagID)
    {
        case (DTAG_DIMS):
        {
            ULONG lsize = sizeof(struct DimensionInfo) * 2;
            UBYTE *lbuf = AllocMem(lsize, MEMF_ANY);
            ULONG _ret = GetDisplayInfoData(hnative, lbuf, lsize, tagID, ID);
            if (_ret > 0)
            {
                struct DimensionInfo *dinative = (struct DimensionInfo *)lbuf;
                struct DimensionInfoV0 *div0 = (struct DimensionInfoV0 *)buf;
                div0->Header.StructID       = dinative->Header.StructID;
                div0->Header.DisplayID      = dinative->Header.DisplayID;
                div0->Header.SkipID         = dinative->Header.SkipID;
                div0->Header.Length         = dinative->Header.Length;
                div0->MaxDepth              = dinative->MaxDepth;
                div0->MinRasterWidth        = dinative->MinRasterWidth;
                div0->MinRasterHeight       = dinative->MinRasterHeight;
                div0->MaxRasterWidth        = dinative->MaxRasterWidth;
                div0->MaxRasterHeight       = dinative->MaxRasterHeight;

                div0->Nominal               = dinative->Nominal;
                div0->MaxOScan              = dinative->MaxOScan;
                div0->VideoOScan            = dinative->VideoOScan;
                div0->TxtOScan              = dinative->TxtOScan;
                div0->StdOScan              = dinative->StdOScan;

                _ret = sizeof(struct DimensionInfoV0);
            }
            FreeMem(lbuf, lsize);
            return _ret;
        }
        case (DTAG_MNTR):
        {
            ULONG lsize = sizeof(struct MonitorInfo) * 2;
            UBYTE *lbuf = AllocMem(lsize, MEMF_ANY);
            ULONG _ret = GetDisplayInfoData(hnative, lbuf, lsize, tagID, ID);
            if (_ret > 0)
            {
                struct MonitorInfo *minative = (struct MonitorInfo *)lbuf;
                struct MonitorInfoV0 *miv0 = (struct MonitorInfoV0 *)buf;
                miv0->Header.StructID       = minative->Header.StructID;
                miv0->Header.DisplayID      = minative->Header.DisplayID;
                miv0->Header.SkipID         = minative->Header.SkipID;
                miv0->Header.Length         = minative->Header.Length;

bug("abiv0_GetDisplayInfoData: STUB\n");

                _ret = sizeof(struct MonitorInfoV0);
            }
            FreeMem(lbuf, lsize);
            return _ret;
        }
        default:
            break;
    }
bug("abiv0_GetDisplayInfoData: STUB\n");
asm("int3");
    return 0;
}
MAKE_PROXY_ARG_6(GetDisplayInfoData)

ULONG  abiv0_ModeNotAvailable(ULONG modeID, struct GfxBaseV0 *GfxBaseV0)
{
    return ModeNotAvailable(modeID);
}
MAKE_PROXY_ARG_2(ModeNotAvailable)

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

void abiv0_ScrollRaster(struct RastPortV0 *rp, LONG dx, LONG dy, LONG xMin, LONG yMin, LONG xMax, LONG yMax, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    ScrollRaster(rpnative, dx, dy, xMin, yMin, xMax,yMax);
}
MAKE_PROXY_ARG_12(ScrollRaster)

static struct ViewPortExtraV0 *_dummy;

ULONG abiv0_VideoControl(struct ColorMapV0 *cm, struct TagItemV0 *tags, struct GfxBaseV0 *GfxBaseV0)
{
    struct TagItemV0 *tmp;

    tmp = (struct TagItemV0 *)tags;
    do
    {
        switch(tmp->ti_Tag)
        {
            case (VTAG_VIEWPORTEXTRA_GET):
            {
                if (_dummy == NULL) _dummy = abiv0_AllocMem(sizeof(struct ViewPortExtraV0), MEMF_CLEAR, Gfx_SysBaseV0); // MEMLEAK
                struct ColorMapProxy *cmproxy = (struct ColorMapProxy *)cm;
                _dummy->DisplayClip.MinX = cmproxy->native->cm_vpe->DisplayClip.MinX;
                _dummy->DisplayClip.MaxX = cmproxy->native->cm_vpe->DisplayClip.MaxX;
                _dummy->DisplayClip.MinY = cmproxy->native->cm_vpe->DisplayClip.MinY;
                _dummy->DisplayClip.MaxY = cmproxy->native->cm_vpe->DisplayClip.MaxY;
                tmp->ti_Data = (APTR32)(IPTR)_dummy;
bug("abiv0_VideoControl: STUB\n");
                break;
            }
            case(TAG_DONE): break;
            default:
bug("abiv0_VideoControl unhandled tag %x\n", tmp->ti_Tag);
asm("int3");
        }
    } while (LibNextTagItemV0(&tmp) != NULL);

    return 0;
}
MAKE_PROXY_ARG_3(VideoControl)

struct LibraryV0 *shallow_InitResident32(struct ResidentV0 *resident, BPTR segList, struct ExecBaseV0 *SysBaseV0);
BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);
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

BPTR graphicsseg;

void Gfx_Unhandled_init(struct LibraryV0 *abiv0GfxBase);

void init_graphics(struct ExecBaseV0 *SysBaseV0)
{
    TEXT path[64];
    NewRawDoFmt("LIBSV0:partial/graphics.library", RAWFMTFUNC_STRING, path);
    graphicsseg = LoadSeg32(path, DOSBase);
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

    /* Set all unhandled LVO addresses to a catch function */
    Gfx_Unhandled_init((struct LibraryV0 *)abiv0GfxBase);

    /* Set all working LVOs */
    __AROS_SETVECADDRV0(abiv0GfxBase,   1, (APTR32)(IPTR)proxy_Gfx_OpenLib);
    __AROS_SETVECADDRV0(abiv0GfxBase,   2, (APTR32)(IPTR)proxy_Gfx_CloseLib);
    __AROS_SETVECADDRV0(abiv0GfxBase,   3, (APTR32)(IPTR)proxy_Gfx_ExpungeLib);
    __AROS_SETVECADDRV0(abiv0GfxBase,  12, (APTR32)(IPTR)proxy_OpenFont);
    __AROS_SETVECADDRV0(abiv0GfxBase, 160, (APTR32)(IPTR)proxy_GetBitMapAttr);
    __AROS_SETVECADDRV0(abiv0GfxBase, 150, (APTR32)(IPTR)proxy_GetRGB32);
    __AROS_SETVECADDRV0(abiv0GfxBase, 140, (APTR32)(IPTR)proxy_ObtainBestPenA);
    __AROS_SETVECADDRV0(abiv0GfxBase, 134, graphicsjmp[202 - 134]);  // WeighTAMatch
    __AROS_SETVECADDRV0(abiv0GfxBase, 136, graphicsjmp[202 - 136]);  // ExtendFont
    __AROS_SETVECADDRV0(abiv0GfxBase,  80, graphicsjmp[202 -  80]);  // AddFont
    __AROS_SETVECADDRV0(abiv0GfxBase,   9, graphicsjmp[202 -   9]);  // TextLength
    __AROS_SETVECADDRV0(abiv0GfxBase, 158, (APTR32)(IPTR)proxy_ReleasePen);
    __AROS_SETVECADDRV0(abiv0GfxBase,  13, (APTR32)(IPTR)proxy_CloseFont);
    __AROS_SETVECADDRV0(abiv0GfxBase, 170, (APTR32)(IPTR)proxy_AllocSpriteDataA);
    __AROS_SETVECADDRV0(abiv0GfxBase, 127, graphicsjmp[202 - 127]);  // FontExtent
    __AROS_SETVECADDRV0(abiv0GfxBase, 115, graphicsjmp[202 - 115]);  // TextExtent
    __AROS_SETVECADDRV0(abiv0GfxBase, 116, graphicsjmp[202 - 116]);  // TextFit
    __AROS_SETVECADDRV0(abiv0GfxBase, 132, (APTR32)(IPTR)proxy_GetVPModeID);
    __AROS_SETVECADDRV0(abiv0GfxBase, 126, (APTR32)(IPTR)proxy_GetDisplayInfoData);
    __AROS_SETVECADDRV0(abiv0GfxBase,  65, graphicsjmp[202 -  65]);  // InitBitmap
    __AROS_SETVECADDRV0(abiv0GfxBase,  38, graphicsjmp[202 -  38]);  // WaitBlit
    __AROS_SETVECADDRV0(abiv0GfxBase,  45, (APTR32)(IPTR)proxy_WaitTOF);
    __AROS_SETVECADDRV0(abiv0GfxBase, 153, (APTR32)(IPTR)proxy_AllocBitMap);
    __AROS_SETVECADDRV0(abiv0GfxBase, 154, (APTR32)(IPTR)proxy_FreeBitMap);
    __AROS_SETVECADDRV0(abiv0GfxBase, 159, (APTR32)(IPTR)proxy_ObtainPen);
    __AROS_SETVECADDRV0(abiv0GfxBase, 142, (APTR32)(IPTR)proxy_SetRGB32);
    __AROS_SETVECADDRV0(abiv0GfxBase, 122, (APTR32)(IPTR)proxy_NextDisplayInfo);
    __AROS_SETVECADDRV0(abiv0GfxBase, 121, (APTR32)(IPTR)proxy_FindDisplayInfo);
    __AROS_SETVECADDRV0(abiv0GfxBase,  82, graphicsjmp[202 -  82]);  // AllocRaster
    __AROS_SETVECADDRV0(abiv0GfxBase,  83, graphicsjmp[202 -  83]);  // FreeRaster
    __AROS_SETVECADDRV0(abiv0GfxBase,  66, (APTR32)(IPTR)proxy_ScrollRaster);
    __AROS_SETVECADDRV0(abiv0GfxBase, 193, graphicsjmp[202 - 193]);  // AndRectRect
    __AROS_SETVECADDRV0(abiv0GfxBase,  78, graphicsjmp[202 -  78]);  // InitTmpRas
    __AROS_SETVECADDRV0(abiv0GfxBase, 118, (APTR32)(IPTR)proxy_VideoControl);
    __AROS_SETVECADDRV0(abiv0GfxBase, 133, (APTR32)(IPTR)proxy_ModeNotAvailable);

    Graphics_Operations_init(abiv0GfxBase, graphicsjmp);
    Graphics_RastPorts_init(abiv0GfxBase, graphicsjmp);
    Graphics_Regions_init(abiv0GfxBase);
}

void exit_graphics()
{
    Graphics_RastPorts_deinit();
    UnLoadSeg(graphicsseg);
}
