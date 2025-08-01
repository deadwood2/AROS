/*
    Copyright (C) 1995-2020, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/cia.h>

#include <exec/libraries.h>
#include <exec/lists.h>
#include <exec/types.h>
#include <hardware/custom.h>
#include <hardware/intbits.h>
#include <hardware/cia.h>
#include <hidd/gfx.h>
#include <graphics/modeid.h>

#include "amigavideo_hidd.h"
#include "amigavideo_bitmap.h"
#include "chipset.h"

#define BPLCONMASKFULL          0xFA55
#define BPLCONMASK              0x8A55

#define DEFAULT_BORDER_GRAY     0x0AAA

static const UBYTE fetchunits[] = { 3,3,3,0, 4,3,3,0, 5,4,3,0 };
static const UBYTE fm_maxplanes[] = { 3,2,1,0, 3,3,2,0, 3,3,3,0 };

/* reset to OCS defaults */
VOID resetcustom(struct amigavideo_staticdata *csd)
{
    volatile struct Custom *custom = (struct Custom*)0xdff000;
    struct GfxBase *GfxBase = (APTR)csd->cs_GfxBase;

    D(
      bug("[AmigaVideo] %s()\n", __func__);
      bug("[AmigaVideo] %s: GfxBase @ 0x%p\n", __func__, GfxBase);
     )

    GfxBase->system_bplcon0 &= ~BPLCONMASKFULL;
    GfxBase->system_bplcon0 |= 0x0200;
    D(bug("[AmigaVideo] %s: system_bplcon0 = %04x\n", __func__, GfxBase->system_bplcon0));

    if (csd->ecs_denise) {
        custom->fmode = 0x0000;
    }
    custom->bplcon0 = GfxBase->system_bplcon0;
    if (csd->ecs_denise) {
        csd->bplcon0_null = GfxBase->system_bplcon0 | 0x1;
    }
    else {
        csd->bplcon0_null = GfxBase->system_bplcon0;
    }
    csd->bplcon1 = 0x0000;
    custom->bplcon1 = csd->bplcon1;
    csd->bplcon2 = 0x0024;
    custom->bplcon2 = csd->bplcon2;
    csd->bplcon3 = 0x0c00;          /* set PF2OFx to default offset value 8 (plane 3 affected) */
    custom->bplcon3 = csd->bplcon3;

    // Use AGA modes and create AGA copperlists only if AGA is "enabled"
    csd->aga_enabled = csd->aga && (GfxBase->ChipRevBits0 & SETCHIPREV_AA) == SETCHIPREV_AA;

    csd->bplcon4 = 0x0011;          /* set OSPRM7 to default */
    if (csd->aga_enabled) {
        custom->bplcon4 = csd->bplcon4;
    }
    custom->vposw = 0x8000;
    custom->color[0] = DEFAULT_BORDER_GRAY;
}

static VOID waitvblank(struct amigavideo_staticdata *csd)
{
    // ugly busy loop for now..
    UWORD fc = csd->framecounter;
    while (fc == csd->framecounter);
}
 
static VOID setnullsprite(struct amigavideo_staticdata *csd)
{
    if (csd->copper1_spritept) {
        UWORD *p = csd->sprite_null;
        csd->copper1_spritept[0] = (UWORD)(((ULONG)p) >> 16);
        csd->copper1_spritept[2] = (UWORD)(((ULONG)p) >> 0);
    }
 }

static VOID new_setnullsprite(struct amigavideo_staticdata *csd, int spritenum)
{
    if (csd->new_copper1_spritept[spritenum]) {
        UWORD *p = csd->sprite_null;
        csd->new_copper1_spritept[spritenum][0] = (UWORD)(((ULONG)p) >> 16);
        csd->new_copper1_spritept[spritenum][2] = (UWORD)(((ULONG)p) >> 0);
    }
 }
 
VOID resetsprite(struct amigavideo_staticdata *csd)
{
    UWORD *sprite = csd->sprite;
    setnullsprite(csd);
    csd->sprite = NULL;
    FreeMem(sprite, csd->spritedatasize);
    csd->sprite_width = csd->sprite_height = 0;
}

VOID new_resetsprite(struct amigavideo_staticdata *csd, int spritenum)
{
    UWORD *sprite = csd->new_sprite[spritenum];
    new_setnullsprite(csd, spritenum);
    csd->new_sprite[spritenum] = NULL;
    FreeMem(sprite, csd->new_spritedatasize[spritenum]);
    csd->new_sprite_width[spritenum] = csd->new_sprite_height[spritenum] = 0;
}

VOID setfmode(struct amigavideo_staticdata *csd, struct amigabm_data *bm)
{
    UWORD fmode;
    fmode  =  csd->fmode_bpl == 2 ? 3 : csd->fmode_bpl;
    fmode |= (csd->fmode_spr == 2 ? 3 : csd->fmode_spr) << 2;
    if (bm && bm->copld.copper2_fmode) {
        *bm->copld.copper2_fmode = fmode;
        if (bm->interlace)
            *bm->copsd.copper2_fmode = fmode;
    }
    // Update sprite fmode in copper list 1 so the control words
    // are read correctly.
    csd->copper1[15] = fmode & 0xc;
}

static void updatecopper1frombm(struct amigavideo_staticdata *, struct amigabm_data *);

VOID setcoppercolors(struct amigavideo_staticdata *csd, struct amigabm_data *bm, UBYTE *palette, BOOL update_copper1)
{
    struct copper2data *c2d = &bm->copld, *c2di = &bm->copsd;

    D(bug("[AmigaVideo] %s()\n", __func__));

    if (!(palette))
    {
        D(bug("[AmigaVideo] %s: missing palette data!\n", __func__));
        return;
    }
    if (!c2d->copper2_palette)
        return;

    UWORD i;

    if (csd->aga && csd->aga_enabled) {
        UWORD off = 1;
        D(bug("[AmigaVideo] %s: AGA\n", __func__));
        for (i = 0; i < bm->use_colors; i++) {
            UWORD vallo, valhi;
            UBYTE r = palette[i * 3 + 0];
            UBYTE g = palette[i * 3 + 1];
            UBYTE b = palette[i * 3 + 2];
            if ((i & 31) == 0)
                off += 2;
            valhi = ((r & 0xf0) << 4) | ((g & 0xf0)) | ((b & 0xf0) >> 4);
            vallo = ((r & 0x0f) << 8) | ((g & 0x0f) << 4) | ((b & 0x0f));
            c2d->copper2_palette[i * 2 + off] = valhi;
            c2d->copper2_palette_aga_lo[i * 2 + off] = vallo;
            if (bm->interlace) {
                c2di->copper2_palette[i * 2 + off] = valhi;
                c2di->copper2_palette_aga_lo[i * 2 + off] = vallo;
            }
        }
    } else if (bm->res == 2 && !csd->aga) {
        D(bug("[AmigaVideo] %s: ECS\n", __func__));
        /* ECS "scrambled" superhires */
        for (i = 0; i < bm->use_colors; i++) {
            UBYTE offset = i < 16 ? 0 : 16;
            UBYTE c1 = (i & 3) + offset;
            UBYTE c2 = ((i >> 2) & 3) + offset;
            UWORD val1 = ((palette[c1 * 3 + 0] >> 4) << 8) | ((palette[c1 * 3 + 1] >> 4) << 4) | ((palette[c1 * 3 + 2] >> 4) << 0);
            UWORD val2 = ((palette[c2 * 3 + 0] >> 4) << 8) | ((palette[c2 * 3 + 1] >> 4) << 4) | ((palette[c2 * 3 + 2] >> 4) << 0);
            UWORD val = (val1 & 0xccc) | ((val2 & 0xccc) >> 2);
            c2d->copper2_palette[i * 2 + 1] = val;
            if (bm->interlace)
                c2di->copper2_palette[i * 2 + 1] = val;
        }
        
    } else {
        for (i = 0; i < bm->use_colors; i++) {
            UWORD val = ((palette[i * 3 + 0] >> 4) << 8) | ((palette[i * 3 + 1] >> 4) << 4) | ((palette[i * 3 + 2] >> 4) << 0);
            c2d->copper2_palette[i * 2 + 1] = val;
            if (bm->interlace)
                c2di->copper2_palette[i * 2 + 1] = val;
        }
    }
    if (update_copper1) {
        updatecopper1frombm(csd, bm);
    }
    D(bug("[AmigaVideo] %s: copper colors set\n", __func__));
}

VOID resetmode(struct amigavideo_staticdata *csd)
{
    volatile struct Custom *custom = (struct Custom*)0xdff000;
    struct GfxBase *GfxBase = (APTR)csd->cs_GfxBase;

    D(bug("[AmigaVideo] %s()\n", __func__));

    custom->dmacon = 0x0100;
    csd->palmode = (GfxBase->DisplayFlags & NTSC) == 0;
    setpalntsc(csd);

    custom->cop2lc = (ULONG)csd->copper2_backup;
    custom->copjmp2 = 0;

    waitvblank(csd);

    GfxBase->LOFlist = GfxBase->SHFlist = csd->copper2_backup;

    resetcustom(csd);
}

static VOID setcopperplanes(struct amigavideo_staticdata *csd, struct amigabm_data *bm,  struct copper2data *c2d, LONG offset, BOOL odd)
{
    UWORD *copbpl = c2d->copper2_bpl;
    WORD i;

    for (i = 0; i < bm->depth; i++) {
        ULONG pptr = (ULONG)(bm->pbm->Planes[bm->bploffsets[i]]);
        if (bm->interlace && odd)
            pptr += bm->bytesperrow;
        pptr += offset;
        copbpl[1] = (UWORD)(pptr >> 16);
        copbpl[3] = (UWORD)(pptr >> 0);
        copbpl += 4;
    }
}

