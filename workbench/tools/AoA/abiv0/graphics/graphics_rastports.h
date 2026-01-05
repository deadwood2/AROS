/*
    Copyright (C) 2026, The AROS Development Team. All rights reserved.
*/

#ifndef _GRAPHICS_RASTPORTS
#define _GRAPHICS_RASTPORTS

void Graphics_Rastports_init(struct GfxBaseV0 *abiv0GfxBase, APTR32 *graphicsjmp);

#define RastPortV0_getnative(rp) (struct RastPort *)*(IPTR *)&rp->longreserved

#define synchronize_SetAfPt(rp, rpnative)           \
    rpnative->AreaPtrn = (APTR)(IPTR)rp->AreaPtrn;  \
    rpnative->AreaPtSz = rp->AreaPtSz;

#endif
