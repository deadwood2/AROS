/*
    Copyright (C) 2024-2026, The AROS Development Team. All rights reserved.
*/

#include <dos/bptr.h>
#include <exec/types.h>
#include <aros/debug.h>
#include <proto/timer.h>
#include <exec/rawfmt.h>
#include <proto/dos.h>
#include <proto/input.h>
#include <string.h>

#include "abiv0/include/exec/functions.h"
#include "abiv0/include/exec/proxy_structures.h"
#include "abiv0/include/aros/proxy.h"
#include "abiv0/include/aros/cpu.h"
#include "abiv0/include/input/structures.h"

const TEXT version_string[] = "$VER: EmuV0 0.42 (9.01.2026)";

struct DeviceProxy *abiv0TimerBase;
struct DeviceProxy *abiv0InputBase;
struct DeviceProxy *abiv0ConsoleBase;

struct LibraryV0 *shallow_InitResident32(struct ResidentV0 *resident, BPTR segList, struct ExecBaseV0 *SysBaseV0)
{
    struct LibraryV0 *library = NULL;

    D(bug("InitResident begin 0x%p (\"%s\")", resident, resident->rt_Name));

    /* Check for validity */
    if(resident->rt_MatchWord != RTC_MATCHWORD ||
       resident->rt_MatchTag != (APTR32)(IPTR)resident)
        return NULL;

    /* Depending on the autoinit flag... */
    if(resident->rt_Flags & RTF_AUTOINIT)
    {
        /* ...initialize automatically... */
        struct init
        {
            ULONG dSize;
            APTR32 vectors;
            APTR32 structure;
            APTR32 init;
        };
        struct init *init = (struct init *)(IPTR)resident->rt_Init;
        init->init = 0;

        library = abiv0_InitResident(resident, segList, SysBaseV0);
    }
    else
    {
        D(bug("InitResident !RTF_AUTOINIT"));
asm("int3");
    }

    D(bug("InitResident end 0x%p (\"%s\"), result 0x%p", resident, resident->rt_Name, library));

    return library;
} /* shallow_InitResident32 */

struct LibraryV0 *abiv0_Layers_OpenLib(ULONG version, struct LibraryV0 *LayersBaseV0)
{
    return LayersBaseV0;
}
MAKE_PROXY_ARG_2(Layers_OpenLib)

#include <proto/layers.h>
#include <graphics/regions.h>

#include "abiv0/include/graphics/structures.h"
#include "abiv0/include/graphics/proxy_structures.h"
#include "abiv0/graphics/graphics_regions.h"

struct ExecBaseV0 *Layers_SysBaseV0;

struct RegionV0 *abiv0_InstallClipRegion(struct LayerV0  *l, struct RegionV0 *region, struct LibraryV0 *LayersBaseV0)
{
    struct LayerProxy *lproxy = (struct LayerProxy *)l;
    struct RegionProxy *regionproxy = (struct RegionProxy *)region;
    if (regionproxy)
        InstallClipRegion(lproxy->native, regionproxy->native);
    else
        InstallClipRegion(lproxy->native, NULL);

    return NULL; /* FIXME */
}
MAKE_PROXY_ARG_3(InstallClipRegion)

void abiv0_LockLayerInfo(struct Layer_InfoV0 *li, struct LibraryV0 *LayersBaseV0)
{
    struct Layer_Info *linative = (struct Layer_Info *)*(IPTR *)(&li->PrivateReserve1);
    LockLayerInfo(linative);
}
MAKE_PROXY_ARG_2(LockLayerInfo)

void abiv0_UnlockLayerInfo(struct Layer_InfoV0 *li, struct LibraryV0 *LayersBaseV0)
{
    struct Layer_Info *linative = (struct Layer_Info *)*(IPTR *)(&li->PrivateReserve1);
    UnlockLayerInfo(linative);
}
MAKE_PROXY_ARG_2(UnlockLayerInfo)

struct LayerV0 *abiv0_WhichLayer(struct Layer_InfoV0 *li, LONG x, LONG y, struct LibraryV0 *LayersBaseV0)
{
bug("abiv0_WhichLayer: STUB\n");
    return NULL;
}
MAKE_PROXY_ARG_4(WhichLayer)