static VOID setcopperscroll2(struct amigavideo_staticdata *csd, struct amigabm_data *bm, struct copper2data *c2d, UWORD *c2, BOOL odd)
{
    UWORD *copptr = c2d->copper2_scroll;
    WORD xscroll, yscroll;
    WORD x, y;
    WORD ystart, yend, xdelay;
    WORD xmaxscroll,  modulo, ddfstrt, fmodewidth, minearly;
    LONG offset;

    D(bug("[AmigaVideo] %s(0x%p)\n", __func__, c2));

    fmodewidth = 16 << csd->fmode_bpl;
    x = bm->leftedge;
    y = csd->starty + (bm->topedge >> bm->interlace);

    yscroll = 0;
    if (y < 10) {
        yscroll = y - 10;
        y = 10;
    }

    xmaxscroll = 1 << (1 + csd->fmode_bpl);
    xdelay = x & (fmodewidth - 1);
    xscroll = -x;

    yend = y + (bm->displayheight >> bm->interlace);
    yend = limitheight(csd, yend, FALSE, TRUE);
    ystart = y - c2d->extralines;

    modulo = (bm->interlace ? bm->bytesperrow : 0) + bm->modulo;
    ddfstrt = bm->ddfstrt;

    offset = ((xscroll + (xmaxscroll << 3) - 1) >> 3) & ~(xmaxscroll - 1);
    offset -= (yscroll * bm->bytesperrow) << (bm->interlace ? 1 : 0);

    minearly = 1 << fetchunits[csd->fmode_bpl * 4 + bm->res];
    if (xdelay) {
        ddfstrt -= minearly;
        modulo -= (minearly << bm->res) / 4;
        offset -= (minearly << bm->res) / 4;
    }

    // DIWSTRT
    copptr[1] = (y << 8) | (csd->startx); //(y << 8) + (x + 1);
    // DIWSTOP
    copptr = c2d->copper2_diwstop;
    copptr[1] = (yend << 8) | ((csd->startx + 0x140) & 0xff); //((y + (bm->rows >> bm->interlace)) << 8) + ((x + 1 + (bm->width >> bm->res)) & 0x00ff);

    /* update the bitmaps copper plane-pointers */
    setcopperplanes(csd, bm, c2d, offset, odd);

    xdelay <<= 2 - bm->res;
    // BPLCON1
    copptr[7] =
          (((xdelay >> 2) & 0x0f) << 0) | (((xdelay >> 2) & 0x0f) << 4)
        | ((xdelay >> 6) << 10) | ((xdelay >> 6) << 14)
        | ((xdelay & 3) << 8) | ((xdelay & 3) << 12);

    // DDFSTRT
    copptr[3] = ddfstrt;
    // DDFSTOP
    copptr[5] = bm->ddfstop;
    // Modulo (odd)
    copptr[9] = modulo;
    // Modulo (even)
    copptr[11] = modulo;

    // DIWHIGH
    if (csd->ecs_denise) {
        if ((copptr = c2d->copper2_fmode) != NULL)
        {
            copptr[-2] = ((y >> 8) & 7) | (((yend >> 8) & 7) << 8) | 0x2000;
        }
        else
        {
            copptr = (APTR)((IPTR)c2 + (bm->bmcl->Count << 2));
            copptr[-1] = ((y >> 8) & 7) | (((yend >> 8) & 7) << 8) | 0x2000;
        }
    }

    /* adjust start wait */
    copptr = c2;
    if (ystart >= (REZ_Y_MIN + REZ_PAL_LINES))
        copptr[0] = 0xffdf;
    else
        copptr[0] = 0x01fe;  // NOP
    copptr[2] = (ystart << 8) | 0x05;

    copptr = c2d->copper2_tail;
    if (copptr[0] != 0x0084)
    {
        /* adjust the screen end wait's ... */
        yend = y + (bm->displayheight >> bm->interlace) + yscroll;
        yend = limitheight(csd, yend, FALSE, TRUE);
        copptr[2] = (yend << 8) | 0x05;
        if (yend < (REZ_Y_MIN + REZ_PAL_LINES) || ystart >= (REZ_Y_MIN + REZ_PAL_LINES)) {
            copptr[0] = 0x00df;
            copptr[1] = 0x00fe;
        } else {
            copptr[0] = 0xffdf;
            copptr[1] = 0xfffe;
        }
    }
}

VOID setcopperscroll(struct amigavideo_staticdata *csd, struct amigabm_data *bm, BOOL interlaced)
{
    if (bm->bmcl)
    {
        setcopperscroll2(csd, bm, &bm->copld, bm->bmcl->CopLStart, FALSE);
        if (interlaced)
            setcopperscroll2(csd, bm, &bm->copsd, bm->bmcl->CopSStart, bm->interlace);
    }
}

UWORD get_copper_list_length(struct amigavideo_staticdata *csd, UBYTE depth)
{
    UWORD v;

    if (csd->aga && csd->aga_enabled) {
        v = 1000 + ((1 << depth) + 1 + (1 << depth) / 32 + 1) * 2;
    } else {
        v = 1000;
    }
    return v * 2;
}

VOID updatebmbplcon(struct amigavideo_staticdata *csd, struct amigabm_data *bm, struct copper2data *c2d)
{
    UWORD *c;
    UWORD bplcon3;

    D(bug("[AmigaVideo] %s()\n", __func__));

    bplcon3 = csd->bplcon3 | bm->bplcon3 | ((bm->sprite_res + 1) << 6);

    // update AGA only palette values ...
#if (0)
    if (csd->aga && csd->aga_enabled) {
        c = c2d->copper2_palette;
        c[1] = bm->bplcon3 | ((i / 32) << 13);
        c2d->copper2_palette_aga_lo = c;
        c[1] = bm->bplcon3 | ((i / 32) << 13) | 0x0200;
        c = (UWORD *)((IPTR)c2d->copper2_palette_aga_lo + ((bm->use_colors + 1) << 2));
        c[1] = bm->bplcon3;
    }
#endif

    c = c2d->copper2_scroll;
    if (csd->aga && csd->aga_enabled) {
        /* update (BPLCON4) bitplane display masks  */
        c[-1] = csd->bplcon4;
        D(bug("[AmigaVideo] %s: bm bplcon4 = %04x\n", __func__, c[-1]));
    }

    c[5] = csd->bplcon2 | ((csd->aga && !(bm->modeid & EXTRAHALFBRITE_KEY)) ? 0x0200 : 0);
    c[7] = csd->bplcon3;
    D(
      bug("[AmigaVideo] %s: bm bplcon3 = %04x\n", __func__, c[7]);
      bug("[AmigaVideo] %s: bm bplcon2 = %04x\n", __func__, c[5]);
     )
}

UWORD *populatebmcopperlist(struct amigavideo_staticdata *csd, struct amigabm_data *bm, struct copper2data *c2d, UWORD *c2, BOOL lace)
{
    struct GfxBase *GfxBase = (struct GfxBase *)csd->cs_GfxBase;
    volatile UWORD *system_bplcon0 = (volatile UWORD *)&GfxBase->system_bplcon0;
    UWORD *c = c2;
    UWORD i;
    UWORD bplcon0, bplcon0_res, bplcon0_null, bplcon3;
    ULONG pptr;

    D(bug("[AmigaVideo] %s()\n", __func__));

    D(bug("[AmigaVideo] %s: Copperlist%d @ 0x%p\n", __func__,  lace ? 2 : 1, c));

    bplcon0_res = *system_bplcon0 & ~BPLCONMASK;
    D(bug("[AmigaVideo] %s: initial bplcon0 = %04x\n", __func__, bplcon0_res));

    if (bm->res == 1)
         bplcon0_res |= 0x8000;
    else if (bm->res == 2)
        bplcon0_res |= 0x0040;
    else
        bplcon0_res = 0;

    bplcon0_null = csd->bplcon0_null | (bm->interlace ? 4 : 0) | bplcon0_res;
    bplcon3 = csd->bplcon3 | bm->bplcon3 | ((bm->sprite_res + 1) << 6);

    D(bug("[AmigaVideo] %s: bplcon0_null = %04x\n", __func__, bplcon0_null));
    D(bug("[AmigaVideo] %s: bm bplcon3 = %04x\n", __func__, bplcon3));

    /* NOP + WAIT - to store start co-ords */
    COPPEROUT(c, 0x01fe, 0xfffe)
    COPPEROUT(c, 0xffff, 0xfffe)

    bm->use_colors = 1 << bm->depth;

    // need to update sprite colors
    if (bm->use_colors < 16 + 4)
        bm->use_colors = 16 + 4;
    if (bm->res == 2 && !csd->aga)
        bm->use_colors = 32; /* ECS "scrambled" superhires */
    
    if (bm->use_colors > 32 && (bm->modeid & EXTRAHALFBRITE_KEY))
        bm->use_colors = 32;
    if (bm->modeid & HAM_KEY) {
        if (bm->depth <= 6)
            bm->use_colors = 16 + 4;
        else
            bm->use_colors = 64;
    }

    // Push the palette registers ...
    c2d->copper2_palette = c;
    if (csd->aga && csd->aga_enabled) {
        // hi
        for (i = 0; i < bm->use_colors; i++) {
            UBYTE agac = i & 31;
            if (agac == 0) {
                COPPEROUT(c, 0x0106, bm->bplcon3 | ((i / 32) << 13))
            }
            COPPEROUT(c, 0x0180 + agac * 2, 0x0000)
        }
        c2d->copper2_palette_aga_lo = c;
        // lo
        for (i = 0; i < bm->use_colors; i++) {
            UBYTE agac = i & 31;
            if (agac == 0) {
                COPPEROUT(c, 0x0106, bm->bplcon3 | ((i / 32) << 13) | 0x0200)
            }
            COPPEROUT(c, 0x0180 + agac * 2, 0x000)
        }
        COPPEROUT(c, 0x106, bm->bplcon3)
    } else {
        // ocs/ecs
        for (i = 0; i < bm->use_colors; i++) {
            COPPEROUT(c, 0x180 + i * 2, 0x000)
        }
    }

    if (csd->aga && csd->aga_enabled) {
        /* Push (BPLCON4) bitplane display masks  */
        COPPEROUT(c, 0x010c, csd->bplcon4)
    }

    bplcon0 = bplcon0_null;
    if (bm->depth > 7)
        bplcon0 |= 0x0010;
    else
        bplcon0 |= bm->depth << 12;
    if (bm->modeid & HAM_KEY)
        bplcon0 |= 0x0800;
    D(bug("[AmigaVideo] %s: copper bplcon0 = %04x\n", __func__, bplcon0));

    c2d->copper2_scroll = c;

    COPPEROUT(c, 0x008e, 0)                     // Push (DIWSTRT) Display window start

    COPPEROUT(c, 0x0100, bplcon0)               // Push the screens bplcon0
    COPPEROUT(c, 0x0104, csd->bplcon2 | ((csd->aga && !(bm->modeid & EXTRAHALFBRITE_KEY)) ? 0x0200 : 0))  // Push the screens bplcon2
    if (csd->ecs_denise) {
        COPPEROUT(c, 0x0106, bplcon3)           // Push the screens bplcon3
    }

    c2d->copper2_diwstop = c;
    COPPEROUT(c, 0x0090, 0)                     // Push (DIWSTOP) Display window stop
    COPPEROUT(c, 0x0092, 0)                     // Push (DDFSTRT) Display bitplane data fetch start
    COPPEROUT(c, 0x0094, 0)                     // Push (DDFSTOP) Display bitplane data fetch stop
    COPPEROUT(c, 0x0102, 0)                     // Push (BPLCON1) Bitplane scroll control reg
    COPPEROUT(c, 0x0108, 0)                     // Push Bitplane modulo (odd planes)
    COPPEROUT(c, 0x010a, 0)                     // Push Bitplane modulo (even planes)

    c2d->copper2_bpl = c;
    for (i = 0; i < bm->depth; i++) {
        pptr = (ULONG)(bm->pbm->Planes[bm->bploffsets[i]]);
        if (lace)
            pptr += bm->bytesperrow;
        COPPEROUT(c, 0x00e0 + i * 4, (UWORD)(pptr >> 16)) // Push the bitplane registers
        COPPEROUT(c, 0x00e2 + i * 4, (UWORD)(pptr >> 0))
    }

    if (csd->ecs_denise) {
        COPPEROUT(c, 0x01e4, 0)                 // Push (DIWHIGH) Display window
    }
    c2d->copper2_fmode = NULL;
    if (csd->aga && csd->aga_enabled) {
        COPPEROUT(c, 0x01fc, 0)
        c2d->copper2_fmode = &c[-1];
    }

    c2d->extralines = (((IPTR)c - (IPTR)c2) / 112) + 1;

    /* store the pointer to the copperlist data tail so it can be adjusted for linking chains */
    c2d->copper2_tail = c;

    return c;
}

