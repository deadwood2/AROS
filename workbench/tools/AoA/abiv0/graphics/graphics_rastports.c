/*
    Copyright (C) 2026, The AROS Development Team. All rights reserved.
*/

#include <graphics/rpattr.h>
#include <proto/graphics.h>

#include <aros/debug.h>

#include "../include/aros/cpu.h"
#include "../include/aros/proxy.h"
#include "../include/exec/functions.h"
#include "../include/graphics/structures.h"
#include "../include/graphics/proxy_structures.h"
#include "../include/utility/structures.h"

#include "graphics_rastports.h"

struct TagItem *CloneTagItemsV02Native(const struct TagItemV0 *tagList);
void FreeClonedV02NativeTagItems(struct TagItem *tagList);
struct TagItemV0 *LibNextTagItemV0(struct TagItemV0 **tagListPtr);

extern struct ExecBaseV0 *Gfx_SysBaseV0;

struct RastPortV0 *makeRastPortV0(struct RastPort *native)
{
    struct RastPortV0 *rpv0 = abiv0_AllocMem(sizeof(struct RastPortV0), MEMF_CLEAR, Gfx_SysBaseV0);
    RastPortV0_attachnative(rpv0, native);
    return rpv0;
}

void abiv0_SetFont(struct RastPortV0 *rp, struct TextFontV0 *textFont, struct GfxBaseV0 *GfxBaseV0)
{
    if (textFont)
    {
        // 32-bit part
        rp->Font       = (APTR32)(IPTR)textFont;
        rp->TxWidth    = textFont->tf_XSize;
        rp->TxHeight   = textFont->tf_YSize;
        rp->TxBaseline = textFont->tf_Baseline;

        // 64-bit part
        struct RastPort *rpnative = RastPortV0_getnative(rp);
        struct TextFont *tfnative = ((struct TextFontProxy *)textFont)->native;
        if (rpnative == NULL)
        {
bug("abiv0_SetFont: NO NATIVE RASTPORT\n");
return;
        }
        SetFont(rpnative, tfnative);
    }
}
MAKE_PROXY_ARG_3(SetFont)

void abiv0_SetDrMd(struct RastPortV0 *rp, ULONG drawMode, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    if (rpnative == NULL)
    {
bug("abiv0_SetFDrMd: NO NATIVE RASTPORT\n"); // TextEditor.mcc
return;
    }
    SetDrMd(rpnative, drawMode);
}
MAKE_PROXY_ARG_3(SetDrMd)

void abiv0_SetAPen(struct RastPortV0 *rp, ULONG pen, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    if (rpnative == NULL)
    {
bug("abiv0_SetAPen: NO NATIVE RASTPORT\n"); // TextEditor.mcc
return;
    }
    SetAPen(rpnative, pen);
}
MAKE_PROXY_ARG_3(SetAPen)

ULONG abiv0_GetAPen(struct RastPortV0 * rp, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    return GetAPen(rpnative);
}
MAKE_PROXY_ARG_2(GetAPen)

ULONG abiv0_GetOutlinePen(struct RastPortV0 *rp,  struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    return GetOutlinePen(rpnative);
}
MAKE_PROXY_ARG_2(GetOutlinePen)

void abiv0_SetBPen(struct RastPortV0 *rp, ULONG pen, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    SetBPen(rpnative, pen);
}
MAKE_PROXY_ARG_3(SetBPen)

void abiv0_SetABPenDrMd(struct RastPortV0 *rp, ULONG apen, ULONG bpen, ULONG drawMode, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
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

void abiv0_SetRPAttrsA(struct RastPortV0 *rp, struct TagItemV0 *tags, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
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

void abiv0_GetRPAttrsA(struct RastPortV0 *rp, struct TagItemV0 *tags, struct GfxBaseV0 *GfxBaseV0)
{
    struct RastPort *rpnative = RastPortV0_getnative(rp);
    struct TagItemV0 *tmp;

    tmp = (struct TagItemV0 *)tags;
    do
    {
        switch(tmp->ti_Tag)
        {
            case(RPTAG_APen): *((ULONG *)(IPTR)tmp->ti_Data) = (ULONG)GetAPen(rpnative); break;
            case(RPTAG_BPen): *((ULONG *)(IPTR)tmp->ti_Data) = (ULONG)GetBPen(rpnative); break;
            case(RPTAG_DrMd): *((ULONG *)(IPTR)tmp->ti_Data) = (ULONG)GetDrMd(rpnative); break;
            case(TAG_DONE): break;
            default:
bug("GetRPAttr unhandled tag %x\n", tmp->ti_Tag);
asm("int3");
        }
    } while (LibNextTagItemV0(&tmp) != NULL);
}
MAKE_PROXY_ARG_3(GetRPAttrsA)

void Graphics_Rastports_init(struct GfxBaseV0 *abiv0GfxBase, APTR32 *graphicsjmp)
{
    __AROS_SETVECADDRV0(abiv0GfxBase,  11, (APTR32)(IPTR)proxy_SetFont);
    __AROS_SETVECADDRV0(abiv0GfxBase,  59, (APTR32)(IPTR)proxy_SetDrMd);
    __AROS_SETVECADDRV0(abiv0GfxBase,  57, (APTR32)(IPTR)proxy_SetAPen);
    __AROS_SETVECADDRV0(abiv0GfxBase, 149, (APTR32)(IPTR)proxy_SetABPenDrMd);
    __AROS_SETVECADDRV0(abiv0GfxBase,  58, (APTR32)(IPTR)proxy_SetBPen);
    __AROS_SETVECADDRV0(abiv0GfxBase, 173, (APTR32)(IPTR)proxy_SetRPAttrsA);
    __AROS_SETVECADDRV0(abiv0GfxBase, 143, (APTR32)(IPTR)proxy_GetAPen);
    __AROS_SETVECADDRV0(abiv0GfxBase, 146, (APTR32)(IPTR)proxy_GetOutlinePen);
    __AROS_SETVECADDRV0(abiv0GfxBase, 174, (APTR32)(IPTR)proxy_GetRPAttrsA);
    __AROS_SETVECADDRV0(abiv0GfxBase,  33, graphicsjmp[202 -  33]);  // InitRastPort
    __AROS_SETVECADDRV0(abiv0GfxBase,  14, graphicsjmp[202 -  14]);  // AskSoftStyle
    __AROS_SETVECADDRV0(abiv0GfxBase,  15, graphicsjmp[202 -  15]);  // SetSoftStyle
    __AROS_SETVECADDRV0(abiv0GfxBase, 177, graphicsjmp[202 - 177]);  // CreateRastPort
    __AROS_SETVECADDRV0(abiv0GfxBase, 180, graphicsjmp[202 - 180]);  // FreeRastPort
}
