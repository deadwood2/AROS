/*
    Copyright (C) 2024-2026, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/layers.h>
#include <aros/debug.h>
#include <exec/rawfmt.h>
#include <graphics/regions.h>

#include <string.h>

#include "../../include/exec/structures.h"
#include "../../include/exec/functions.h"
#include "../../include/aros/cpu.h"
#include "../../include/aros/proxy.h"
#include "../../include/aros/call32.h"
#include "../../include/graphics/structures.h"
#include "../../include/graphics/proxy_structures.h"

#include "../exec/exec_libraries.h"
#include "../graphics/graphics_regions.h"

#include "../../support.h"

struct ExecBaseV0 *Layers_SysBaseV0;

struct LibraryV0 *abiv0_Layers_OpenLib(ULONG version, struct LibraryV0 *LayersBaseV0)
{
    LayersBaseV0->lib_OpenCnt++;
    return LayersBaseV0;
}
MAKE_PROXY_ARG_2(Layers_OpenLib)

BPTR abiv0_Layers_CloseLib(struct LibraryV0 *LayersBaseV0)
{
    LayersBaseV0->lib_OpenCnt--;
    return BNULL;
}
MAKE_PROXY_ARG_1(Layers_CloseLib)

BPTR abiv0_Layers_ExpungeLib(struct LibraryV0 *extralhV0, struct LibraryV0 *LayersBaseV0)
{
    CALL32_ARG_2_NR(__AROS_GETVECADDRV0(Layers_SysBaseV0, 42), LayersBaseV0, (APTR32)(IPTR)Layers_SysBaseV0);
    return BNULL;
}
MAKE_PROXY_ARG_2(Layers_ExpungeLib)

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

struct Layer_InfoV0 *abiv0_NewLayerInfo(struct LibraryV0 *LayersBaseV0)
{
bug("abiv0_NewLayerInfo: STUB\n");
    return NULL; /* Workaround for TextEditor.mcc 15.56*/
}
MAKE_PROXY_ARG_1(NewLayerInfo)

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

void Layers_Unhandled_init(struct LibraryV0 *abiv0LayersBase);
struct LibraryV0 *shallow_InitResident32(struct ResidentV0 *resident, BPTR segList, struct ExecBaseV0 *SysBaseV0);
BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);

static BPTR layersseg;

void init_layers(struct ExecBaseV0 *SysBaseV0)
{
    TEXT path[64];

    NewRawDoFmt("LIBSV0:partial/layers.library", RAWFMTFUNC_STRING, path);
    layersseg = LoadSeg32(path, DOSBase);
    struct ResidentV0 *layersres = findResident(layersseg, NULL);
    struct LibraryV0 *abiv0LayersBase = shallow_InitResident32(layersres, layersseg, SysBaseV0);
    Layers_SysBaseV0 = SysBaseV0;
    /* Remove all vectors for now */
    for (int i = 1; i <= 45; i++) __AROS_SETVECADDRV0(abiv0LayersBase, i, 0);
    /* Set all unhandled LVO addresses to a catch function */
    Layers_Unhandled_init((struct LibraryV0 *)abiv0LayersBase);
    __AROS_SETVECADDRV0(abiv0LayersBase,   1, (APTR32)(IPTR)proxy_Layers_OpenLib);
    __AROS_SETVECADDRV0(abiv0LayersBase,   2, (APTR32)(IPTR)proxy_Layers_CloseLib);
    __AROS_SETVECADDRV0(abiv0LayersBase,   3, (APTR32)(IPTR)proxy_Layers_ExpungeLib);
    __AROS_SETVECADDRV0(abiv0LayersBase,  29, (APTR32)(IPTR)proxy_InstallClipRegion);
    __AROS_SETVECADDRV0(abiv0LayersBase,  20, (APTR32)(IPTR)proxy_LockLayerInfo);
    __AROS_SETVECADDRV0(abiv0LayersBase,  23, (APTR32)(IPTR)proxy_UnlockLayerInfo);
    __AROS_SETVECADDRV0(abiv0LayersBase,  16, (APTR32)(IPTR)proxy_LockLayer);
    __AROS_SETVECADDRV0(abiv0LayersBase,  17, (APTR32)(IPTR)proxy_UnlockLayer);
    __AROS_SETVECADDRV0(abiv0LayersBase,  14, (APTR32)(IPTR)proxy_EndUpdate);
    __AROS_SETVECADDRV0(abiv0LayersBase,  13, (APTR32)(IPTR)proxy_BeginUpdate);
    __AROS_SETVECADDRV0(abiv0LayersBase,  36, (APTR32)(IPTR)proxy_DoHookClipRects);
    __AROS_SETVECADDRV0(abiv0LayersBase,  22, (APTR32)(IPTR)proxy_WhichLayer);
    __AROS_SETVECADDRV0(abiv0LayersBase,  24, (APTR32)(IPTR)proxy_NewLayerInfo);
}

void exit_layers()
{
    UnLoadSeg(layersseg);
}