VOID setcopperlisttail(struct amigavideo_staticdata *csd, UWORD *copper2tail, UWORD *c2next, BOOL jmp)
{
    UWORD *c = copper2tail;

    D(bug("[AmigaVideo] %s(0x%p)\n", __func__, c2next));
    D(bug("[AmigaVideo] %s: tail @ 0x%p\n", __func__, c));

    if (!jmp)
    {
        COPPEROUT(c, 0xffff, 0xfffe)
        COPPEROUT(c, 0xffff, 0xfffe)
    }
    if (c2next)
    {
        COPPEROUT(c, 0x0084, (UWORD)((IPTR)c2next >> 16))
        COPPEROUT(c, 0x0086, (UWORD)((IPTR)c2next >> 0))
        if (jmp)
        {
            D(bug("[AmigaVideo] %s: pushing COPJMP2\n", __func__));
            COPPEROUT(c, 0x008A, 0x0000)
        }
    }
    COPPEROUT(c, 0xffff, 0xfffe)                // END
}

BOOL setbitmap(struct amigavideo_staticdata *csd, struct amigabm_data *bm)
{
    D(bug("[AmigaVideo] %s()\n", __func__));

    bm->modulo = bm->bytesperrow - bm->modulopre / (4 >> bm->res);
    bm->modulo &= ~((2 << csd->fmode_bpl) - 1);

    setcopperscroll(csd, bm, ((csd->interlaced == TRUE) || (bm->interlace == TRUE)));

    D(bug("[AmigaVideo] %s: bm=%x mode=%08x w=%d h=%d d=%d bpr=%d\n",
        __func__, bm, bm->modeid, bm->width, bm->height, bm->depth, bm->bytesperrow));
        return TRUE;
}

BOOL setmode(struct amigavideo_staticdata *csd, struct amigabm_data *bm)
{
    volatile struct Custom *custom = (struct Custom*)0xdff000;
    struct GfxBase *GfxBase = (APTR)csd->cs_GfxBase;
    UWORD *c;
    UWORD ddfstrt, ddfstop;
    UBYTE fetchunit, maxplanes;
    UWORD bplwidth, viewwidth;
    UBYTE i;

    D(bug("[AmigaVideo] %s(0x%p)\n", __func__, bm));

    csd->fmode_bpl = csd->aga && csd->aga_enabled ? 2 : 0;

    fetchunit = fetchunits[csd->fmode_bpl * 4 + bm->res];
    maxplanes = fm_maxplanes[csd->fmode_bpl * 4 + bm->res];

    D(bug("[AmigaVideo] %s: res %d fmode %d depth %d maxplanes %d aga %d agae %d\n",
        __func__, bm->res, csd->fmode_bpl, bm->depth, maxplanes, csd->aga, csd->aga_enabled));

    if (bm->depth > (1 << maxplanes)) {
        if (csd->aga && !csd->aga_enabled) {
            // Enable AGA if requesting AGA only mode.
            // This is a compatibility hack because our display
            // database currently contains all AGA modes even if AGA
            // is "disabled".
            GfxBase->ChipRevBits0 = SETCHIPREV_AA | GFXF_AA_MLISA;
            csd->aga_enabled = TRUE;
            csd->fmode_bpl = 2;
            fetchunit = fetchunits[csd->fmode_bpl * 4 + bm->res];
            maxplanes = fm_maxplanes[csd->fmode_bpl * 4 + bm->res];
        }
        if (bm->depth > (1 << maxplanes))
            return FALSE;
    }

    viewwidth = bm->width;
    // use nominal width for now
    if ((viewwidth << bm->res) > 320)
        viewwidth = 320 << bm->res;

    D(bug("[AmigaVideo] %s:  mode %08x (%dx%dx%d) bpr=%d fu=%d\n",
        __func__, bm->modeid, bm->width, bm->height, bm->depth, bm->bytesperrow, fetchunit));

    bplwidth = viewwidth >> (bm->res + 1);
    ddfstrt = (csd->startx / 2) & ~((1 << fetchunit) - 1);
    ddfstop = ddfstrt + ((bplwidth + ((1 << fetchunit) - 1) - 2 * (1 << fetchunit)) & ~((1 << fetchunit) - 1));
    bm->modulopre = ddfstop + 2 * (1 << fetchunit) - ddfstrt;
    ddfstrt -= 1 << maxplanes;
    bm->ddfstrt = ddfstrt;
    bm->ddfstop = ddfstop;

    if ((bm->modeid & HAM_KEY) && bm->depth > 6) {
        bm->bploffsets[0] = 6;
        bm->bploffsets[1] = 7;
        for (i = 0; i < 6; i++)
            bm->bploffsets[i + 2] = i;
    }
    else
    {
        for (i = 0; i < 8; i++)
            bm->bploffsets[i] = i;
    }

    setfmode(csd, bm);
    bm->displaywidth = viewwidth;

    setbitmap(csd, bm);

    setspritepos(csd, csd->spritex, csd->spritey, bm->res, bm->interlace);
    
    for(i=1;i<8;i++)
      new_setspritepos(csd, csd->new_spritex[i], csd->new_spritey[i], bm->res, bm->interlace, i);

    D(bug("[AmigaVideo] %s: done\n", __func__));

    return 1;
 }

UBYTE av__PickPen(struct amigavideo_staticdata *csd, ULONG pixel)
{
    UBYTE retval = 1;
    return retval;
}

BOOL setsprite(OOP_Class *cl, OOP_Object *o, WORD width, WORD height, struct pHidd_Gfx_SetCursorShape *msg)
{
    struct amigavideo_staticdata *csd = CSD(cl);
    struct Library *OOPBase = csd->cs_OOPBase;
    OOP_MethodID HiddGfxBase = csd->cs_HiddGfxBase;
    OOP_MethodID HiddBitMapBase = csd->cs_HiddBitMapBase;
    struct amigabm_data *data = OOP_INST_DATA(cl, o);
    OOP_Object *bmPFObj = NULL;
    HIDDT_PixelFormat *bmPF;
    IPTR pf, bmcmod;
    UWORD fetchsize;
    UWORD bitmapwidth = width;
    UWORD y, *p;

    D(bug("[AmigaVideo] %s()\n", __func__));

    OOP_GetAttr(msg->shape, aHidd_BitMap_PixFmt, (IPTR*)&bmPFObj);
    OOP_GetAttr(bmPFObj, aHidd_PixFmt_ColorModel, &bmcmod);
    if (bmcmod == vHidd_ColorModel_TrueColor)
    {
        OOP_GetAttr(bmPFObj, aHidd_PixFmt_StdPixFmt, (IPTR*)&pf);
        bmPF = (HIDDT_PixelFormat *)HIDD_Gfx_GetPixFmt(o, pf);
    }

    if (csd->aga && csd->aga_enabled && width > 16)
        csd->fmode_spr = 2;
    else
        csd->fmode_spr = 0;
    D(bug("[AmigaVideo] %s: fmode_spr = %x\n", __func__, csd->fmode_spr));
    fetchsize = 2 << csd->fmode_spr;
    width = 16 << csd->fmode_spr;

    if (width != csd->sprite_width || height != csd->sprite_height) {
        resetsprite(csd);
        csd->spritedatasize = fetchsize * 2 + fetchsize * height * 2 + fetchsize * 2;
        csd->sprite = AllocMem(csd->spritedatasize, MEMF_CHIP | MEMF_CLEAR);
        if (!csd->sprite)
            return FALSE;
        csd->sprite_width = width;
        csd->sprite_height = height;
        csd->sprite_offset_x = msg->xoffset;
        csd->sprite_offset_y = msg->yoffset;
    }
    p = csd->sprite;
    p += fetchsize;
    for(y = 0; y < height; y++) {
        UWORD xx, xxx, x;
        for (xx = 0, xxx = 0; xx < width; xx += 16, xxx++) {
            UWORD pix1 = 0, pix2 = 0;
            for(x = 0; x < 16; x++) {
                UBYTE c = 0;
                if (xx + x < bitmapwidth)
                {
                    if (bmcmod != vHidd_ColorModel_TrueColor)
                        c = HIDD_BM_GetPixel(msg->shape, xx + x, y);
                    else
                    {
                        HIDDT_Pixel pix = HIDD_BM_GetPixel(msg->shape, xx + x, y);
                        c = 0;
                        if ((ALPHA_COMP(pix, bmPF) & 0xFF00) == 0xFF00)
                            c = av__PickPen(csd, ((RED_COMP(pix, bmPF) & 0xFF00) << 8) | (GREEN_COMP(pix, bmPF) & 0xFF00) | ((BLUE_COMP(pix, bmPF) >> 8) & 0xFF));
                        else c = 0;
                    }
                }
                pix1 <<= 1;
                pix2 <<= 1;
                pix1 |= (c & 1) ? 1 : 0;
                pix2 |= (c & 2) ? 1 : 0;
            }
            p[xxx] = pix1;
            p[xxx + fetchsize / 2] = pix2;
        }
        p += fetchsize;
    }
    setspritepos(csd, csd->spritex, csd->spritey, data->res, data->interlace);
    setspritevisible(csd, csd->cursorvisible);
    return TRUE;
}