void abiv0_LockLayer(LONG dummy, struct LayerV0 *layer, struct LibraryV0 *LayersBaseV0)
{
    struct LayerProxy *proxy = (struct LayerProxy *)layer;
    LockLayer(dummy, proxy->native);

    if (proxy->native->DamageList)
    {
        struct RegionProxy *rproxy = abiv0_AllocMem(sizeof(struct RegionProxy), MEMF_CLEAR, Layers_SysBaseV0);
        rproxy->native  = proxy->native->DamageList;

        syncRegionV0(rproxy);

        proxy->base.DamageList  = (APTR32)(IPTR)rproxy;
    }
    else
    {
        if (proxy->base.DamageList) bug("abiv0_LockLayer: MEMORY LEAK\n");
        proxy->base.DamageList = (APTR32)(IPTR)NULL;
    }
}
MAKE_PROXY_ARG_3(LockLayer)

void abiv0_UnlockLayer(struct LayerV0 *layer, struct LibraryV0 *LayersBaseV0)
{
    struct LayerProxy *proxy = (struct LayerProxy *)layer;
    UnlockLayer(proxy->native);
}
MAKE_PROXY_ARG_2(UnlockLayer)

void abiv0_EndUpdate(struct LayerV0 *l, UWORD flag, struct LibraryV0 *LayersBaseV0)
{
    struct LayerProxy *proxy = (struct LayerProxy *)l;
    EndUpdate(proxy->native, flag);
}
MAKE_PROXY_ARG_3(EndUpdate)

LONG abiv0_BeginUpdate(struct LayerV0 *l, struct LibraryV0 *LayersBaseV0)
{
    struct LayerProxy *proxy = (struct LayerProxy *)l;
    BeginUpdate(proxy->native);
}
MAKE_PROXY_ARG_3(BeginUpdate)

void abiv0_DoHookClipRects(struct Hook *hook, struct RastPortV0 * rport, struct Rectangle *rect, struct LibraryV0 *LayersBaseV0)
{
bug("abiv0_DoHookClipRects: STUB\n");
    return;
}
MAKE_PROXY_ARG_4(DoHookClipRects)

void abiv0_GetSysTime(struct timeval *dest, struct LibraryV0 *TimerBaseV0)
{
    GetSysTime(dest);
}
MAKE_PROXY_ARG_2(GetSysTime)

void abiv0_SubTime(struct timeval *dest, struct timeval *src, struct LibraryV0 *TimerBaseV0)
{
    SubTime(dest, src);
}
MAKE_PROXY_ARG_3(SubTime)

void abiv0_AddTime(struct timeval *dest, struct timeval *src, struct LibraryV0 *TimerBaseV0)
{
    AddTime(dest, src);
}
MAKE_PROXY_ARG_3(AddTime)

LONG abiv0_CmpTime(struct timeval *dest, struct timeval *src, struct LibraryV0 *TimerBaseV0)
{
    return CmpTime(dest, src);
}
MAKE_PROXY_ARG_3(CmpTime)

UWORD abiv0_PeekQualifier(struct LibraryV0 *InputBaseV0)
{
    struct Library *InputBase = &(((struct DeviceProxy *)InputBaseV0)->native->dd_Library);
    return PeekQualifier();
}
MAKE_PROXY_ARG_1(PeekQualifier)

#include <proto/console.h>

LONG abiv0_RawKeyConvert(struct InputEventV0 *events, STRPTR buffer, LONG length, struct KeyMap * keyMap, struct LibraryV0 *ConsoleBaseV0)
{
bug("abiv0_RawKeyConvert: STUB\n");
    /* Support only SDL->CGX_TranslateKey case */
    if (length != 5 || keyMap != NULL)
        return 0;
    if ((APTR)(IPTR)events->ie_NextEvent != NULL || (APTR)(IPTR)events->ie_position.ie_addr != NULL)
        return 0;

    struct Library *ConsoleDevice = &(((struct DeviceProxy *)ConsoleBaseV0)->native->dd_Library);
    struct InputEvent eventnative;
    eventnative.ie_Qualifier    = events->ie_Qualifier;
    eventnative.ie_Class        = events->ie_Class;
    eventnative.ie_SubClass     = events->ie_SubClass;
    eventnative.ie_position.ie_addr = NULL;
    eventnative.ie_NextEvent = NULL;
    return RawKeyConvert(&eventnative, buffer, length, NULL);
}
MAKE_PROXY_ARG_5(RawKeyConvert)

