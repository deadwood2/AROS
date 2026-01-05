/*
    Copyright (C) 2026, The AROS Development Team. All rights reserved.
*/

#include <proto/graphics.h>
#include <proto/exec.h>
#include <proto/alib.h>

#include "../include/exec/functions.h"
#include "../include/graphics/structures.h"
#include "../include/graphics/proxy_structures.h"
#include "../include/aros/cpu.h"
#include "../include/aros/proxy.h"
#include "../include/utility/structures.h"

#include "graphics_regions.h"

extern struct ExecBaseV0 *Gfx_SysBaseV0;

void syncRegionV0(struct RegionProxy *proxy)
{
    struct RegionRectangle *rrnative = proxy->native->RegionRectangle;
    struct RegionRectangleV0 *rrv0prev = NULL, *rrv0first = NULL;

    if (proxy->rrArray != NULL)
        abiv0_FreeMem(proxy->rrArray, sizeof(struct RegionRectangleV0) * proxy->rrCount, Gfx_SysBaseV0);
    proxy->rrArray = NULL;
    proxy->rrCount = 0;
    proxy->base.RegionRectangle = (APTR32)(IPTR)NULL;

    proxy->base.bounds.MaxX    = proxy->native->bounds.MaxX;
    proxy->base.bounds.MinX    = proxy->native->bounds.MinX;
    proxy->base.bounds.MaxY    = proxy->native->bounds.MaxY;
    proxy->base.bounds.MinY    = proxy->native->bounds.MinY;

    while(rrnative)
    {
        proxy->rrCount++;
        rrnative = rrnative->Next;
    }

    if (proxy->rrCount == 0)
        return;

    proxy->rrArray = abiv0_AllocMem(sizeof(struct RegionRectangleV0) * proxy->rrCount, MEMF_CLEAR, Gfx_SysBaseV0);
    rrnative = proxy->native->RegionRectangle;
    ULONG i = 0;

    while(rrnative)
    {
        struct RegionRectangleV0 *rrv0 = &proxy->rrArray[i++];
        rrv0->bounds.MaxX = rrnative->bounds.MaxX;
        rrv0->bounds.MinX = rrnative->bounds.MinX;
        rrv0->bounds.MaxY = rrnative->bounds.MaxY;
        rrv0->bounds.MinY = rrnative->bounds.MinY;
        if (rrv0prev)
        {
            rrv0prev->Next = (APTR32)(IPTR)rrv0;
            rrv0->Prev = (APTR32)(IPTR)rrv0prev;
            rrv0prev = rrv0;
        }
        if (!rrv0prev)
        {
            rrv0first = rrv0prev = rrv0;
        }

        rrnative = rrnative->Next;
    }

    proxy->base.RegionRectangle = (APTR32)(IPTR)rrv0first;
}

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

BOOL abiv0_XorRectRegion(struct RegionV0 *Reg, struct Rectangle *Rect, struct GfxBaseV0 *GfxBaseV0)
{
    struct RegionProxy *proxy = (struct RegionProxy *)Reg;
    return XorRectRegion(proxy->native, Rect);
}
MAKE_PROXY_ARG_3(XorRectRegion)

void abiv0_DisposeRegion(struct RegionV0 *region, struct GfxBaseV0 *GfxBaseV0)
{
    struct RegionProxy *proxy = (struct RegionProxy *)region;
    if (proxy->rrArray != NULL)
        abiv0_FreeMem(proxy->rrArray, sizeof(struct RegionRectangleV0) * proxy->rrCount, Gfx_SysBaseV0);
    DisposeRegion(proxy->native);
}
MAKE_PROXY_ARG_2(DisposeRegion)

BOOL abiv0_ClearRectRegion(struct RegionV0 *Reg, struct Rectangle *Rect, struct GfxBaseV0 *GfxBaseV0)
{
    struct RegionProxy *proxy = (struct RegionProxy *)Reg;
    BOOL _res = ClearRectRegion(proxy->native, Rect);
    syncRegionV0(proxy);
    return _res;
}
MAKE_PROXY_ARG_3(ClearRectRegion)

BOOL abiv0_AndRegionRegion(struct RegionV0 *R1, struct RegionV0 *R2, struct GfxBaseV0 *GfxBaseV0)
{
    struct RegionProxy *proxy1 = (struct RegionProxy *)R1;
    struct RegionProxy *proxy2 = (struct RegionProxy *)R2;
    return AndRegionRegion(proxy1->native, proxy2->native);
}
MAKE_PROXY_ARG_3(AndRegionRegion)

void Graphics_Regions_init(struct GfxBaseV0 *abiv0GfxBase)
{
    __AROS_SETVECADDRV0(abiv0GfxBase,  86, (APTR32)(IPTR)proxy_NewRegion);
    __AROS_SETVECADDRV0(abiv0GfxBase,  85, (APTR32)(IPTR)proxy_OrRectRegion);
    __AROS_SETVECADDRV0(abiv0GfxBase,  89, (APTR32)(IPTR)proxy_DisposeRegion);
    __AROS_SETVECADDRV0(abiv0GfxBase,  87, (APTR32)(IPTR)proxy_ClearRectRegion);
    __AROS_SETVECADDRV0(abiv0GfxBase, 194, (APTR32)(IPTR)proxy_NewRectRegion);
    __AROS_SETVECADDRV0(abiv0GfxBase, 104, (APTR32)(IPTR)proxy_AndRegionRegion);
    __AROS_SETVECADDRV0(abiv0GfxBase,  93, (APTR32)(IPTR)proxy_XorRectRegion);
}