BOOL new_setsprite(OOP_Class *cl, OOP_Object *o, WORD width, WORD height, struct pHidd_AmigaGfx_SetSpriteShape *msg, int spritenum)
{
    struct amigavideo_staticdata *csd = CSD(cl);
    struct Library *OOPBase = csd->cs_OOPBase;
    OOP_MethodID HiddGfxBase = csd->cs_HiddGfxBase;
    OOP_MethodID HiddBitMapBase = csd->cs_HiddBitMapBase;
    struct amigabm_data *data = OOP_INST_DATA(cl, o);
    OOP_Object *bmPFObj = NULL;
    HIDDT_PixelFormat *bmPF;
    IPTR pf, bmcmod;
    UWORD fetchsize;
    UWORD bitmapwidth = width;
    UWORD y, *p;

    D(bug("[AmigaVideo] %s()\n", __func__));

    OOP_GetAttr(msg->shape, aHidd_BitMap_PixFmt, (IPTR*)&bmPFObj);
    OOP_GetAttr(bmPFObj, aHidd_PixFmt_ColorModel, &bmcmod);
    if (bmcmod == vHidd_ColorModel_TrueColor)
    {
        OOP_GetAttr(bmPFObj, aHidd_PixFmt_StdPixFmt, (IPTR*)&pf);
        bmPF = (HIDDT_PixelFormat *)HIDD_Gfx_GetPixFmt(o, pf);
    }

    if (csd->aga && csd->aga_enabled && width > 16)
        csd->fmode_spr = 2;
    else
        csd->fmode_spr = 0;
    D(bug("[AmigaVideo] %s: fmode_spr = %x\n", __func__, csd->fmode_spr));
    fetchsize = 2 << csd->fmode_spr;
    width = 16 << csd->fmode_spr;

    if (width != csd->new_sprite_width[spritenum] || height != csd->new_sprite_height[spritenum]) {
        new_resetsprite(csd, spritenum);
        csd->new_spritedatasize[spritenum] = fetchsize * 2 + fetchsize * height * 2 + fetchsize * 2;
        csd->new_sprite[spritenum] = AllocMem(csd->new_spritedatasize[spritenum], MEMF_CHIP | MEMF_CLEAR);
        if (!csd->new_sprite[spritenum])
            return FALSE;
        csd->new_sprite_width[spritenum] = width;
        csd->new_sprite_height[spritenum] = height;
        csd->new_sprite_offset_x[spritenum] = msg->xoffset;
        csd->new_sprite_offset_y[spritenum] = msg->yoffset;
    }
    p = csd->new_sprite[spritenum];
    p += fetchsize;
    for(y = 0; y < height; y++) {
        UWORD xx, xxx, x;
        for (xx = 0, xxx = 0; xx < width; xx += 16, xxx++) {
            UWORD pix1 = 0, pix2 = 0;
            for(x = 0; x < 16; x++) {
                UBYTE c = 0;
                if (xx + x < bitmapwidth)
                {
                    if (bmcmod != vHidd_ColorModel_TrueColor)
                        c = HIDD_BM_GetPixel(msg->shape, xx + x, y);
                    else
                    {
                        HIDDT_Pixel pix = HIDD_BM_GetPixel(msg->shape, xx + x, y);
                        c = 0;
                        if ((ALPHA_COMP(pix, bmPF) & 0xFF00) == 0xFF00)
                            c = av__PickPen(csd, ((RED_COMP(pix, bmPF) & 0xFF00) << 8) | (GREEN_COMP(pix, bmPF) & 0xFF00) | ((BLUE_COMP(pix, bmPF) >> 8) & 0xFF));
                        else c = 0;
                    }
                }
                pix1 <<= 1;
                pix2 <<= 1;
                pix1 |= (c & 1) ? 1 : 0;
                pix2 |= (c & 2) ? 1 : 0;
            }
            p[xxx] = pix1;
            p[xxx + fetchsize / 2] = pix2;
        }
        p += fetchsize;
    }
    new_setspritepos(csd, csd->spritex, csd->spritey, data->res, data->interlace, spritenum);
    new_setspritevisible(csd, csd->spritevisible[spritenum], spritenum);
    return TRUE;
}

VOID setspritepos(struct amigavideo_staticdata *csd, WORD x, WORD y, UBYTE res, BOOL interlace)
{
    UWORD ctl, pos;

    csd->spritex = x;
    csd->spritey = y;
    if (!csd->sprite || csd->sprite_height == 0)
        return;

    x += csd->sprite_offset_x << res;
    x <<= (2 - res); // convert x to shres coordinates
    x += (csd->startx - 1) << 2; // display left edge offset
 
    if (interlace)
        y >>= 1; // y is always in nonlaced
    y += csd->starty;
    y += csd->sprite_offset_y;

    pos = (y << 8) | (x >> 3);
    ctl = ((y + csd->sprite_height) << 8);
    ctl |= ((y >> 8) << 2) | (((y + csd->sprite_height) >> 8) << 1) | ((x >> 2) & 1) | ((x & 3) << 3);
    csd->spritepos = pos;
    csd->spritectl = ctl;
}

VOID new_setspritepos(struct amigavideo_staticdata *csd, WORD x, WORD y, UBYTE res, BOOL interlace, int spritenum)
{
    UWORD ctl, pos;
    volatile struct Custom *custom = (struct Custom*)0xdff000;

    csd->new_spritex[spritenum] = x;
    csd->new_spritey[spritenum] = y;
    if (!csd->new_sprite[spritenum] || csd->new_sprite_height[spritenum] == 0)
        return;

    x += csd->new_sprite_offset_x[spritenum] << res;
    x <<= (2 - res); // convert x to shres coordinates
    x += (csd->startx - 1) << 2; // display left edge offset

    if (interlace)
        y >>= 1; // y is always in nonlaced
    y += csd->starty;
    y += csd->new_sprite_offset_y[spritenum];

    pos = (y << 8) | (x >> 3);
    ctl = ((y + csd->new_sprite_height[spritenum]) << 8);
    ctl |= ((y >> 8) << 2) | (((y + csd->new_sprite_height[spritenum]) >> 8) << 1) | ((x >> 2) & 1) | ((x & 3) << 3);
    csd->new_spritepos[spritenum] = pos;
    csd->new_spritectl[spritenum] = ctl;
    custom->spr[spritenum].pos = pos;
    custom->spr[spritenum].ctl = ctl;
}

VOID setspritevisible(struct amigavideo_staticdata *csd, BOOL visible)
{
    D(bug("[AmigaVideo] %s()\n", __func__));

    csd->cursorvisible = visible;
    if (visible) {
        if (csd->copper1_spritept) {
            UWORD *p = csd->sprite;
            struct amigabm_data *bm;
            ForeachNode(csd->compositedbms, bm)
            {
                if (csd->spritey < ((bm->topedge + bm->displayheight) >> bm->interlace))
                {
                    setfmode(csd, bm);
                    break;
                }
            }
            csd->copper1_spritept[0] = (UWORD)(((ULONG)p) >> 16);
            csd->copper1_spritept[2] = (UWORD)(((ULONG)p) >> 0);
        }
    } else {
        setnullsprite(csd);
    }
}

VOID new_setspritevisible(struct amigavideo_staticdata *csd, BOOL visible, int spritenum)
{
    D(bug("[AmigaVideo] %s()\n", __func__));
    
    csd->spritevisible[spritenum] = visible;
    if (visible) {
        if (csd->new_copper1_spritept[spritenum]) {
            UWORD *p = csd->new_sprite[spritenum];
            struct amigabm_data *bm;
            ForeachNode(csd->compositedbms, bm)
            {
                if (csd->new_spritey[spritenum] < ((bm->topedge + bm->displayheight) >> bm->interlace))
                {
                    setfmode(csd, bm);
                    break;
                }
            }
            csd->new_copper1_spritept[spritenum][0] = (UWORD)(((ULONG)p) >> 16);
            csd->new_copper1_spritept[spritenum][2] = (UWORD)(((ULONG)p) >> 0);
        }
    } else {
        new_setnullsprite(csd, spritenum);
    }
}

BOOL setcolors(struct amigavideo_staticdata *csd, struct amigabm_data *bm, struct pHidd_BitMap_SetColors *msg)
{
    D(bug("[AmigaVideo] %s()\n", __func__));

    if (!(bm->palette))
    {
        D(bug("[AmigaVideo] %s: missing palette data!\n", __func__));
        return FALSE;
    }

    if (msg->firstColor + msg->numColors > csd->max_colors)
    {
        D(bug("[AmigaVideo] %s: too many colors specified!\n", __func__));
        return FALSE;
    }

    UWORD i, j = 0;

    for (i = msg->firstColor; j < msg->numColors; i++, j++) {
        UBYTE red, green, blue;
        red   = msg->colors[j].red   >> 8;
        green = msg->colors[j].green >> 8;
        blue  = msg->colors[j].blue  >> 8;
        bm->palette[i * 3 + 0] = red;
        bm->palette[i * 3 + 1] = green;
        bm->palette[i * 3 + 2] = blue;
        //bug("%d: %02x %02x %02x\n", i, red, green, blue);
    }

    if ((bm->bmcl) && (bm->bmcl->CopLStart))
        setcoppercolors(csd, bm, bm->palette,
                        (void *)bm == (void *)csd->compositedbms->lh_Head);

    return TRUE;
}

VOID setscroll(struct amigavideo_staticdata *csd, struct amigabm_data *bm)
{
    csd->updatescroll = bm;
}

/* Convert Z flag to normal C-style return variable. Fun. */
UBYTE bltnode_wrapper(VOID)
{
    UBYTE ret;
    asm volatile (
       "    pea 1f\n"
       "    move.l 4(%%a1),-(%%sp)\n"
       "    rts\n"
       "1:  sne %d0\n"
       "    move.b %%d0,%0\n"
       : "=g" (ret)
    );
    return ret;
}

#define BEAMSYNC_ALARM 0x0f00
/* AOS must use some GfxBase flags field for these. Later.. */
#define bqvar GfxBase->pad3
#define BQ_NEXT 1
#define BQ_BEAMSYNC 2
#define BQ_BEAMSYNCWAITING 4
#define BQ_MISSED 8