#include <proto/cybergraphics.h>
#include "abiv0/graphics/graphics_rastports.h"

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
    BOOL clear = FALSE;
    ULONG _ret;

    if (rpnative->BitMap == NULL)
    {
        /* RNOTunes uses locally created RastPort */
        rpnative->BitMap = ((struct BitMapProxy *)(IPTR)rp->BitMap)->native;
        rpnative->Layer = ((struct LayerProxy *)(IPTR)rp->Layer)->native;
        clear = TRUE;
    }

    _ret = WritePixelArrayAlpha(src, srcx, srcy, srcmod, rpnative, destx, desty, width, height, globalalpha);

    if (clear)
    {
        rpnative->BitMap = NULL;
        rpnative->Layer = NULL;
    }

    return _ret;
}
MAKE_PROXY_ARG_12(WritePixelArrayAlpha)

#include <proto/graphics.h>

LONG abiv0_WriteLUTPixelArray(APTR srcRect, UWORD SrcX, UWORD SrcY, UWORD SrcMod, struct RastPortV0 *rp,
    APTR CTable, UWORD DestX, UWORD DestY, UWORD SizeX, UWORD SizeY, UBYTE CTabFormat, struct LibraryV0 *CyberGfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
#if 0
    struct RastPort rptmp;

    /* picture.datatype uses locally created RastPort */
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

    return WriteLUTPixelArray(srcRect, SrcX, SrcY, SrcMod, rpnative, CTable, DestX, DestY, SizeX, SizeY, CTabFormat);
}
MAKE_PROXY_ARG_12(WriteLUTPixelArray)

ULONG abiv0_WritePixelArray(APTR src, UWORD srcx, UWORD srcy, UWORD srcmod, struct RastPortV0 *rp,
    UWORD destx, UWORD desty, UWORD width, UWORD height, UBYTE srcformat, struct LibraryV0 *CyberGfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    BOOL clear = FALSE;
    ULONG _ret;

    if (rpnative->BitMap == NULL)
    {
        /* Soliton operates on locally created RastPort */
        /* picture.datatype uses locally created RastPort */
        rpnative->BitMap = ((struct BitMapProxy *)(IPTR)rp->BitMap)->native;
        clear = TRUE;
    }

    _ret = WritePixelArray(src, srcx, srcy, srcmod, rpnative, destx, desty, width, height, srcformat);

    if (clear) rpnative->BitMap = NULL;

    return _ret;
}
MAKE_PROXY_ARG_12(WritePixelArray)

ULONG abiv0_ReadPixelArray(APTR dst, UWORD dstx, UWORD dsty, UWORD dstmod, struct RastPortV0 *rp,
    UWORD srcx, UWORD srcy, UWORD width, UWORD height, UBYTE dstformat, struct LibraryV0 *CyberGfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    return ReadPixelArray(dst, dstx, dsty, dstmod, rpnative, srcx, srcy, width, height, dstformat);
}
MAKE_PROXY_ARG_12(ReadPixelArray)

#include "abiv0/include/utility/structures.h"

VOID abiv0_ProcessPixelArray(struct RastPortV0 *rp, ULONG destX, ULONG destY, ULONG sizeX, ULONG sizeY, ULONG operation,
        LONG value, struct TagItemV0 *taglist, struct LibraryV0 *CyberGfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    BOOL clear = FALSE;

    if (rpnative->BitMap == NULL)
    {
        /* dtpic.mui uses locally created RastPort */
        rpnative->BitMap = ((struct BitMapProxy *)(IPTR)rp->BitMap)->native;
        clear = TRUE;
    }

    ProcessPixelArray(rpnative, destX, destY, sizeX, sizeY, operation, value, NULL);

    if (clear) rpnative->BitMap = NULL;
}
MAKE_PROXY_ARG_12(ProcessPixelArray)

BOOL abiv0_IsCyberModeID(ULONG modeID, struct LibraryV0 *CyberGfxBaseV0)
{
    return IsCyberModeID(modeID);
}
MAKE_PROXY_ARG_2(IsCyberModeID)

extern struct TagItem *CloneTagItemsV02Native(const struct TagItemV0 *tagList);
extern void FreeClonedV02NativeTagItems(struct TagItem *tagList);

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

BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);
struct ResidentV0 * findResident(BPTR seg, CONST_STRPTR name);

void init_graphics(struct ExecBaseV0 *);
void exit_graphics();
void init_intuition(struct ExecBaseV0 *, struct DeviceProxy *);
void init_dos(struct ExecBaseV0 *);
struct ExecBaseV0 *init_exec();

void execute_in_32_bit(APTR start, CONST_STRPTR argstr, LONG argsize, struct ExecBaseV0 *SysBaseV0)
{
    __asm__ volatile(
    "   subq $16, %%rsp\n"
    "   movl $0, %%eax\n"
    "   movl %%eax, 12(%%rsp)\n" //pad
    "   movl %3, %%eax\n"
    "   movl %%eax, 8(%%rsp)\n" //SysBase
    "   movl %2, %%eax\n"
    "   movl %%eax, 4(%%rsp)\n" //argsize
    "   movl %1, %%eax\n"
    "   movl %%eax, (%%rsp)\n" //argstr
    "   movl %0, %%edx\n"  // start
    ENTER32
    "   call *%%edx\n"
    ENTER64
    "   addq $16, %%rsp"// Clean up stack
        :: "mr"(start), "mr" (argstr), "mr" (argsize), "mr" (SysBaseV0)
        : SCRATCH_REGS_64_TO_32 );
}

void refresh_g_v0maintask();

STRPTR emu_argstr = NULL;
LONG emu_argsize = 0;

