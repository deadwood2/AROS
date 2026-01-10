/*
    Copyright (C) 2026, The AROS Development Team. All rights reserved.
*/

#ifndef _GRAPHICS_RASTPORTS
#define _GRAPHICS_RASTPORTS

void Graphics_RastPorts_init(struct GfxBaseV0 *abiv0GfxBase, APTR32 *graphicsjmp);
void Graphics_RastPorts_deinit();

struct RastPort *RastPortV0_getnative(struct RastPortV0 *rp);
void RastPortV0_attachnative(struct RastPortV0 *rp, struct RastPort *rpnative);

#define synchronize_SetAfPt(rp, rpnative)           \
    rpnative->AreaPtrn = (APTR)(IPTR)rp->AreaPtrn;  \
    rpnative->AreaPtSz = rp->AreaPtSz;

struct RastPortV0 *makeRastPortV0(struct RastPort *native);
void freeRastPortV0(struct RastPortV0 *v0);

#endif