static AROS_INTH1(gfx_blit, struct GfxBase *, GfxBase)
{
    AROS_INTFUNC_INIT

    volatile struct Custom *custom = (struct Custom*)0xdff000;
    struct bltnode *bn = NULL;
    UBYTE v;
    UWORD dmaconr;
    
    dmaconr = custom->dmaconr;
    dmaconr = custom->dmaconr;
    if (dmaconr & 0x4000) {
        /* Blitter still active? Wait for next interrupt. */
        return 0;
    }
    
    if (GfxBase->blthd == NULL && GfxBase->bsblthd == NULL) {
        custom->intena = INTF_BLIT;
        return 0;
    }
    
    /* Was last blit in this node? */
    if (bqvar & BQ_NEXT) {
        bqvar &= ~(BQ_NEXT | BQ_MISSED);
        if (bqvar & BQ_BEAMSYNC)
            bn = GfxBase->bsblthd;
        else
            bn = GfxBase->blthd;
        if (bn->stat == CLEANUP)
            AROS_UFC2(UBYTE, bn->cleanup,
                AROS_UFCA(struct Custom *, custom, A0),
                AROS_UFCA(struct bltnode*, bn, A1));
        /* Next node */
        bn = bn->n;
        if (bqvar & BQ_BEAMSYNC)
            GfxBase->bsblthd = bn;
        else
            GfxBase->blthd = bn;
    }

    if (GfxBase->bsblthd) {
        bn = GfxBase->bsblthd;
        bqvar |= BQ_BEAMSYNC;
    } else if (GfxBase->blthd) {
        bn = GfxBase->blthd;
        bqvar &= ~BQ_BEAMSYNC;
    }

    if (!bn) {
        /* Last blit finished */
        bqvar = 0;
        custom->intena = INTF_BLIT;
        GfxBase->blthd = GfxBase->bsblthd = NULL;
        DisownBlitter();
        return 0;
    }

    if (bqvar & BQ_BEAMSYNC) {
        UWORD vpos = VBeamPos();
        bqvar &= ~BQ_BEAMSYNCWAITING;
        if (!(bqvar & BQ_MISSED) && bn->beamsync > vpos) {
            volatile struct CIA *ciab = (struct CIA*)0xbfd000;
            UWORD w = BEAMSYNC_ALARM - (bn->beamsync - vpos);
            bqvar |= BQ_BEAMSYNCWAITING;
            ciab->ciacrb &= ~0x80;
            ciab->ciatodhi = 0;
            ciab->ciatodmid = w >> 8;
            ciab->ciatodlow = w;
            return 0;
        }
    }
 
    v = AROS_UFC2(UBYTE, bltnode_wrapper,
            AROS_UFCA(struct Custom *, custom, A0),
            AROS_UFCA(struct bltnode*, bn, A1));

    dmaconr = custom->dmaconr;
    dmaconr = custom->dmaconr;
    if (!(dmaconr & 0x4000)) {
        /* Eh? Blitter not active?, better fake the interrupt. */
        custom->intreq = INTF_SETCLR | INTF_BLIT;
    }
    
    if (v) {
        /* Handle same node again next time */
        return 0;
    }

    bqvar |= BQ_NEXT;

    return 0;
        
    AROS_INTFUNC_EXIT
}

static AROS_INTH1(gfx_beamsync, struct amigavideo_staticdata*, csd)
{
    AROS_INTFUNC_INIT

    struct GfxBase *GfxBase = (APTR)csd->cs_GfxBase;

    if (bqvar & BQ_BEAMSYNCWAITING) {
        /* We only need to trigger blitter interrupt */
        volatile struct Custom *custom = (struct Custom*)0xdff000;
        custom->intreq = INTF_SETCLR | INTF_BLIT;
    }

    return FALSE;

    AROS_INTFUNC_EXIT
}

/* update the copper1 data to reflect the requested bm */
static void updatecopper1frombm(struct amigavideo_staticdata *csd, struct amigabm_data *bm)
{
    D(bug("[AmigaVideo] %s(0x%p)\n", __func__, bm));

    csd->copper1[5] = csd->bplcon3 | bm->bplcon3 | ((bm->sprite_res + 1) << 6);
    csd->copper1[9] = csd->bplcon3 | bm->bplcon3 | (1 << 9) | ((bm->sprite_res + 1) << 6);
    csd->copper1[13] = csd->bplcon3 | bm->bplcon3 | ((bm->sprite_res + 1) << 6);
    csd->copper1[27] = csd->bplcon2 | ((csd->aga && !(bm->modeid & EXTRAHALFBRITE_KEY)) ? 0x0200 : 0);
    if (bm->copld.copper2_palette)
    {
        if (csd->aga_enabled)
        {
            csd->copper1[7] = bm->copld.copper2_palette[3];
            csd->copper1[11] = bm->copld.copper2_palette[3];
        }
        else
        {
            csd->copper1[7] = bm->copld.copper2_palette[1];
            csd->copper1[11] = bm->copld.copper2_palette[1];
        }
    }
    else
    {
        csd->copper1[7] = DEFAULT_BORDER_GRAY;
        csd->copper1[11] = DEFAULT_BORDER_GRAY;
    }
    if (bm->copld.copper2_fmode)
        csd->copper1[15] = *bm->copld.copper2_fmode;
    else
        csd->copper1[15] = 0;
}

/* attach a hidden screen to the visible copperlist chains */
static BOOL gfx_vblank_attachbm(struct amigavideo_staticdata *csd, struct amigabm_data *bm, struct amigabm_data *bmpred)
{
    struct HIDD_ViewPortData *vpdata = (struct HIDD_ViewPortData *)bm->node.ln_Name;
    struct ViewPort  *bmvp = vpdata->vpe->ViewPort;
    struct GfxBase *GfxBase = (APTR)csd->cs_GfxBase;
    struct amigabm_data *bmtmp;
    LONG screen_start, screen_finish = (STANDARD_DENISE_MAX << 1);

    D(bug("[AmigaVideo] %s(0x%p)\n", __func__, bm));

    if (!csd->interlaced && bm->interlace)
    {
        /*
        Signal the support task to force a rethink of the display
        */
        D(bug("[AmigaVideo] %s: display rebuild needed\n", __func__));
        if (csd->svcTask)
            Signal(csd->svcTask, SIGBREAKF_CTRL_F);
        return FALSE;
    }

    /* calculate and adjust the screens visible region */
    screen_start = bm->topedge;
    if (!bmpred || (((bmtmp = (struct amigabm_data *)bmpred->node.ln_Succ) == NULL) ||
        (bmtmp->node.ln_Succ == NULL)))
    {
        bmtmp = (struct amigabm_data *)GetHead(csd->compositedbms);
    }

    if (bmtmp)
    {
        screen_finish = bmtmp->topedge - (bmtmp->copld.extralines + 1);
        if ((bm->interlace) && (!bmtmp->interlace))
            screen_finish <<= 1;
        else if ((!bm->interlace) && (bmtmp->interlace))
            screen_finish >>= 1;
    }
    else
    {
        screen_finish = bm->height - 1;
    }
    bm->displayheight = limitheight(csd, (screen_finish - screen_start) + 1, bm->interlace, FALSE);
    D(bug("[AmigaVideo] %s: screen range = %d -> %d (%d rows)\n", __func__, screen_start, screen_finish, bm->displayheight);)
    /* sanity check .. */
    if (bm->displayheight <= 1)
    {
        /*
         * either not enough of the screen is visible,
         * or it is obscured by the next screens copperlist pre-amble
         */
        return FALSE;
    }

    if (bmpred)
        Insert(&csd->c2fragments, &bm->copld.cnode, &bmpred->copld.cnode);
    else
        AddHead(&csd->c2fragments, &bm->copld.cnode);

    if (bm->interlace)
    {
        if (bmpred)
            Insert(&csd->c2ifragments, &bm->copsd.cnode, &bmpred->copsd.cnode);
        else
            AddHead(&csd->c2ifragments, &bm->copsd.cnode);
    }
    else if (csd->interlaced)
    {
        initvpicopper(bmvp,bm,"[AmigaVideo] %s:  allocated dummy CopSStart @ 0x%p\n", __func__);

        if (bmpred)
            Insert(&csd->c2ifragments, &bm->copsd.cnode, &bmpred->copsd.cnode);
        else
            AddHead(&csd->c2ifragments, &bm->copsd.cnode);
    }

    D(
        if (!bmpred)
            bug("[AmigaVideo] %s: * new topmost bitmap\n", __func__);
      )

    setcopperscroll(csd, bm, ((csd->interlaced == TRUE) || (bm->interlace == TRUE)));

    if (bm->bmucl)
    {
        AmigaVideo_ParseCopperlist(csd, bm, bmvp->UCopIns->FirstCopList);
#if !USE_UCOP_DIRECT
        D(bug("[AmigaVideo] %s:  -- copying user-copperlist data ...\n", __func__);)
        CopyMemQuick(bm->bmucl->CopLStart, bm->copld.copper2_tail, bm->bmuclsize);
        bm->copld.copper2_tail = (APTR)((IPTR)bm->copld.copper2_tail + bm->bmuclsize);
        if ((bm->interlace) || (csd->interlaced))
        {
            if (bm->bmucl->CopSStart)
                CopyMemQuick(bm->bmucl->CopSStart, bm->copsd.copper2_tail, bm->bmuclsize);
            else
                CopyMemQuick(bm->bmucl->CopLStart, bm->copsd.copper2_tail, bm->bmuclsize);
            bm->copsd.copper2_tail = (APTR)((IPTR)bm->copsd.copper2_tail + bm->bmuclsize);
        }
        bm->bmucl->Flags |= (1<<15);
#endif
    }

    /* link the copperlist fragments */
    if (bm->copld.cnode.mln_Pred && bm->copld.cnode.mln_Pred->mln_Pred)
    {
        bmtmp = BMDATFROMCOPLD(bm->copld.cnode.mln_Pred);
        setcopperlisttail(csd, bmtmp->copld.copper2_tail, bm->bmcl->CopLStart, TRUE);                                           // PREV CopL >> THIS CopL
        D(bug("[AmigaVideo] %s:     CopL @ 0x%p jmp to CopL @ 0x%p\n", __func__, &bmtmp->copld, &bm->copld);)
    }
    else
    {
        /* update the copper1 data to reflect the topmost screen */
        updatecopper1frombm(csd, bm);
    }

    if (bm->copld.cnode.mln_Succ && bm->copld.cnode.mln_Succ->mln_Succ)
    {
        bmtmp = BMDATFROMCOPLD(bm->copld.cnode.mln_Succ);
        setcopperlisttail(csd, bm->copld.copper2_tail, bmtmp->bmcl->CopLStart, TRUE);                                           // THIS CopL >> NEXT CopL
        D(bug("[AmigaVideo] %s:     CopL @ 0x%p jmp to CopL @ 0x%p\n", __func__, &bm->copld, &bmtmp->copld);)
    }

    if ((bm->interlace) || (csd->interlaced))
    {
        if (bm->copsd.cnode.mln_Pred && bm->copsd.cnode.mln_Pred->mln_Pred)
        {
            bmtmp = BMDATFROMCOPSD(bm->copsd.cnode.mln_Pred);
            setcopperlisttail(csd, bmtmp->copsd.copper2_tail, bm->bmcl->CopSStart, TRUE);                                       // PREV CopS >> THIS CopS
            D(bug("[AmigaVideo] %s:     CopS @ 0x%p jmp to CopS @ 0x%p\n", __func__, &bmtmp->copsd, &bm->copsd);)
        }
        if (bm->copsd.cnode.mln_Succ && bm->copsd.cnode.mln_Succ->mln_Succ)
        {
            bmtmp = BMDATFROMCOPSD(bm->copsd.cnode.mln_Succ);
            setcopperlisttail(csd, bm->copsd.copper2_tail, bmtmp->bmcl->CopSStart, TRUE);                                       // THIS CopS >> NEXT CopS
            D(bug("[AmigaVideo] %s:     CopS @ 0x%p jmp to CopS @ 0x%p\n", __func__, &bm->copsd, &bmtmp->copsd);)
        }
        else
        {
            bmtmp = BMDATFROMCOPLD(GetHead(&csd->c2fragments));
            setcopperlisttail(csd, bm->copsd.copper2_tail, bmtmp->bmcl->CopLStart, FALSE);                                      // THIS CopS -> FIRST CopL
            D(bug("[AmigaVideo] %s:     CopS @ 0x%p point to CopL @ 0x%p\n", __func__, &bm->copsd, &bmtmp->copld);)
        }
    }
    else
    {
        if (!(bm->copld.cnode.mln_Succ && bm->copld.cnode.mln_Succ->mln_Succ))
        {
            if (!IsListEmpty(&csd->c2ifragments))
            {
                bmtmp = BMDATFROMCOPSD(GetHead(&csd->c2ifragments));
                D(bug("[AmigaVideo] %s:     CopL @ 0x%p point to CopS @ 0x%p\n", __func__, &bm->copld, &bmtmp->copsd);)
                setcopperlisttail(csd, bm->copld.copper2_tail, bmtmp->bmcl->CopSStart, FALSE);                              // THIS CopL -> FIRST CopS
            }
            else
            {
                bmtmp = BMDATFROMCOPLD(GetHead(&csd->c2fragments));
                D(bug("[AmigaVideo] %s:     CopL @ 0x%p point to CopL @ 0x%p\n", __func__, &bm->copld, &bmtmp->copld);)
                setcopperlisttail(csd, bm->copld.copper2_tail, bmtmp->bmcl->CopLStart, FALSE);                              // THIS CopL -> FIRST CopL
            }
        }
    }

    if (!bmpred)
    {
        GfxBase->LOFlist = bm->bmcl->CopLStart;
        if ((bm->interlace) || (csd->interlaced))
            GfxBase->SHFlist = bm->bmcl->CopSStart;
        else
            GfxBase->SHFlist = bm->bmcl->CopLStart;
    }
    return TRUE;
}