LONG_FUNC run_emulation(CONST_STRPTR program_path)
{
    TEXT path[64];
    TEXT currdir[256];
    UWORD negsize, possize, lastlvo;
    APTR tmpmem;

    /* Init ROM */
    struct ExecBaseV0 *SysBaseV0 = init_exec();

    /* timer.device */
    lastlvo = 12;
    negsize = (lastlvo + 1) * sizeof(struct JumpVecV0);
    possize = sizeof(struct DeviceProxy);
    tmpmem  = abiv0_AllocMem(negsize + possize, MEMF_CLEAR, SysBaseV0);
    abiv0TimerBase = (tmpmem + negsize);
        /* Set all LVO addresses to their number so that code jumps to "number" of the LVO and crashes */
    for (int i = 5; i <= lastlvo; i++) __AROS_SETVECADDRV0(abiv0TimerBase, i, (APTR32)(IPTR)i + 1100);
    __AROS_SETVECADDRV0(abiv0TimerBase, 11, (APTR32)(IPTR)proxy_GetSysTime);
    __AROS_SETVECADDRV0(abiv0TimerBase,  8, (APTR32)(IPTR)proxy_SubTime);
    __AROS_SETVECADDRV0(abiv0TimerBase,  7, (APTR32)(IPTR)proxy_AddTime);
    __AROS_SETVECADDRV0(abiv0TimerBase,  9, (APTR32)(IPTR)proxy_CmpTime);
    abiv0TimerBase->type = DEVPROXY_TYPE_TIMER;

    /* input.device */
    lastlvo = 7;
    negsize = (lastlvo + 1) * sizeof(struct JumpVecV0);
    possize = sizeof(struct DeviceProxy);
    tmpmem  = abiv0_AllocMem(negsize + possize, MEMF_CLEAR, SysBaseV0);
    abiv0InputBase = (tmpmem + negsize);
    /* Set all LVO addresses to their number so that code jumps to "number" of the LVO and crashes */
    for (int i = 5; i <= lastlvo; i++) __AROS_SETVECADDRV0(abiv0InputBase, i, (APTR32)(IPTR)i + 1150);
    __AROS_SETVECADDRV0(abiv0InputBase,  7, (APTR32)(IPTR)proxy_PeekQualifier);
    abiv0InputBase->type                        = DEVPROXY_TYPE_INPUT;
    abiv0InputBase->base.dd_Library.lib_NegSize = negsize;
    abiv0InputBase->base.dd_Library.lib_PosSize = possize;

    /* console.device */
    lastlvo = 12;
    negsize = (lastlvo + 1) * sizeof(struct JumpVecV0);
    possize = sizeof(struct DeviceProxy);
    tmpmem  = abiv0_AllocMem(negsize + possize, MEMF_CLEAR, SysBaseV0);
    abiv0ConsoleBase = (tmpmem + negsize);
    /* Set all LVO addresses to their number so that code jumps to "number" of the LVO and crashes */
    for (int i = 5; i <= lastlvo; i++) __AROS_SETVECADDRV0(abiv0ConsoleBase, i, (APTR32)(IPTR)i + 1200);
    __AROS_SETVECADDRV0(abiv0ConsoleBase,  8, (APTR32)(IPTR)proxy_RawKeyConvert);
    abiv0ConsoleBase->type                        = DEVPROXY_TYPE_CONSOLE;
    abiv0ConsoleBase->base.dd_Library.lib_NegSize = negsize;
    abiv0ConsoleBase->base.dd_Library.lib_PosSize = possize;

    init_dos(SysBaseV0);

    init_graphics(SysBaseV0);

    NewRawDoFmt("LIBSV0:partial/layers.library", RAWFMTFUNC_STRING, path);
    BPTR layersseg = LoadSeg32(path, DOSBase);
    struct ResidentV0 *layersres = findResident(layersseg, NULL);
    struct LibraryV0 *abiv0LayersBase = shallow_InitResident32(layersres, layersseg, SysBaseV0);
    Layers_SysBaseV0 = SysBaseV0;
    /* Remove all vectors for now */
    for (int i = 1; i <= 45; i++) __AROS_SETVECADDRV0(abiv0LayersBase, i, 0);
    /* Set all LVO addresses to their number so that code jumps to "number" of the LVO and crashes */
    for (int i = 5; i <= 45; i++) __AROS_SETVECADDRV0(abiv0LayersBase, i, (APTR32)(IPTR)i + 200 + 300 + 200 + 200);
    __AROS_SETVECADDRV0(abiv0LayersBase,   1, (APTR32)(IPTR)proxy_Layers_OpenLib);
    __AROS_SETVECADDRV0(abiv0LayersBase,  29, (APTR32)(IPTR)proxy_InstallClipRegion);
    __AROS_SETVECADDRV0(abiv0LayersBase,  20, (APTR32)(IPTR)proxy_LockLayerInfo);
    __AROS_SETVECADDRV0(abiv0LayersBase,  23, (APTR32)(IPTR)proxy_UnlockLayerInfo);
    __AROS_SETVECADDRV0(abiv0LayersBase,  16, (APTR32)(IPTR)proxy_LockLayer);
    __AROS_SETVECADDRV0(abiv0LayersBase,  17, (APTR32)(IPTR)proxy_UnlockLayer);
    __AROS_SETVECADDRV0(abiv0LayersBase,  14, (APTR32)(IPTR)proxy_EndUpdate);
    __AROS_SETVECADDRV0(abiv0LayersBase,  13, (APTR32)(IPTR)proxy_BeginUpdate);
    __AROS_SETVECADDRV0(abiv0LayersBase,  36, (APTR32)(IPTR)proxy_DoHookClipRects);
    __AROS_SETVECADDRV0(abiv0LayersBase,  22, (APTR32)(IPTR)proxy_WhichLayer);

    NewRawDoFmt("LIBSV0:partial/cybergraphics.library", RAWFMTFUNC_STRING, path);
    BPTR cgfxseg = LoadSeg32(path, DOSBase);
    struct ResidentV0 *cgfxres = findResident(cgfxseg, NULL);
    struct LibraryV0 *abiv0CyberGfxBase = shallow_InitResident32(cgfxres, cgfxseg, SysBaseV0);

    /* Remove all vectors for now (leave LibOpen) */
    const ULONG cybergraphicsjmpsize = 38 * sizeof(APTR32);
    APTR32 *cybergraphicsjmp = AllocMem(cybergraphicsjmpsize, MEMF_CLEAR);
    CopyMem((APTR)abiv0CyberGfxBase - cybergraphicsjmpsize, cybergraphicsjmp, cybergraphicsjmpsize);
    for (int i = 5; i <= 38; i++) __AROS_SETVECADDRV0(abiv0CyberGfxBase, i, 0);
    /* Set all LVO addresses to their number so that code jumps to "number" of the LVO and crashes */
    for (int i = 5; i <= 38; i++) __AROS_SETVECADDRV0(abiv0CyberGfxBase, i, (APTR32)(IPTR)i + 200 + 300 + 200 + 200 + 100);
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

    init_intuition(SysBaseV0, abiv0TimerBase);

    /* Install datatypes */
    NewRawDoFmt("EmuV0:C/AddDataTypes", RAWFMTFUNC_STRING, path);
    BPTR adtseg = LoadSeg32(path, DOSBase);
    APTR (*adtstart)() = (APTR)((IPTR)BADDR(adtseg) + sizeof(BPTR));
    /* Inject arguments for AddDataTypes*/
    struct FileHandle *fhinput = BADDR(Input());
    CopyMem("REFRESH\n", BADDR(fhinput->fh_Buf), 9);
    fhinput->fh_Pos = 0;
    fhinput->fh_End = 9;

    execute_in_32_bit(adtstart, "\n", 1, SysBaseV0);

    /* Start Program */
    NewRawDoFmt("%s", RAWFMTFUNC_STRING, path, program_path);
    BPTR seg = LoadSeg32(path, DOSBase);
    APTR (*start)() = (APTR)((IPTR)BADDR(seg) + sizeof(BPTR));

    /* Set arguments for main program */
    if (fhinput->fh_BufSize < emu_argsize) asm("int3");
    CopyMem(emu_argstr, BADDR(fhinput->fh_Buf), emu_argsize);
    fhinput->fh_Pos = 0;
    fhinput->fh_End = emu_argsize;

    /* Make sure PROGDIR: is correct */
    *(PathPart(path)) = '\0';
    BPTR progdir = Lock(path, SHARED_LOCK);
    BPTR oldprogdir = SetProgramDir(progdir);
    BPTR oldcurdir = CurrentDir(DupLock(progdir));
    GetCurrentDirName(currdir, 256);
    SetCurrentDirName(path);
    refresh_g_v0maintask();

    execute_in_32_bit(start, emu_argstr, emu_argsize, SysBaseV0);

    SetCurrentDirName(currdir);
    UnLock(CurrentDir(oldcurdir));
    SetProgramDir(oldprogdir);
    UnLock(progdir);

    exit_graphics();
}