/* remove a visible screen from the copperlist chains */
static VOID gfx_vblank_detachbm(struct amigavideo_staticdata *csd, struct amigabm_data *bm)
{
    struct GfxBase *GfxBase = (APTR)csd->cs_GfxBase;
    struct copper2data *c2dp = NULL, *c2ds = NULL, *c2dip = NULL, *c2dis = NULL;
    struct amigabm_data *bmtmp;

    D(bug("[AmigaVideo] %s(0x%p)\n", __func__, bm));

    if (bm->copld.cnode.mln_Pred && bm->copld.cnode.mln_Pred->mln_Pred)
        c2dp = (struct copper2data *)bm->copld.cnode.mln_Pred;
    if (bm->copld.cnode.mln_Succ && bm->copld.cnode.mln_Succ->mln_Succ)
        c2ds = (struct copper2data *)bm->copld.cnode.mln_Succ;
    Remove(&bm->copld.cnode);
    D(bug("[AmigaVideo] %s:   removed bm->copld node\n", __func__);)

    if (csd->interlaced || bm->interlace)
    {
        if (bm->copsd.cnode.mln_Pred && bm->copsd.cnode.mln_Pred->mln_Pred)
            c2dip = (struct copper2data *)bm->copsd.cnode.mln_Pred;
        if (bm->copsd.cnode.mln_Succ && bm->copsd.cnode.mln_Succ->mln_Succ)
            c2dis = (struct copper2data *)bm->copsd.cnode.mln_Succ;
        Remove(&bm->copsd.cnode);
        D(bug("[AmigaVideo] %s:   removed bm->copsd node\n", __func__);)
    }

    if (c2dp && c2dp->cnode.mln_Pred)
    {
        if (c2ds)
        {
            bmtmp = BMDATFROMCOPLD(c2ds);
            setcopperlisttail(csd, c2dp->copper2_tail, bmtmp->bmcl->CopLStart, TRUE);
            D(bug("[AmigaVideo] %s: CopL @ 0x%p jmp to CopL @ 0x%p\n", __func__, c2dp, c2ds);)
        }
        else
        {
            D(bug("[AmigaVideo] %s: No screens follow us\n", __func__);)
            bmtmp = BMDATFROMCOPLD(GetHead(&csd->c2fragments));
            setcopperlisttail(csd, c2dp->copper2_tail, bmtmp->bmcl->CopLStart, FALSE);
            D(bug("[AmigaVideo] %s: CopL @ 0x%p point to CopL @ 0x%p\n", __func__, c2dp, &bmtmp->copld);)
        }

        if (csd->interlaced || bm->interlace)
        {
            if (c2dis)
            {
                bmtmp = BMDATFROMCOPSD(c2dis);
                setcopperlisttail(csd, c2dip->copper2_tail, bmtmp->bmcl->CopSStart, TRUE);
                D(bug("[AmigaVideo] %s: CopS @ 0x%p jmp to CopS @ 0x%p\n", __func__, c2dip, c2dis);)
            }
            else
            {
                bmtmp = BMDATFROMCOPLD(GetHead(&csd->c2fragments));
                setcopperlisttail(csd, c2dip->copper2_tail, bmtmp->bmcl->CopLStart, FALSE);
                D(bug("[AmigaVideo] %s: CopS @ 0x%p point to CopL @ 0x%p\n", __func__, c2dip, &bmtmp->copld);)
            }
        }
    }
    else
    {
        struct copper2data *c2t;
        UWORD *copstart;

        D(bug("[AmigaVideo] %s: screen-bitmap was first on copperlist chain\n", __func__);)
        if (c2ds)
        {
            bmtmp = BMDATFROMCOPLD(c2ds);

            /* first, replace any tail refrence ... */
            if (!IsListEmpty(&csd->c2ifragments))
            {
                c2t = (struct copper2data *)GetTail(&csd->c2ifragments);
                setcopperlisttail(csd, c2t->copper2_tail, bmtmp->bmcl->CopLStart, FALSE);
                D(bug("[AmigaVideo] %s: CopS @ 0x%p point to CopL @ 0x%p\n", __func__, c2t, &bmtmp->copsd);)
            }

            c2t = (struct copper2data *)GetTail(&csd->c2fragments);
            if (!IsListEmpty(&csd->c2ifragments))
            {
                setcopperlisttail(csd, c2t->copper2_tail, bmtmp->bmcl->CopSStart, FALSE);
                D(bug("[AmigaVideo] %s: CopL @ 0x%p point to CopS @ 0x%p\n", __func__, c2t, &bmtmp->copsd);)
            }
            else
            {
                setcopperlisttail(csd, c2t->copper2_tail, bmtmp->bmcl->CopLStart, FALSE);
                D(bug("[AmigaVideo] %s: CopL @ 0x%p point to CopL @ 0x%p\n", __func__, c2t, &bmtmp->copld);)
            }

            /* update the copper1 data to reflect the topmost screen */
            csd->copper1[5] = csd->bplcon3 | bmtmp->bplcon3 | ((bmtmp->sprite_res + 1) << 6);
            csd->copper1[9] = csd->bplcon3 | bmtmp->bplcon3 | (1 << 9) | ((bmtmp->sprite_res + 1) << 6);
            csd->copper1[13] = csd->bplcon3 | bmtmp->bplcon3 | ((bmtmp->sprite_res + 1) << 6);
            csd->copper1[27] = csd->bplcon2 | ((csd->aga && !(bmtmp->modeid & EXTRAHALFBRITE_KEY)) ? 0x0200 : 0);
            if (csd->aga && csd->aga_enabled)
            {
                csd->copper1[7] = bmtmp->copld.copper2_palette[3];
                csd->copper1[11] = bmtmp->copld.copper2_palette[3];
            }
            else
            {
                csd->copper1[7] = bmtmp->copld.copper2_palette[1];
                csd->copper1[11] = bmtmp->copld.copper2_palette[1];
            }

            /* now adjust the list(s) ... */
            GfxBase->LOFlist = bmtmp->bmcl->CopLStart;
            if (!IsListEmpty(&csd->c2ifragments))
            {
                bmtmp = BMDATFROMCOPSD(GetHead(&csd->c2ifragments));
                GfxBase->SHFlist = bmtmp->bmcl->CopSStart;
            }
            else
                GfxBase->SHFlist = bmtmp->bmcl->CopLStart;
        }
        else
        {
            D(bug("[AmigaVideo] %s: Nothing left to display!\n", __func__);)

            /* for our safety load the backup copperlist... */
            GfxBase->LOFlist = csd->copper2_backup;
            GfxBase->SHFlist = GfxBase->LOFlist;
        }
    }
}

static BOOL gfx_vblank_doupdatescroll(struct amigavideo_staticdata *csd)
{
    struct amigabm_data *bm, *bmsafe, *bmtmp;
    BOOL retval = FALSE;

    ForeachNodeSafe(csd->compositedbms, bm, bmsafe)
    {
        UWORD bmend;
        if (bm->node.ln_Succ && bm->node.ln_Succ->ln_Succ && (bm->node.ln_Succ == (struct Node *)csd->updatescroll))
        {
            UWORD olddisplayheight = bm->displayheight;

            /* adjust the screen situated above the moved screen-bitmap */
            bmend = csd->updatescroll->topedge - (csd->updatescroll->copld.extralines + 1);
            if ((bm->interlace) && (!csd->updatescroll->interlace))
                bmend <<= 1;
            else if ((!bm->interlace) && (csd->updatescroll->interlace))
                bmend >>= 1;

            bm->displayheight = limitheight(csd, (bmend - bm->topedge), bm->interlace, FALSE);

            /* only adjust if enough is visible - otherwise it will be obscured, and hidden in the next case... */
            if ((bm->displayheight != olddisplayheight) && (bm->displayheight > 1))
            {
                setcopperscroll(csd, bm, ((csd->interlaced == TRUE) || (bm->interlace == TRUE)));
                if (bm->bmucl)
                {

                    AmigaVideo_ParseCopperlist(csd, bm, bm->bmucl);
#if !USE_UCOP_DIRECT
                    D(bug("[AmigaVideo] %s:  -- copying user-copperlist data ...\n", __func__);)
                    CopyMemQuick(bm->bmucl->CopLStart, bm->copld.copper2_tail, bm->bmuclsize);
                    bm->copld.copper2_tail = (APTR)((IPTR)bm->copld.copper2_tail + bm->bmuclsize);
                    if ((bm->interlace) || (csd->interlaced))
                    {
                        if (bm->bmucl->CopSStart)
                            CopyMemQuick(bm->bmucl->CopSStart, bm->copsd.copper2_tail, bm->bmuclsize);
                        else
                            CopyMemQuick(bm->bmucl->CopLStart, bm->copsd.copper2_tail, bm->bmuclsize);
                        bm->copsd.copper2_tail = (APTR)((IPTR)bm->copsd.copper2_tail + bm->bmuclsize);
                    }
                    bm->bmucl->Flags |= (1<<15);
#endif
                }
            }
        }
        else if (bm == csd->updatescroll)
        {
            UWORD toptmp;
            retval = TRUE;

            if (bm->updtop < bm->topedge)
            {
                struct Node *tmpnode, *lastpred = &bm->node;
                UWORD adjust, topvis = bm->topedge - (bm->copld.extralines + 2);

                /* check if any of the obscured screens have become visible .. */
                ForeachNodeSafe(csd->obscuredbms, bmtmp, tmpnode)
                {
                    if (bmtmp->node.ln_Pri > bm->node.ln_Pri)
                    {
                        toptmp = bmtmp->topedge;
                        if ((bm->interlace) && (!bmtmp->interlace))
                        {
                            toptmp <<= 1;
                            adjust = (bmtmp->copld.extralines + 2) << 1;
                        }
                        else if ((!bm->interlace) && (bmtmp->interlace))
                        {
                            toptmp >>= 1;
                            adjust = (bmtmp->copld.extralines + 2) >> 1;
                        }

                        if (toptmp < topvis)
                        {
                            struct amigabm_data *bmpred = NULL;

                            if (lastpred->ln_Pred && lastpred->ln_Pred->ln_Pred)
                                bmpred = (struct amigabm_data *)lastpred->ln_Pred;

                            D(bug("[AmigaVideo] %s: attempt to make screen-bitmap with bmdata @ 0x%p visible\n", __func__, bmtmp, bmtmp->node.ln_Pri);)

                            if (gfx_vblank_attachbm(csd, bmtmp, bmpred))
                            {
                                D(bug("[AmigaVideo] %s: screen-bitmap became visible\n", __func__);)

                                topvis = toptmp - adjust;
                                Remove(&bmtmp->node);
                                Insert(csd->compositedbms, &bmtmp->node, lastpred);
                                lastpred = &bmtmp->node;
                            }
                        }
                    }
                }
            }
            else
            {
                /* check if we have obscured any screens .. */
                struct amigabm_data *bmtmp2;
                bmtmp = bm;
                while ((bmtmp = (struct amigabm_data *)GetPred(&bmtmp->node)))
                {
                    toptmp = bmtmp->topedge;
                    if ((bm->interlace) && (!bmtmp->interlace))
                        toptmp <<= 1;
                    else if ((!bm->interlace) && (bmtmp->interlace))
                        toptmp >>= 1;

                    if (toptmp >= bm->topedge - (bm->copld.extralines + 2))
                    {
                        bmtmp2 = (struct amigabm_data *)bmtmp->node.ln_Succ;

                        gfx_vblank_detachbm(csd, bmtmp);

                        D(bug("[AmigaVideo] %s: screen bitmap with bmdata @ 0x%p and depth %d is now obscured\n", __func__, bmtmp, bmtmp->node.ln_Pri);)

                        /* Remove the bitmap node and add it to the obscured list ... */
                        Remove(&bmtmp->node);
                        Enqueue(csd->obscuredbms, &bmtmp->node);
                        D(bug("[AmigaVideo] %s: * obscured bitmap has been moved from display lists\n", __func__);)

                        bmtmp = bmtmp2;
                    }
                }
            }

            /* adjust the moved screen-bitmap */
            if (bm->node.ln_Succ && bm->node.ln_Succ->ln_Succ)
            {
                bmtmp = (struct amigabm_data *)bm->node.ln_Succ;
                bmend = bmtmp->topedge - (bmtmp->copld.extralines + 1);
                if ((bm->interlace) && (!bmtmp->interlace))
                    bmend <<= 1;
                else if ((!bm->interlace) && (bmtmp->interlace))
                    bmend >>= 1;

                bm->displayheight = limitheight(csd, (bmend - bm->topedge), bm->interlace, FALSE);
            }
            else
                bm->displayheight = limitheight(csd, (bm->height - bm->topedge), bm->interlace, FALSE);
            if (bm->displayheight > 1)
            {
                setcopperscroll(csd, csd->updatescroll, ((csd->interlaced == TRUE) || (csd->updatescroll->interlace == TRUE)));
                if (bm->bmucl)
                {
                    AmigaVideo_ParseCopperlist(csd, bm, bm->bmucl);
#if !USE_UCOP_DIRECT
                    D(bug("[AmigaVideo] %s:  -- copying user-copperlist data ...\n", __func__);)
                    CopyMemQuick(bm->bmucl->CopLStart, bm->copld.copper2_tail, bm->bmuclsize);
                    bm->copld.copper2_tail = (APTR)((IPTR)bm->copld.copper2_tail + bm->bmuclsize);
                    if ((bm->interlace) || (csd->interlaced))
                    {
                        if (bm->bmucl->CopSStart)
                            CopyMemQuick(bm->bmucl->CopSStart, bm->copsd.copper2_tail, bm->bmuclsize);
                        else
                            CopyMemQuick(bm->bmucl->CopLStart, bm->copsd.copper2_tail, bm->bmuclsize);
                        bm->copsd.copper2_tail = (APTR)((IPTR)bm->copsd.copper2_tail + bm->bmuclsize);
                    }
                    bm->bmucl->Flags |= (1<<15);
#endif
                }
            }
            else
            {
                /* This screen has become obscured ... */
                gfx_vblank_detachbm(csd, bm);

                D(bug("[AmigaVideo] %s: active screen bitmap with bmdata @ 0x%p and depth %d is now obscured\n", __func__, bm, bm->node.ln_Pri);)

                /* Remove the bitmap node and add it to the obscured list ... */
                Remove(&bm->node);
                Enqueue(csd->obscuredbms, &bm->node);
                D(bug("[AmigaVideo] %s: * obscured bitmap for active screen has been moved from display lists\n", __func__);)
            }
            bm->updtop = bm->topedge;
            bm->updleft = bm->leftedge;
        }
    }
    D(bug("[AmigaVideo] %s: updatescroll processed\n", __func__);)
    return retval;
}
static AROS_INTH1(gfx_vblank, struct amigavideo_staticdata*, csd)
{
    AROS_INTFUNC_INIT

    struct GfxBase *GfxBase = (APTR)csd->cs_GfxBase;
    volatile struct Custom *custom = (struct Custom*)0xdff000;
    BOOL lof = (custom->vposr & 0x8000) != 0;

    /* handle updatescroll first, since it may change the LOFlist/SHFlist pointers */
    if (csd->updatescroll) {
        D(bug("[AmigaVideo] %s: handling updatescroll\n", __func__);)

        if ((IsListEmpty(csd->compositedbms)) || (!gfx_vblank_doupdatescroll(csd)))
        {
            /* obscurred dragging bitmap has become visible */
            D(bug("[AmigaVideo] %s: re-displaying 0x%p\n", __func__, csd->updatescroll);)
        }
        csd->updatescroll = NULL;
    }

    /* is any displayed screen interlaced? */
    if (GfxBase->LOFlist != GfxBase->SHFlist) {
        custom->cop2lc = (ULONG)(lof ? GfxBase->LOFlist : GfxBase->SHFlist);
    } else {
        custom->cop2lc = (ULONG)GfxBase->LOFlist;
        /* We may be in SHF mode after switching interlace off. Fix it here. */
        if (!lof)
            custom->vposw = custom->vposr | 0x8000;
    }

    csd->framecounter++;
    if (csd->sprite) {
        UWORD *p = csd->sprite;
        p[0] = csd->spritepos;
        p[1 << csd->fmode_spr] = csd->spritectl;
    }
    
    for(int i=1; i<8; i++)
    {
      if (csd->new_sprite[i]) {
          UWORD *p = csd->new_sprite[i];
          p[0] = csd->new_spritepos[i];
          p[1 << csd->fmode_spr] = csd->new_spritectl[i];
      }
    }

    if (bqvar & BQ_BEAMSYNC)
        bqvar |= BQ_MISSED;

    return FALSE;

    AROS_INTFUNC_EXIT
}