struct timerequest tr;
struct Device *TimerBase;

STRPTR program_name = NULL;

int main(int argc, char **argv)
{
    STRPTR program_path = NULL;

    if (argc > 1)
    {
        struct Process *me;
        STRPTR p; LONG qc = 0;
        program_path = argv[1];
        BPTR tmp = Lock(program_path, SHARED_LOCK);
        if (tmp == BNULL)
        {
            Printf("Program '%s' not found.\n", program_path);
            return 0;
        }

        /* Build emu_argstr */
        p = ((struct Process *)FindTask(NULL))->pr_Arguments;
        /* Assume worst case: "program with space" "arg1 with space" arg arg3 \n */
        /* Get rid off '"program with space"' or 'program' */
        while (*p != '\n')
        {
            if (*p == '"') qc++;
            if (*p == ' ')
                if (qc == 0 || qc == 2) { p++; break; }
            p++;
        }

        emu_argsize = strlen(p);
        emu_argstr = AllocMem(emu_argsize + 1, MEMF_31BIT | MEMF_CLEAR);
        CopyMem(p, emu_argstr, emu_argsize);
    }
    else
    {
        Printf("EmuV0 needs at least one argument - program name\n");
        return 0;
    }

    /* Save program name - dependency - this need to be set before first call to abiv0_FindTask() */
    program_name = StrDup(FilePart(program_path));

    OpenDevice("timer.device", UNIT_VBLANK, &tr.tr_node, 0);
    TimerBase = tr.tr_node.io_Device;

    /* Run emulation code with stack allocated in 31-bit memory */
    APTR stack31bit = AllocMem(64 * 1024, MEMF_CLEAR | MEMF_31BIT);
    struct StackSwapStruct sss;
    sss.stk_Lower = stack31bit;
    sss.stk_Upper = sss.stk_Lower + 64 * 1024;
    sss.stk_Pointer = sss.stk_Upper;

    struct StackSwapArgs ssa;
    ssa.Args[0] = (IPTR)program_path;

    NewStackSwap(&sss, run_emulation, &ssa);

    FreeVec(program_name);
    if (emu_argstr) FreeMem(emu_argstr, emu_argsize + 1);
    return 0;
}