VOID initcustom(struct amigavideo_staticdata *csd)
{
    UBYTE i;
    UWORD *c;
    UWORD vposr, val;
    struct GfxBase *GfxBase;
    struct Library *OOPBase;
    volatile struct Custom *custom = (struct Custom*)0xdff000;
    volatile struct CIA *ciab = (struct CIA*)0xbfd000;

    D(bug("[AmigaVideo] %s()\n", __func__));

#if (1)
    /* TODO: This shouldnt be done in the gfx driver!
     * move to somewhere more appropriate */

    /* Reset audio registers to values that help emulation
     * if some program enables audio DMA without setting period
     * or length. Very high period emulation is very CPU intensive.
     */
    for (i = 0; i < 4; i++) {
        custom->aud[i].ac_vol = 0;
        custom->aud[i].ac_per = 100;
        custom->aud[i].ac_len = 1000;
    }
#endif

    /* csd->cs_OOPBase was already set up.
     * See amigavideo.conf's 'oopbase_field' config
     */
    OOPBase = csd->cs_OOPBase;
    csd->cs_HiddBitMapBase = OOP_GetMethodID(IID_Hidd_BitMap, 0);
    csd->cs_HiddGfxBase = OOP_GetMethodID(IID_Hidd_Gfx, 0);

    csd->cs_UtilityBase = TaggedOpenLibrary(TAGGEDOPEN_UTILITY);
    if (!csd->cs_UtilityBase)
        Alert(AT_DeadEnd | AN_Hidd | AG_OpenLib | AO_UtilityLib);
    csd->cs_GfxBase = TaggedOpenLibrary(TAGGEDOPEN_GRAPHICS);
    if (!csd->cs_GfxBase)
        Alert(AT_DeadEnd | AN_Hidd | AG_OpenLib | AO_GraphicsLib);
    GfxBase = ((struct GfxBase *)csd->cs_GfxBase);
    GfxBase->cia = OpenResource("ciab.resource");

    vposr = custom->vposr;
    csd->aga = (vposr & 0x0200) == 0x0200;
    csd->ecs_agnus = (vposr & 0x2000) == 0x2000;
    val = custom->deniseid;
    custom->deniseid = custom->dmaconr;
    if (val == custom->deniseid) {
        custom->deniseid = custom->dmaconr ^ 0x8000;
        if (val == custom->deniseid) {
            if ((val & (2 + 8)) == 8)
                csd->ecs_denise = TRUE;
        }
    }

    /* Reset now we have the bases */
    resetcustom(csd);
    resetsprite(csd);
    
    new_resetsprite(csd, 1);
    new_resetsprite(csd, 2);
    new_resetsprite(csd, 3);
    new_resetsprite(csd, 4);
    new_resetsprite(csd, 5);
    new_resetsprite(csd, 6);
    new_resetsprite(csd, 7);

    csd->inter.is_Code         = (APTR)gfx_vblank;
    csd->inter.is_Data         = csd;
    csd->inter.is_Node.ln_Name = "GFX VBlank server";
    csd->inter.is_Node.ln_Pri  = 25;
    csd->inter.is_Node.ln_Type = NT_INTERRUPT;
    AddIntServer(INTB_VERTB, &csd->inter);

    /* There are programs that take over the system and
     * assume SysBase->IntVects[BLITTER].iv_Data = GfxBase!
     */
    GfxBase->bltsrv.is_Code         = (APTR)gfx_blit;
    GfxBase->bltsrv.is_Data         = GfxBase;
    GfxBase->bltsrv.is_Node.ln_Name = "Blitter";
    GfxBase->bltsrv.is_Node.ln_Type = NT_INTERRUPT;
    SetIntVector(INTB_BLIT, &GfxBase->bltsrv);
    custom->intena = INTF_BLIT;

    // CIA-B TOD counts scanlines */
    GfxBase->timsrv.is_Code = (APTR)gfx_beamsync;
    GfxBase->timsrv.is_Data = csd;
    GfxBase->timsrv.is_Node.ln_Name = "Beamsync";
    GfxBase->timsrv.is_Node.ln_Type = NT_INTERRUPT;
    Disable();
    AddICRVector(GfxBase->cia, 2, &GfxBase->timsrv);
    AbleICR(GfxBase->cia, 1 << 2);
    ciab->ciacrb |= 0x80;
    ciab->ciatodhi = 0;
    /* TOD/ALARM CIA bug: http://eab.abime.net/showpost.php?p=277315&postcount=10 */
    ciab->ciatodmid = BEAMSYNC_ALARM >> 8;
    ciab->ciatodlow = BEAMSYNC_ALARM & 0xff;
    ciab->ciacrb &= ~0x80;
    ciab->ciatodhi = 0;
    ciab->ciatodmid = 0;
    ciab->ciatodlow = 0;
    AbleICR(GfxBase->cia, 0x80 | (1 << 2));
    Enable();

    GfxBase->NormalDisplayColumns = 640;
    GfxBase->NormalDisplayRows = (GfxBase->DisplayFlags & NTSC) ? REZ_Y_MIN : (REZ_Y_MIN + REZ_PAL_LINES);
    GfxBase->MaxDisplayColumn = 640;
    GfxBase->MaxDisplayRow = (GfxBase->DisplayFlags & NTSC) ? REZ_Y_MIN : (REZ_Y_MIN + REZ_PAL_LINES);

    csd->startx = STANDARD_VIEW_X;
    csd->starty = STANDARD_VIEW_Y;

    csd->max_colors = csd->aga ? 256 : 32;

    csd->sprite_null = AllocMem((2 << 3) + (46 * (sizeof(WORD) << 1)), MEMF_CLEAR | MEMF_CHIP);
    csd->copper1 = (APTR)((IPTR)csd->sprite_null + (2 << 3));
    c = csd->copper1;

    // vsync_hblank
    COPPEROUT(c, 0x00e0, 0)

    // diagstrt
    COPPEROUT(c, 0x0100, csd->bplcon0_null)
    COPPEROUT(c, 0x0106, csd->bplcon3 | (1 << 6))               // Push the default display bplcon3 (ecs sprites)
    COPPEROUT(c, 0x0180, DEFAULT_BORDER_GRAY)                   // Pen = 0
    // If AGA is enabled, set the high bits of the color palette entry.
    COPPEROUT(c, csd->aga_enabled? 0x0106 : 0x1fe, csd->bplcon3 | (1 << 9) | (1 << 6))
    COPPEROUT(c, csd->aga_enabled? 0x0180 : 0x1fe, DEFAULT_BORDER_GRAY) // Pen = 0
    // Push the default display bplcon3 again
    COPPEROUT(c, csd->aga_enabled? 0x0106 : 0x1fe, csd->bplcon3 | (1 << 6))

    // fm0
    COPPEROUT(c, csd->ecs_denise? 0x01fc: 0x1fe, 0)             // Push fmode 0

    // diwstart, all NOP on OCS Denise
    COPPEROUT(c, csd->ecs_denise? 0x008e : 0x1fe, (1 << 8) | 0x81) // Push (DIWSTRT) Display window start
    COPPEROUT(c, csd->ecs_denise? 0x0090 : 0x1fe, (2 << 8) | 0x81) // Push (DIWSTOP) Display window stop
    COPPEROUT(c, csd->ecs_denise? 0x01e4 : 0x1fe, 0)               // Push (DIWHIGH) Display window
    COPPEROUT(c, csd->ecs_denise? 0x0092 : 0x1fe, 0)               // Push (DDFSTRT) Display bitplane data fetch start
    COPPEROUT(c, csd->ecs_denise? 0x0094 : 0x1fe, 0)               // Push (DDFSTOP) Display bitplane data fetch stop

    // bplcon2
    COPPEROUT(c, csd->ecs_denise? 0x0104 : 0x1fe, csd->bplcon2)                          // Push the screens bplcon2

    /* initialize SPRxPOS (sprfix) */
    COPPEROUT(c, 0x0140, 0)
    COPPEROUT(c, 0x0148, 0)
    COPPEROUT(c, 0x0150, 0)
    COPPEROUT(c, 0x0158, 0)
    COPPEROUT(c, 0x0160, 0)
    COPPEROUT(c, 0x0168, 0)
    COPPEROUT(c, 0x0170, 0)
    COPPEROUT(c, 0x0178, 0)
    
    /* set the initial sprite to a blank cursor (sprstrtup) */
    for (i = 0; i < 8; i++) {
        COPPEROUT(c, 0x0120 + (i << 2), (UWORD)(((ULONG)csd->sprite_null) >> 16))
        if (i == 0)
            csd->copper1_spritept = &c[-1];
        COPPEROUT(c, 0x0122 + (i << 2), (UWORD)(((ULONG)csd->sprite_null) >> 0))
    }

    // wait14
    COPPEROUT(c, 0x0c03, 0xfffe)        // Wait for VPOS = 0c, HPOS = 2

    // norm_hblank
    COPPEROUT(c, 0x0c03, 0xfffe)        // Wait for VPOS = 0c, HPOS = 2

    // jump
    COPPEROUT(c, 0x008a, 0x0000)        // COPJMP2

    csd->copper2_backup = c;

    // wait_forever
    COPPEROUT(c, 0x0106, csd->bplcon3)  // Push the display bplcon3 again
    COPPEROUT(c, 0xffff, 0xfffe)

    custom->cop1lc = (ULONG)csd->copper1;
    custom->cop2lc = (ULONG)csd->copper2_backup;

    custom->dmacon = 0x8000 | 0x0080 | 0x0040 | 0x0020;
    
    GfxBase->copinit = (struct copinit*)csd->copper1;

    D(bug("[AmigaVideo] %s: Copperlist0 %p (%d bytes)\n", __func__, csd->copper1, 46 * (sizeof(WORD) << 1)));
}

// This function unconditionally enables AGA features. Only call if you are SetChipRev!
VOID AmigaVideoCl__Hidd_AmigaGfx__EnableAGA(OOP_Class *cl, OOP_Object *o, void *msg)
{
    struct amigabm_data *bm, *bmnext;
    struct amigavideo_staticdata *csd = CSD(cl);
    IPTR copperlist_end;
    csd->aga_enabled = TRUE;
    // Recalculate copper lists for all visible screens now that AGA is on.
    // This should be enough to cover the sane cases when SetChipRev is called.
    // Interlace is ignored. Don't go open interlace screens before activating AGA!
    ForeachNodeSafe(csd->compositedbms, bm, bmnext) {
        setmode(csd, bm);
        copperlist_end = (IPTR)populatebmcopperlist(csd, bm, &bm->copld, bm->bmcl->CopLStart, FALSE);
        bm->bmcl->Count = (copperlist_end - (IPTR)bm->bmcl->CopLStart) >> 2;
        setfmode(csd, bm);
        setcoppercolors(csd, bm, bm->palette,
                        (void *)bm == (void *)csd->compositedbms->lh_Head);
        setcopperscroll2(csd, bm, &bm->copld, bm->bmcl->CopLStart, FALSE);
    }
}
