/*
    Copyright (C) 1995-2020, The AROS Development Team. All rights reserved.

    Desc:
*/

#include <exec/libraries.h>
#include <exec/rawfmt.h>
#include <exec/types.h>
#include <exec/resident.h>
#include <exec/memory.h>
#include <graphics/displayinfo.h>
#include <aros/libcall.h>
#include <proto/alib.h>
#include <proto/graphics.h>
#include <proto/kernel.h>
#include <proto/oop.h>
#include <proto/utility.h>
#include <oop/oop.h>

#include <hidd/hidd.h>
#include <aros/symbolsets.h>

#include <string.h>

#include LC_LIBDEFS_FILE

#include "chipset.h"
#include "blitter.h"

#define SPECIALMODES 3
#define NATIVEMODES (3 * 4 * SPECIALMODES)

static const UWORD widthtable[] = {
    REZ_X_MIN,
    (REZ_X_MIN << 1),
    (REZ_X_MIN << 2),
    0
};
static const UWORD heighttable[] = {
    REZ_Y_MIN,
    (REZ_Y_MIN + REZ_PAL_LINES),
    (REZ_Y_MIN << 1),
    ((REZ_Y_MIN + REZ_PAL_LINES) << 1),
    0
};
static const ULONG specialmask_aga[] = {
    0,
    EXTRAHALFBRITE_KEY,
    HAM_KEY,
    0xffffffff
};

#define SPECIAL_MODE_MASK (EXTRAHALFBRITE_KEY | HAM_KEY)

VOID AmigaVideoCl__Hidd_Gfx__NominalDimensions(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_NominalDimensions *msg)
{
    struct amigavideo_staticdata *csd = CSD(cl);
    struct GfxBase *GfxBase = (struct GfxBase *)csd->cs_GfxBase;

    DB2(bug("[AmigaVideo:Hidd] %s()\n", __func__));

    if (msg->width)
        *(msg->width) = GfxBase->NormalDisplayColumns;
    if (msg->height)
        *(msg->height) = GfxBase->NormalDisplayRows;
    if (msg->depth)
        *(msg->depth) = 2;
}

ULONG AmigaVideoCl__Hidd_Gfx__ModeProperties(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_ModeProperties *msg)
{
    ULONG flags = 0;
    ULONG modeid = msg->modeID;

    flags = DIPF_IS_SPRITES | DIPF_IS_DRAGGABLE |
        DIPF_IS_SPRITES_ATT | DIPF_IS_SPRITES_CHNG_BASE | DIPF_IS_SPRITES_CHNG_PRI |
        DIPF_IS_DBUFFER | DIPF_IS_BEAMSYNC | DIPF_IS_GENLOCK;
    msg->props->NumHWSprites = 8;
    if ((modeid & MONITOR_ID_MASK) == PAL_MONITOR_ID)
        flags |= DIPF_IS_PAL;
    if (modeid & LORESLACE_KEY)
        flags |= DIPF_IS_LACE;
    if (modeid & HAM_KEY) {
        flags |= DIPF_IS_HAM;
        if (modeid & SUPER_KEY)
            flags |= DIPF_IS_AA;
    }
    if (modeid & EXTRAHALFBRITE_KEY) {
        flags |= DIPF_IS_EXTRAHALFBRITE;
        if (modeid & SUPER_KEY)
            flags |= DIPF_IS_AA;
    }
    if ((modeid & SUPER_KEY) == SUPER_KEY && !(flags & DIPF_IS_AA))
        flags |= DIPF_IS_ECS;
    if (!(modeid & SPECIAL_MODE_MASK))
        flags |= DIPF_IS_WB;
    msg->props->DisplayInfoFlags = flags;
    msg->props->CompositionFlags = COMPF_ABOVE;
    DB2(bug("[AmigaVideo:Hidd] %s: %08x = %08x\n", __func__, modeid, flags));
    return sizeof(struct HIDD_ModeProperties);
}

HIDDT_ModeID *AmigaVideoCl__Hidd_Gfx__QueryModeIDs(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_QueryModeIDs *msg)
{
    struct amigavideo_staticdata *csd = CSD(cl);
    struct NativeChipsetMode *node;
    struct TagItem *tag, *tstate;
    ULONG minwidth = 0, maxwidth = 0xFFFFFFFF;
    ULONG minheight = 0, maxheight = 0xFFFFFFFF;
    HIDDT_ModeID *modeids;
    struct Library *UtilityBase = csd->cs_UtilityBase;
    WORD cnt;

   if (csd->superforward)
        return (HIDDT_ModeID*)OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);

    for (tstate = msg->queryTags; (tag = NextTagItem(&tstate)); )
    {
        switch (tag->ti_Tag)
        {
            case tHidd_GfxMode_MinWidth:
                minwidth = (ULONG)tag->ti_Tag;
                break;

            case tHidd_GfxMode_MaxWidth:
                maxwidth = (ULONG)tag->ti_Tag;
                break;

            case tHidd_GfxMode_MinHeight:
                minheight = (ULONG)tag->ti_Tag;
                break;

            case tHidd_GfxMode_MaxHeight:
                maxheight = (ULONG)tag->ti_Tag;
                break;
                
            case tHidd_GfxMode_PixFmts:
                /* all chipset modes have same pixelformat */
                break;

        }
    }
    DB2(bug("[AmigaVideo:Hidd] %s(%dx%d->%dx%d)\n", __func__, minwidth, minheight, maxwidth, maxheight));
    cnt = 0;
    ForeachNode(&csd->nativemodelist, node) {
        if (node->width >= minwidth && node->width <= maxwidth && node->height >= minheight && node->height <= maxheight) {
            cnt++;
        }
    }
    modeids = AllocVec((cnt + 1) * sizeof(HIDDT_ModeID), MEMF_PUBLIC);
    if (!modeids)
        return NULL;
    cnt = 0;
    ForeachNode(&csd->nativemodelist, node) {
        if (node->width >= minwidth && node->width <= maxwidth && node->height >= minheight && node->height <= maxheight) {
            DB2(bug("[AmigaVideo:Hidd] %s: #%d = %08x\n", __func__, cnt, node->modeid));
            modeids[cnt++] = node->modeid;
        }
    }
    modeids[cnt] = vHidd_ModeID_Invalid;
    return modeids;
}

VOID AmigaVideoCl__Hidd_Gfx__ReleaseModeIDs(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_ReleaseModeIDs *msg)
{
    struct amigavideo_staticdata *csd = CSD(cl);
    if (csd->superforward)
        OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
    else
        FreeVec(msg->modeIDs);
}

HIDDT_ModeID AmigaVideoCl__Hidd_Gfx__NextModeID(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_NextModeID *msg)
{
    struct amigavideo_staticdata *csd = CSD(cl);
    struct NativeChipsetMode *node = NULL;
    HIDDT_ModeID mid = vHidd_ModeID_Invalid;

    DB2(bug("[AmigaVideo:Hidd] %s(%08x)\n", __func__, msg->modeID));

    if (msg->modeID != vHidd_ModeID_Invalid) {
        ForeachNode(&csd->nativemodelist, node) {
            if (node->modeid == msg->modeID) {
                node = (struct NativeChipsetMode*)node->node.ln_Succ;
                break;
            }
        }
    }
    if (!node)
        node = (struct NativeChipsetMode*)csd->nativemodelist.lh_Head;
    if (node->node.ln_Succ) {
        mid = node->modeid;
        *msg->syncPtr = node->sync;
        *msg->pixFmtPtr = node->pf;
    }
    DB2(bug("[AmigaVideo:Hidd] %s: %08x %p %p\n", __func__, mid, *msg->syncPtr, *msg->pixFmtPtr));
    return mid;
}

BOOL AmigaVideoCl__Hidd_Gfx__GetMode(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_GetMode *msg)
{
    struct amigavideo_staticdata *csd = CSD(cl);
    struct NativeChipsetMode *node;

    DB2(bug("[AmigaVideo:Hidd] %s(%08x)\n", __func__, msg->modeID));

    if (csd->superforward)
        return (BOOL)OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);

    ForeachNode(&csd->nativemodelist, node) {
        if (node->modeid == msg->modeID) {
            *msg->syncPtr = node->sync;
            *msg->pixFmtPtr = node->pf;
            DB2(bug("[AmigaVideo:Hidd] %s: %p %p %dx%dx%d %d\n", __func__, node->sync, node->pf, node->width, node->height, node->depth, node->special));
            return TRUE;
        }
    }
    DB2(bug("[AmigaVideo:Hidd] %s: FAIL\n", __func__));
    return FALSE;
}

#define ADDTAG(tag,data) { *tagptr++ = tag; *tagptr++ = data; }

static void makemodename(ULONG modeid, UBYTE *bufptr)
{
    char tmpl[] = "%s:%s%s%s%s%s%s";
    IPTR modeargs[7];
    BOOL special = FALSE;
    int count = 0;

    special = (modeid & SPECIAL_MODE_MASK) != 0;

    if ((modeid & MONITOR_ID_MASK) == PAL_MONITOR_ID)
        modeargs[0] = (IPTR)"PAL";
    else if ((modeid & MONITOR_ID_MASK) == NTSC_MONITOR_ID)
        modeargs[0] = (IPTR)"NTSC";

    if ((modeid & (HIRES_KEY | SUPER_KEY)) == LORES_KEY)
        modeargs[1] = special ? (IPTR)"LowRes" : (IPTR)"Low Res";
    else if ((modeid & (HIRES_KEY | SUPER_KEY)) == HIRES_KEY)
        modeargs[1] = special ? (IPTR)"HighRes" : (IPTR)"High Res";
    else
        modeargs[1] = special ? (IPTR)"SuperHighRes" : (IPTR)"Super-High Res";

    if (modeid & HAM_KEY)
        modeargs[2 + count++] = (IPTR)" HAM";
    if (modeid & EXTRAHALFBRITE_KEY)
        modeargs[2 + count++] = (IPTR)" EHB";
    if ((modeid & LORESDPF2_KEY) == LORESDPF_KEY)
        modeargs[2 + count++] = (IPTR)" DualPF";
    if ((modeid & LORESDPF2_KEY) == LORESDPF2_KEY)
        modeargs[2 + count++] = (IPTR)" DualPF2";
    if (modeid & LORESLACE_KEY)
        modeargs[2 + count++] = special ? (IPTR)" Interlace" : (IPTR)" Laced";

    tmpl[5 + (count << 1)] = '\0';
    RawDoFmt(tmpl, (RAWARG)modeargs, RAWFMTFUNC_STRING, bufptr);

    DB2(bug("[AmigaVideo:Hidd] %s: %08x '%s'\n", __func__, modeid, bufptr));
}

static struct NativeChipsetMode *addmodeid(struct amigavideo_staticdata *csd, ULONG modeid, WORD w, WORD h, WORD d, UBYTE special)
{
    struct NativeChipsetMode *m;

    m = AllocMem(sizeof(struct NativeChipsetMode), MEMF_CLEAR | MEMF_PUBLIC);
    DB2(bug("[AmigaVideo:Hidd] %s: %p %08x %dx%dx%d %d\n", __func__, m, modeid, w, h, d, special));
    m->width = w;
    m->height = h;
    m->depth = d;
    m->special = special;
    m->modeid = modeid;
    AddTail(&csd->nativemodelist, &m->node);
    return m;
}

OOP_Object *AmigaVideoCl__Root__New(OOP_Class *cl, OOP_Object *o, struct pRoot_New *msg)
{
    struct amigavideo_staticdata *csd = CSD(cl);
#if !defined(USE_ALIEN_DISPLAYMODES)
    struct GfxBase *GfxBase = (struct GfxBase *)csd->cs_GfxBase;
#endif
    struct Library *OOPBase = csd->cs_OOPBase;
    struct TagItem mytags[] =
    {
        { aHidd_Gfx_ModeTags,   (IPTR)NULL              },
        { aHidd_Name,           (IPTR)"amigavideo.hidd" },
        { aHidd_HardwareName,   0                       },
        { aHidd_ProducerName,   (IPTR)"Commodore"       },
        { TAG_MORE,             (IPTR)msg->attrList     }
    };
    struct pRoot_New mymsg;
    ULONG allocsize = 3000, allocsizebuf = 1000;
    WORD x, y, cnt, i, j;

    UBYTE *buf, *bufptr;
    ULONG *tags, *tagptr;
    ULONG *modetags[NATIVEMODES], modeids[NATIVEMODES];
    ULONG *pftags_aga[SPECIALMODES];
    ULONG *pftags_ecs_shres = NULL, *pftags_ecs_hires, *pftags_ecs_lores, *pftags_ecs_6;
    ULONG *mode_tags_aga;
    ULONG *mode_tags_ecs;

    if (csd->initialized)
        return NULL;

    /* construct the hardware name .. */
    {
        IPTR rdfargs[2];

        if (csd->aga)
            rdfargs[0] = (IPTR)"AGA";
        else if ((csd->ecs_agnus) || (csd->ecs_denise))
            rdfargs[0] = (IPTR)"ECS";
        else
            rdfargs[0] = (IPTR)"OCS";
        if (GfxBase->DisplayFlags & NTSC)
            rdfargs[1] = (IPTR)"NTSC";
        else
            rdfargs[1] = (IPTR)"PAL";

        mytags[2].ti_Data = (IPTR)AllocVec(strlen((char *)rdfargs[0]) + strlen((char *)rdfargs[1]) + 33, MEMF_PUBLIC);
        RawDoFmt("Amiga %s Chipset %s Display Hardware", (RAWARG)rdfargs, RAWFMTFUNC_STRING, mytags[2].ti_Data);
        D(bug("[AmigaVideo:Hidd] %s: Hardware = '%s'\n", __func__, mytags[2].ti_Data));
    }

    NewList(&csd->nativemodelist);
    tags = tagptr = AllocVec(allocsize, MEMF_PUBLIC | MEMF_REVERSE);
    buf = bufptr = AllocVec(allocsizebuf, MEMF_PUBLIC | MEMF_REVERSE);

    /* build the sync tags for the standard modes */
    cnt = 0;
    for (y = 0; heighttable[y]; y++) {
        WORD h = heighttable[y];
#if !defined(USE_ALIEN_DISPLAYMODES)
        if (GfxBase->DisplayFlags & NTSC)
        {
            if (h != REZ_Y_MIN && h != (REZ_Y_MIN << 1))
                continue;
        }
        else
        {
            if (h == REZ_Y_MIN || h == (REZ_Y_MIN << 1))
                continue;
        }
#endif
        for (x = 0; widthtable[x]; x++) {
            WORD w = widthtable[x];
            WORD d, res;
            ULONG modeid;
            
            modeid = 0;
            if (w == (REZ_X_MIN << 2)) {
                res = 2;
                modeid |= SUPER_KEY;
                if (!csd->aga && !csd->ecs_denise)
                    continue;
                d = csd->aga ? 8 : 2;
            }
            else if (w == (REZ_X_MIN << 1)) {
                res = 1;
                modeid |= HIRES_KEY;
                d = csd->aga ? 8 : 4;
            } else {
                res = 0;
                d = csd->aga ? 8 : 5;
            }
            if (h >= (REZ_Y_MIN << 1))
                modeid |= LORESLACE_KEY;
            if (h == REZ_Y_MIN || h == (REZ_Y_MIN << 1))
                modeid |= NTSC_MONITOR_ID;
            else
                modeid |= PAL_MONITOR_ID;

            for (i = 0; i < SPECIALMODES; i++) {
                ULONG mid = modeid, pclock, syncstrt, synctot;
                UWORD d2 = d;
                if (i == 1) {
                    if (!csd->aga && (modeid & SUPER_KEY))
                        continue;
                    mid |= EXTRAHALFBRITE_KEY;
                    d2 = 6;
                } else if (i == 2) {
                    if (!csd->aga && (modeid & SUPER_KEY))
                        continue;
                    mid |= HAM_KEY;
                    d2 = csd->aga ? 8 : 6;
                }
                
                addmodeid(csd, mid, w, h, d2, csd->aga ? i : (i == 0 ? res : i - 1 + 3));
                modetags[cnt] = tagptr;
                modeids[cnt++] = mid;

                ADDTAG(aHidd_Sync_HMin, (16 << res));

                ADDTAG(aHidd_Sync_HDisp, w);
                ADDTAG(aHidd_Sync_VDisp, h);
                ADDTAG(aHidd_Sync_HDispMax, w + (48 << res));
                ADDTAG(aHidd_Sync_Flags, (modeid & LORESLACE_KEY) ? vHidd_Sync_Interlaced : 0);

                pclock = 1000000000 / (280 >> (res + 1));
                D(bug("[AmigaVideo:Hidd] %s: pclock = %d\n", __func__, pclock));
                ADDTAG(aHidd_Sync_PixelClock, pclock);

                if (modeid & PAL_MONITOR_ID)
                {
                    ADDTAG(aHidd_Sync_VDispMax, (modeid & LORESLACE_KEY) ? ((STANDARD_PAL_ROWS - MIN_PAL_ROW) << 1) : (STANDARD_PAL_ROWS - MIN_PAL_ROW));
                }
                else
                {
                    ADDTAG(aHidd_Sync_VDispMax, (modeid & LORESLACE_KEY) ? ((STANDARD_NTSC_ROWS - MIN_NTSC_ROW) << 1) : (STANDARD_NTSC_ROWS - MIN_NTSC_ROW));
                }
                ADDTAG(aHidd_Sync_VSyncStart, STANDARD_VBSTRT);
                ADDTAG(aHidd_Sync_VSyncEnd, STANDARD_VBSTOP);
                ADDTAG(aHidd_Sync_VTotal, (modeid & PAL_MONITOR_ID) ? STANDARD_PAL_ROWS : STANDARD_NTSC_ROWS);

                ADDTAG(aHidd_Sync_HSyncStart, STANDARD_HBSTRT);
                ADDTAG(aHidd_Sync_HSyncEnd, STANDARD_HBSTOP);
                ADDTAG(aHidd_Sync_HTotal, pclock / (100000000 / STANDARD_COLORCLOCKS / 28));
                ADDTAG(TAG_DONE, 0);
            }
        }
    }

    if (csd->aga) {
        pftags_aga[0] = tagptr;
        ADDTAG(aHidd_PixFmt_RedShift,            8);
        ADDTAG(aHidd_PixFmt_GreenShift,         16);
        ADDTAG(aHidd_PixFmt_BlueShift,          24);
        ADDTAG(aHidd_PixFmt_AlphaShift,          0);
        ADDTAG(aHidd_PixFmt_RedMask,            0x00FF0000);
        ADDTAG(aHidd_PixFmt_GreenMask,          0x0000FF00);
        ADDTAG(aHidd_PixFmt_BlueMask,           0x000000FF);
        ADDTAG(aHidd_PixFmt_AlphaMask,          0x00000000);
        ADDTAG(aHidd_PixFmt_CLUTMask,           0x000000FF);
        ADDTAG(aHidd_PixFmt_CLUTShift,          0);
        ADDTAG(aHidd_PixFmt_ColorModel,         vHidd_ColorModel_Palette);
        ADDTAG(aHidd_PixFmt_Depth,              8);
        ADDTAG(aHidd_PixFmt_BytesPerPixel,      1);
        ADDTAG(aHidd_PixFmt_BitsPerPixel,       8);
        ADDTAG(aHidd_PixFmt_StdPixFmt,          vHidd_StdPixFmt_Plane);
        ADDTAG(aHidd_PixFmt_BitMapType,         vHidd_BitMapType_Planar);
        ADDTAG(TAG_DONE, 0);
        
        pftags_aga[1] = tagptr;
        CopyMemQuick(pftags_aga[0], pftags_aga[1], (17 << 3));
        tagptr[23] = 6;
        tagptr[27] = 6;
        tagptr = (APTR)((IPTR)tagptr + (17 << 3));

        pftags_aga[2] = NULL;

        mode_tags_aga = tagptr;

        ADDTAG(aHidd_Sync_VMin,         1);
        ADDTAG(aHidd_Sync_HMax,         16384);
        ADDTAG(aHidd_Sync_VMax,         16384);

        for (j = 0; specialmask_aga[j] != 0xffffffff; j++) {
            if (pftags_aga[j])
                ADDTAG(aHidd_Gfx_PixFmtTags,    (IPTR)pftags_aga[j]);
            for (i = 0; i < cnt; i++) {
                ULONG modeid = modeids[i];
                if ((j == 0 && !(modeid & SPECIAL_MODE_MASK)) || (j > 0 && ((modeid & SPECIAL_MODE_MASK) == specialmask_aga[j]))) {
                    makemodename(modeid, bufptr);
                    ADDTAG(aHidd_Sync_Description,      (IPTR)bufptr);
                    bufptr += strlen(bufptr) + 1;
                    ADDTAG(aHidd_Gfx_SyncTags, (IPTR)modetags[i]);
                }
            }
        }

        ADDTAG(TAG_DONE, 0);

        mytags[0].ti_Data = (IPTR)mode_tags_aga;
    } else {

        pftags_ecs_lores = tagptr;
        ADDTAG(aHidd_PixFmt_RedShift,           20);
        ADDTAG(aHidd_PixFmt_GreenShift,         24);
        ADDTAG(aHidd_PixFmt_BlueShift,          28);
        ADDTAG(aHidd_PixFmt_AlphaShift,          0);
        ADDTAG(aHidd_PixFmt_RedMask,            0x00000F00);
        ADDTAG(aHidd_PixFmt_GreenMask,          0x000000F0);
        ADDTAG(aHidd_PixFmt_BlueMask,           0x0000000F);
        ADDTAG(aHidd_PixFmt_AlphaMask,          0x00000000);
        ADDTAG(aHidd_PixFmt_CLUTMask,           0x0000001F);
        ADDTAG(aHidd_PixFmt_CLUTShift,          0);
        ADDTAG(aHidd_PixFmt_ColorModel,         vHidd_ColorModel_Palette);
        ADDTAG(aHidd_PixFmt_Depth,              5);
        ADDTAG(aHidd_PixFmt_BytesPerPixel,      1);
        ADDTAG(aHidd_PixFmt_BitsPerPixel,       5);
        ADDTAG(aHidd_PixFmt_StdPixFmt,          vHidd_StdPixFmt_Plane);
        ADDTAG(aHidd_PixFmt_BitMapType,         vHidd_BitMapType_Planar);
        ADDTAG(TAG_DONE, 0);

        pftags_ecs_hires = tagptr;
        CopyMemQuick(pftags_ecs_lores, pftags_ecs_hires, (17 << 3));
        tagptr[23] = 4;
        tagptr[27] = 4;
        tagptr = (APTR)((IPTR)tagptr + (17 << 3));

        pftags_ecs_shres = tagptr;
        CopyMemQuick(pftags_ecs_lores, pftags_ecs_shres, (17 << 3));
        tagptr[23] = 2;
        tagptr[27] = 2;
        tagptr = (APTR)((IPTR)tagptr + (17 << 3));

        pftags_ecs_6 = tagptr;
        CopyMemQuick(pftags_ecs_lores, pftags_ecs_6, (17 << 3));
        tagptr[23] = 6;
        tagptr[27] = 6;
        tagptr = (APTR)((IPTR)tagptr + (17 << 3));

        mode_tags_ecs = tagptr;

        ADDTAG(aHidd_Sync_VMin,         1);
        ADDTAG(aHidd_Sync_HMax,         csd->ecs_agnus ? 16384 : 1008);
        ADDTAG(aHidd_Sync_VMax,         csd->ecs_agnus ? 16384 : 1008);

        ADDTAG(aHidd_Gfx_PixFmtTags,    (IPTR)pftags_ecs_lores);
        for (i = 0; i < cnt; i++) {
                if ((modeids[i] & SPECIAL_MODE_MASK) || (modeids[i] & SUPER_KEY) != LORES_KEY)
                        continue;
                makemodename(modeids[i], bufptr);
                ADDTAG(aHidd_Sync_Description,  (IPTR)bufptr);
                bufptr += strlen(bufptr) + 1;
                ADDTAG(aHidd_Gfx_SyncTags, (IPTR)modetags[i]);
        }

        ADDTAG(aHidd_Gfx_PixFmtTags,    (IPTR)pftags_ecs_hires);
        for (i = 0; i < cnt; i++) {
                if ((modeids[i] & SPECIAL_MODE_MASK) || (modeids[i] & SUPER_KEY) != HIRES_KEY)
                        continue;
                makemodename(modeids[i], bufptr);
                ADDTAG(aHidd_Sync_Description,  (IPTR)bufptr);
                bufptr += strlen(bufptr) + 1;
                ADDTAG(aHidd_Gfx_SyncTags, (IPTR)modetags[i]);
        }

        ADDTAG(aHidd_Gfx_PixFmtTags,    (IPTR)pftags_ecs_shres);
        if (csd->ecs_denise) {
                for (i = 0; i < cnt; i++) {
                        if ((modeids[i] & SPECIAL_MODE_MASK) || (modeids[i] & SUPER_KEY) != SUPER_KEY)
                                continue;
                        makemodename(modeids[i], bufptr);
                        ADDTAG(aHidd_Sync_Description,  (IPTR)bufptr);
                        bufptr += strlen(bufptr) + 1;
                        ADDTAG(aHidd_Gfx_SyncTags, (IPTR)modetags[i]);
                }
        }

        ADDTAG(aHidd_Gfx_PixFmtTags,    (IPTR)pftags_ecs_6);
        for (i = 0; i < cnt; i++) {
                if (!(modeids[i] & SPECIAL_MODE_MASK))
                        continue;
                makemodename(modeids[i], bufptr);
                ADDTAG(aHidd_Sync_Description,  (IPTR)bufptr);
                bufptr += strlen(bufptr) + 1;
                ADDTAG(aHidd_Gfx_SyncTags, (IPTR)modetags[i]);
        }

        ADDTAG(TAG_DONE, 0);

        mytags[0].ti_Data = (IPTR)mode_tags_ecs;
    }

    EnterFunc(bug("[AmigaVideo:Hidd] %s()\n", __func__));

    D(
      bug("[AmigaVideo:Hidd] %s: tags @ 0x%p, alloc=%d used=%d\n", __func__, tags, allocsize, (ULONG)tagptr - (ULONG)tags);
      bug("[AmigaVideo:Hidd] %s: buf @ 0x%p, alloc=%d used=%d\n", __func__, buf, allocsizebuf, bufptr - buf);
     )

    mymsg.mID   = msg->mID;
    mymsg.attrList = mytags;
    msg = &mymsg;

    /* Register gfxmodes */
    o = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
    if (NULL != o)
    {
        struct amigagfx_data        *data = OOP_INST_DATA(cl, o);
        OOP_MethodID                HiddGfxBase = csd->cs_HiddGfxBase;
        OOP_MethodFunc              gfxgetmode;
        OOP_Class                   *gfxgetmode_Class;
        struct pHidd_Gfx_GetMode    ggmmsg =
        {
            mID : HiddGfxBase + moHidd_Gfx_GetMode,
        };

        /* use the fast path calling GetMode */
        gfxgetmode = OOP_GetMethod(o, ggmmsg.mID, &gfxgetmode_Class);
    
        struct NativeChipsetMode *node;
        HIDDT_ModeID *midp;
        UWORD pfcnt;
        OOP_Object *pixelformats[8] = { 0 };

        D(bug("[AmigaVideo:Hidd] %s(): Got object from super\n", __func__));
        NewList((struct List *)&data->bitmaps);
        csd->initialized = 1;

        /* this can't be the right way to match modes.. */
        csd->superforward = TRUE;
        midp = HIDD_Gfx_QueryModeIDs(o, NULL);
        for (pfcnt = 0, i = 0; midp[i] != vHidd_ModeID_Invalid; i++) {
            OOP_Object *sync, *pf;
            ggmmsg.modeID = midp[i];
            ggmmsg.syncPtr = &sync;
            ggmmsg.pixFmtPtr = &pf;
            if (!gfxgetmode(gfxgetmode_Class, o, (OOP_Msg)&ggmmsg.mID))
                continue;
            for (j = 0; j < pfcnt; j++) {
                if (pf == pixelformats[j])
                    break;
            }
            if (j < pfcnt)
                continue;
            pixelformats[pfcnt++] = pf;
        }
        while (pfcnt < 8) {
            if (csd->aga)
                pixelformats[pfcnt] = pixelformats[0];
            else
                pixelformats[pfcnt] = pixelformats[pfcnt - 1];
            pfcnt++;
        }
        for (i = 0; i < pfcnt; i++) {
            DB2(bug("[AmigaVideo:Hidd] %s: pf %d: %p\n", __func__, i, pixelformats[i]));
        }

        ForeachNode(&csd->nativemodelist, node) {
            if (!node->pf) {
                OOP_Object *sync = NULL;
                for (i = 0; midp[i] != vHidd_ModeID_Invalid; i++) {
                    struct NativeChipsetMode *node2;
                    IPTR dwidth, dheight;
                    OOP_Object *pf;
                    BOOL found = FALSE;

                    ggmmsg.modeID = midp[i];
                    ggmmsg.syncPtr = &sync;
                    ggmmsg.pixFmtPtr = &pf;
                    if (!gfxgetmode(gfxgetmode_Class, o, (OOP_Msg)&ggmmsg.mID))
                        continue;
                    OOP_GetAttr(sync, aHidd_Sync_HDisp, &dwidth);
                    OOP_GetAttr(sync, aHidd_Sync_VDisp, &dheight);
                    if (node->width != dwidth || node->height != dheight)
                        continue;
                    ForeachNode(&csd->nativemodelist, node2) {
                        if (node->width == dwidth && node->height == dheight) {
                            if (node2->sync == sync) {
                                found = TRUE;
                                break;
                            }
                        }
                    }
                    if (!found)
                        break;
                }
                if (midp[i] == vHidd_ModeID_Invalid) {
                    sync = NULL;
                    D(bug("[AmigaVideo:Hidd] %s: sync not found!?\n", __func__));
                }
                node->sync = sync;
                node->pf = pixelformats[node->special];
                DB2(bug("[AmigaVideo:Hidd] %s: %08x %dx%dx%d sync = %p pf = %p\n",
                    __func__, node->modeid, node->width, node->height, node->depth, node->sync, node->pf));
            }
        }
        HIDD_Gfx_ReleaseModeIDs(o, midp);
        csd->superforward = FALSE;
        DB2(
          ForeachNode(&csd->nativemodelist, node) {
            bug("[AmigaVideo:Hidd] %s: %08x %dx%dx%d sync = %p pf = %p\n", __func__, node->modeid, node->width, node->height, node->depth, node->sync, node->pf);
          }
         )
        /* Create the display compositor object */
        {
            struct TagItem comptags [] =
            {
                { aHidd_Compositor_GfxHidd, (IPTR)o },
                { TAG_DONE, TAG_DONE }
            };
            data->compositor = OOP_NewObject(csd->amigacompositorclass, NULL, comptags);
            D(bug("[AmigaVideo:Hidd] %s(): display Compositor @ 0x%p\n", __func__, data->compositor));
            if (data->compositor)
            {
#if USE_FAST_BMSTACKCHANGE
                data->bmstackchange = OOP_GetMethod(data->compositor, csd->mid_BitMapStackChanged, &data->bmstackchange_Class);
#endif
            }
        }
    }
    FreeVec(buf);
    FreeVec(tags);
    ReturnPtr("[AmigaVideo:Hidd] AGFX::New", OOP_Object *, o);
}

/********** GfxHidd::Dispose()  ******************************/
VOID AmigaVideoCl__Root__Dispose(OOP_Class *cl, OOP_Object *o, OOP_Msg msg)
{
    EnterFunc(bug("[AmigaVideo:Hidd] %s(o=%p)\n", __func__, o));
    
    D(bug("[AmigaVideo:Hidd] %s: calling super\n", __func__));
    OOP_DoSuperMethod(cl, o, msg);
    
    ReturnVoid("[AmigaVideo:Hidd] AGFX::Dispose");
}


OOP_Object *AmigaVideoCl__Hidd_Gfx__CreateObject(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_CreateObject *msg)
{
    struct amigagfx_data *data = OOP_INST_DATA(cl, o);
    OOP_Object      *object = NULL;

    EnterFunc(bug("[AmigaVideo:Hidd] %s()\n", __func__));

    if (msg->cl == CSD(cl)->cs_basebm)
    {
        struct amigavideo_staticdata *csd = CSD(cl);
        struct Library *UtilityBase = csd->cs_UtilityBase;
        HIDDT_ModeID            modeid;
        struct pHidd_Gfx_CreateObject   p;
        struct TagItem tags[] =
        {
            { TAG_IGNORE, TAG_IGNORE }, /* Placeholder for aHidd_BitMap_ClassPtr */
            { aHidd_BitMap_AmigaVideo_Compositor, (IPTR)data->compositor },
            { TAG_MORE, (IPTR)msg->attrList }
        };

        modeid = (HIDDT_ModeID)GetTagData(aHidd_BitMap_ModeID, vHidd_ModeID_Invalid, msg->attrList);
        D(bug("[AmigaVideo:Hidd] %s: compositor @ 0x%p\n", __func__, tags[1].ti_Data));
        if (modeid != vHidd_ModeID_Invalid) {
            tags[0].ti_Tag = aHidd_BitMap_ClassPtr;
            tags[0].ti_Data = (IPTR)CSD(cl)->amigabmclass;
            D(bug("[AmigaVideo:Hidd] %s: modeid = %08x\n", __func__, modeid));
        }
        p.mID = msg->mID;
        p.cl = msg->cl;
        p.attrList = tags;

        object = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg)&p);
    }
    else
        object = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);

    ReturnPtr("[AmigaVideo:Hidd] AGFX::CreateObject", OOP_Object *, object);
}

VOID AmigaVideoCl__Root__Get(OOP_Class *cl, OOP_Object *o, struct pRoot_Get *msg)
{
    struct amigavideo_staticdata *csd = CSD(cl);
    ULONG idx;
    
    //bug("AmigaVideoCl__Root__Get %x\n", msg->attrID);

    if (IS_GFX_ATTR(msg->attrID, idx))
    {
        //bug("=%x\n", idx);
        switch (idx)
        {
            case aoHidd_Gfx_HWSpriteTypes:
                *msg->storage = vHidd_SpriteType_3Plus1;
            return;
            case aoHidd_Gfx_SupportsDisplayChange:
            case aoHidd_Gfx_SupportsHWCursor:
            case aoHidd_Gfx_NoFrameBuffer:
                *msg->storage = TRUE;
            return;
            case aoHidd_Gfx_IsWindowed:
                *msg->storage = FALSE;
            return;
            case aoHidd_Gfx_DriverName:
                *msg->storage = (IPTR)"AmigaVideo";
            return;
        }
    }
    OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
}

VOID AmigaVideoCl__Root__Set(OOP_Class *cl, OOP_Object *obj, struct pRoot_Set *msg)
{
    struct amigavideo_staticdata *csd = CSD(cl);
    struct Library *UtilityBase = csd->cs_UtilityBase;
    struct TagItem *tag, *tstate;

    tstate = msg->attrList;
    while((tag = NextTagItem(&tstate)))
    {
        ULONG idx;
        D(bug("[AmigaVideo:Hidd] %s(%x:%p)\n", __func__, tag->ti_Tag, tag->ti_Data));
        if (IS_GFX_ATTR(tag->ti_Tag, idx)) {
            D(bug("[AmigaVideo:Hidd] %s: ->%d\n", __func__, idx));
            switch(idx)
            {
            case aoHidd_Gfx_DisplayChangeCallBack:
                csd->ccb = (void *)tag->ti_Data;
                break;

            case aoHidd_Gfx_DisplayChangeCallBackData:
                csd->ccbdata = (APTR)tag->ti_Data;
                break;

            case aoHidd_Gfx_ActiveCallBack:
                csd->acb = (void *)tag->ti_Data;
                break;

            case aoHidd_Gfx_ActiveCallBackData:
                csd->acbdata = (APTR)tag->ti_Data;
                break;
            }
        }
    }
    OOP_DoSuperMethod(cl, obj, (OOP_Msg)msg);
}

VOID AmigaVideoCl__Hidd_Gfx__CopyBox(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_CopyBox *msg)
{
    struct amigavideo_staticdata *csd = CSD(cl);
    struct Library *OOPBase = csd->cs_OOPBase;
    HIDDT_DrawMode mode = GC_DRMD(msg->gc);
    IPTR src, dst;
    BOOL ok = FALSE;
 
    OOP_GetAttr(msg->src,  aHidd_BitMap_AmigaVideo_Drawable, &src);
    OOP_GetAttr(msg->dest, aHidd_BitMap_AmigaVideo_Drawable, &dst);
    if (src && dst) {
        struct amigabm_data *sdata = OOP_INST_DATA(OOP_OCLASS(msg->src), msg->src);
        struct amigabm_data *ddata = OOP_INST_DATA(OOP_OCLASS(msg->dest), msg->dest);
        ok = blit_copybox(csd, sdata->pbm, ddata->pbm, msg->srcX, msg->srcY, msg->width, msg->height, msg->destX, msg->destY, mode);
    }
    if (!ok)
        OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
}

BOOL AmigaVideoCl__Hidd_Gfx__CopyBoxMasked(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_CopyBoxMasked *msg)
{
    struct amigavideo_staticdata *csd = CSD(cl);
    struct Library *OOPBase = csd->cs_OOPBase;
    HIDDT_DrawMode mode = GC_DRMD(msg->gc);
    IPTR src, dst;
    BOOL ok = FALSE;
 
    OOP_GetAttr(msg->src,  aHidd_BitMap_AmigaVideo_Drawable, &src);
    OOP_GetAttr(msg->dest, aHidd_BitMap_AmigaVideo_Drawable, &dst);
    if (src && dst) {
        struct amigabm_data *sdata = OOP_INST_DATA(OOP_OCLASS(msg->src), msg->src);
        struct amigabm_data *ddata = OOP_INST_DATA(OOP_OCLASS(msg->dest), msg->dest);
        ok = blit_copybox_mask(csd, sdata->pbm, ddata->pbm, msg->srcX, msg->srcY, msg->width, msg->height, msg->destX, msg->destY, mode, msg->mask);
    }
    if (!ok)
        return OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);

    return TRUE;
}

BOOL AmigaVideoCl__Hidd_Gfx__SetCursorShape(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_SetCursorShape *msg)
{
    struct amigavideo_staticdata *csd = CSD(cl);
    struct Library *OOPBase = csd->cs_OOPBase;
    IPTR width, height;
    UWORD maxw, maxh;

    OOP_GetAttr(msg->shape, aHidd_BitMap_Width, &width);
    OOP_GetAttr(msg->shape, aHidd_BitMap_Height, &height);

    maxw = (csd->aga_enabled ? 64 : 16);
    maxh = maxw * 2;

    if (width > maxw || height > maxh)
        return FALSE;

    if (!setsprite(cl, o, width, height, msg))
        return FALSE;

    return TRUE;
}

BOOL AmigaVideoCl__Hidd_AmigaGfx__SetSpriteShape(OOP_Class *cl, OOP_Object *o, struct pHidd_AmigaGfx_SetSpriteShape *msg, int spritenum)
{
    struct amigavideo_staticdata *csd = CSD(cl);
    struct Library *OOPBase = csd->cs_OOPBase;
    IPTR width, height;
    UWORD maxw, maxh;

    OOP_GetAttr(msg->shape, aHidd_BitMap_Width, &width);
    OOP_GetAttr(msg->shape, aHidd_BitMap_Height, &height);

    maxw = (csd->aga ? 64 : 16);
    maxh = maxw * 2;

    if (width > maxw || height > maxh)
        return FALSE;

    if (!new_setsprite(cl, o, width, height, msg, spritenum))
        return FALSE;

    return TRUE;
}

BOOL AmigaVideoCl__Hidd_Gfx__GetMaxSpriteSize(OOP_Class *cl, ULONG Type, ULONG *Width, ULONG *Height)
{
    struct amigavideo_staticdata *csd = CSD(cl);
    *Width = csd->aga ? 64 : 16;
    *Height = *Width * 2;
    return TRUE;
}

BOOL AmigaVideoCl__Hidd_Gfx__SetCursorPos(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_SetCursorPos *msg)
{
    struct amigavideo_staticdata *csd = CSD(cl);
    UBYTE res = 0, cursy;
    BOOL interlace = FALSE;

    struct amigabm_data *bm;
    ForeachNode(csd->compositedbms, bm)
    {
        cursy = msg->y;
        if (csd->interlaced)
            cursy >>= 1;
        if (cursy < ((bm->topedge + bm->displayheight) >> bm->interlace))
        {
            res = bm->res;
            interlace = (bm->interlace != 0);
            break;
        }
    }

    setspritepos(csd, msg->x, msg->y, res, interlace);
    return TRUE;
}

BOOL AmigaVideoCl__Hidd_AmigaGfx__SetSpritePos(OOP_Class *cl, OOP_Object *o, struct pHidd_AmigaGfx_SetSpritePos *msg, int spritenum)
{
    struct amigavideo_staticdata *csd = CSD(cl);
    UBYTE res = 0, cursy;
    BOOL interlace = FALSE;

    struct amigabm_data *bm;
    ForeachNode(csd->compositedbms, bm)
    {
        cursy = msg->y;
        if (csd->interlaced)
            cursy >>= 1;
        if (cursy < ((bm->topedge + bm->displayheight) >> bm->interlace))
        {
            res = bm->res;
            interlace = (bm->interlace != 0);
            break;
        }
    }

    new_setspritepos(csd, msg->x, msg->y, res, interlace, spritenum);
    return TRUE;
}

VOID AmigaVideoCl__Hidd_Gfx__SetCursorVisible(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_SetCursorVisible *msg)
{
    struct amigavideo_staticdata *csd = CSD(cl);
    setspritevisible(csd, msg->visible);
}

VOID AmigaVideoCl__Hidd_AmigaGfx__SetSpriteVisible(OOP_Class *cl, OOP_Object *o, struct pHidd_AmigaGfx_SetSpriteVisible *msg, int spritenum)
{
    struct amigavideo_staticdata *csd = CSD(cl);
    new_setspritevisible(csd, msg->visible, spritenum);
}

ULONG AmigaVideoCl__Hidd_Gfx__MakeViewPort(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_MakeViewPort *msg)
{
    ULONG retval = MVP_OK;
    struct HIDD_ViewPortData *vpd = msg->Data;

    D(bug("[AmigaVideo:Hidd] %s(vp=%p, bm=%p, vpe=%p)\n", __func__, vpd->vpe->ViewPort, vpd->Bitmap, vpd->vpe));

    D(bug("[AmigaVideo:Hidd] %s: initial ViewPort->DspIns=0x%p\n", __func__, vpd->vpe->ViewPort->DspIns));

    /* Allocate copperlist storage */
    if ((vpd->vpe->ViewPort->DspIns = AllocMem(sizeof(struct CopList), MEMF_PUBLIC | MEMF_CLEAR)) == NULL)
        retval = MVP_NO_MEM;
    else
    {
        struct amigavideo_staticdata *csd = CSD(cl);
        struct amigabm_data *bmdata = OOP_INST_DATA(OOP_OCLASS(vpd->Bitmap), vpd->Bitmap);

        bmdata->bmcl = vpd->vpe->ViewPort->DspIns;
        D(bug("[AmigaVideo:Hidd] %s: allocated DspIns @ 0x%p\n", __func__, bmdata->bmcl));
        bmdata->bmcl->_ViewPort = vpd->vpe->ViewPort;
        bmdata->bmcl->MaxCount = get_copper_list_length(csd, bmdata->depth) >> 2;
        D(bug("[AmigaVideo:Hidd] %s: DspIns->MaxCount = %d\n", __func__, bmdata->bmcl->MaxCount));

        bmdata->bplcon3 &= ~((1 << 5)|(1 << 1));
        if (vpd->vpe->ViewPort->ColorMap)
        {
            struct ColorMap *vpcm = vpd->vpe->ViewPort->ColorMap;
            if (vpcm->Flags & BORDER_BLANKING)
                bmdata->bplcon3 |= (1 << 5);
            if (vpcm->Flags & BORDERSPRITES)
                bmdata->bplcon3 |= (1 << 1);
            switch (vpcm->SpriteResolution)
            {
                case SPRITERESN_35NS:
                    if (csd->aga && csd->aga_enabled)
                    {
                        bmdata->sprite_res = 2;
                        break;
                    }
                case SPRITERESN_70NS:
                    bmdata->sprite_res = 1;
                    break;

                case SPRITERESN_140NS:
                case SPRITERESN_ECS:
                case SPRITERESN_DEFAULT:
                    bmdata->sprite_res = 0;
                    break;
            }
        }
    }

    return retval;
}

void AmigaVideoCl__Hidd_Gfx__CleanViewPort(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_CleanViewPort *msg)
{
    struct amigavideo_staticdata *csd = CSD(cl);
    struct Library *GfxBase = csd->cs_GfxBase;
    struct HIDD_ViewPortData *vpd = msg->Data;
    struct ViewPort *vp = vpd->vpe->ViewPort;

    D(bug("[AmigaVideo:Hidd] %s(vp=%p, bm=%p, vpe=%p)\n", __func__, vpd->vpe->ViewPort, vpd->Bitmap, vpd->vpe));
    /* It's safe to call these functions on NULL pointers */
    FreeCopList(vp->ClrIns);
    FreeCopList(vp->DspIns);
    FreeCopList(vp->SprIns);

    if (vp->UCopIns)
    {
        struct CopList  *usercopl = vp->UCopIns->FirstCopList;
        while (usercopl)
        {
#if !USE_UCOP_DIRECT
            FreeVec(usercopl->CopLStart);
            FreeVec(usercopl->CopSStart);
#endif
            usercopl->CopLStart;
            usercopl->CopSStart;
            usercopl = usercopl->Next;
        }
        FreeCopList(vp->UCopIns->FirstCopList);
        FreeMem(vp->UCopIns, sizeof(struct UCopList));
    }

    /* Everything has been freed */
    vp->ClrIns  = NULL;
    vp->DspIns  = NULL;
    vp->SprIns  = NULL;
    vp->UCopIns = NULL;
}

void AmigaVideo_ParseCopperlist(struct amigavideo_staticdata *csd, struct amigabm_data *bmdata, struct CopList  *copFirst)
{
    struct CopList  *usercopl = copFirst;

    D(bug("[AmigaVideo:Hidd] %s: FirstCopList = 0x%p\n", __func__, usercopl);)

    if (usercopl->Count > 0)
    {
        struct CopIns *copIns = usercopl->CopIns;
        UWORD *copl, *cops = NULL;
        WORD count = 0;
        BOOL ucend=FALSE, passed = FALSE;

        usercopl->Flags &= ~(1<<15);
        while (usercopl)
        {
            count += usercopl->Count;
            usercopl = usercopl->Next;
        }
        D(bug("[AmigaVideo:Hidd] %s: %d instructions total\n", __func__, count);)
        bmdata->bmuclsize = (count << 2);
        usercopl = copFirst;

#if !USE_UCOP_DIRECT
        if (!usercopl->CopLStart)
        {
            /* user copperlist is allocated in any memory, since it is copied into the actual copperlist.. */
            usercopl->CopLStart = AllocVec(((count + 2) << 2), MEMF_CLEAR | MEMF_PUBLIC);
        }
#endif
        copl = usercopl->CopLStart;
        D(bug("[AmigaVideo:Hidd] %s:   CopList->CopLStart = 0x%p\n", __func__, copl);)
        if (bmdata->interlace != 0)
        {
#if !USE_UCOP_DIRECT
            if (!usercopl->CopSStart)
            {
                usercopl->CopSStart = AllocVec(((count + 2) << 2), MEMF_CLEAR | MEMF_PUBLIC);
            }
#endif
            cops = usercopl->CopSStart;
            D(bug("[AmigaVideo:Hidd] %s:   CopList->CopList = 0x%p\n", __func__, cops);)
        }

        while ((!ucend) && (count-- > 0))
        {
            switch (copIns->OpCode)
            {
                case CPRNXTBUF:
                    D(bug("[AmigaVideo:Hidd] %s:     CPRNXTBUF\n", __func__);)
                    copIns = copIns->u3.nxtlist->CopIns;
                    continue;
                case COPPER_MOVE:
                    D(bug("[AmigaVideo:Hidd] %s:     CMOVE *(%04x) = %04x\n", __func__, copIns->u3.u4.u1.DestAddr, copIns->u3.u4.u2.DestData);)
                    COPPEROUT(copl, copIns->u3.u4.u1.DestAddr, copIns->u3.u4.u2.DestData)
                    if (cops)
                    {
                        COPPEROUT(cops, copIns->u3.u4.u1.DestAddr, copIns->u3.u4.u2.DestData)
                    }
                    break;
                case COPPER_WAIT:
                    {
                        D(bug("[AmigaVideo:Hidd] %s:     CWAIT v0x%02x,h0x%02x\n", __func__, copIns->u3.u4.u1.VWaitPos, copIns->u3.u4.u2.HWaitPos);)
                        UWORD   movey = csd->starty + copIns->u3.u4.u1.VWaitPos + (bmdata->topedge >> bmdata->interlace),
                                movex = csd->startx + copIns->u3.u4.u2.HWaitPos;

                        /* If its the end of the user copperlist, or the displayheight of the bitmap,  bail out.. */
                        if (!((copIns->u3.u4.u1.VWaitPos == 1000) && (copIns->u3.u4.u2.HWaitPos == 0xFF)) &&
                            (copIns->u3.u4.u1.VWaitPos < (bmdata->displayheight >> bmdata->interlace)))
                        {
                            if (!passed && (movey > 256))
                            {
                                COPPEROUT(copl, 0xFFDF, 0xFFFE)
                            }
                            COPPEROUT(copl, (movey << 8) | (movex << 1) | 0x1, 0xFFFE)
                            if (cops)
                            {
                                if (!passed && (movey > 256))
                                {
                                    COPPEROUT(cops, 0xFFDF, 0xFFFE)
                                }
                                COPPEROUT(cops, (movey << 8) | (movex << 1) | 0x1, 0xFFFE)
                            }
                            if (!passed && (movey > 256))
                            {
                                passed = TRUE;
                            }
                            break;
                        }
                    }
                default:
                    ucend = TRUE;
                    break;
            }
            copIns++;
        }
        if (count > 0)
        {
            /* adjust to reflect the actual used size .. */
            D(bug("[AmigaVideo:Hidd] %s: adjusting for %d unused instructions\n", __func__, count);)
            bmdata->bmuclsize -= (count << 2);
#if defined(USE_COPPER_NOP_FILL)
            while (count-- > 0)
            {
                COPPEROUT(copl, 0x01fe, 0xfffe)
                if (cops)
                {
                    COPPEROUT(cops, 0x01fe, 0xfffe)
                }
            }
#endif
        }
#if USE_UCOP_DIRECT
        bmdata->copld.copper2_tail = (APTR)((IPTR)usercopl->CopLStart + bmdata->bmuclsize);
        if (cops)
            bmdata->copsd.copper2_tail = (APTR)((IPTR)usercopl->CopSStart + bmdata->bmuclsize);
#endif
        bmdata->bmucl = usercopl;
    }
}

ULONG AmigaVideoCl__Hidd_Gfx__PrepareViewPorts(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_PrepareViewPorts *msg)
{
    struct amigavideo_staticdata *csd = CSD(cl);
    struct HIDD_ViewPortData *vpd;
    struct amigabm_data *bmdata;
    ULONG retval = MVP_OK;

    D(bug("[AmigaVideo:Hidd] %s()\n", __func__));

    for (vpd = msg->Data; vpd; vpd = vpd->Next)
    {
        bmdata = OOP_INST_DATA(OOP_OCLASS(vpd->Bitmap), vpd->Bitmap);
        bmdata->node.ln_Name = (char *)vpd;

        if (bmdata->bmcl)
        {
            D(bug("[AmigaVideo:Hidd] %s: DspIns @ 0x%p for ViewPort @ 0x%p\n", __func__, bmdata->bmcl, vpd->vpe->ViewPort));

            if (!(bmdata->bmcl->CopLStart))
            {
                bmdata->bmcl->CopLStart = AllocVec((bmdata->bmcl->MaxCount << 2), MEMF_CLEAR | MEMF_CHIP);
                D(bug("[AmigaVideo:Hidd] %s:    allocated %d bytes for new copperlist data buffer\n", __func__, (bmdata->bmcl->MaxCount << 2));)
            }
            D(bug("[AmigaVideo:Hidd] %s:    copperlist data @ 0x%p\n", __func__, bmdata->bmcl->CopLStart);)
            bmdata->bmcl->Count = ((IPTR)populatebmcopperlist(csd, bmdata, &bmdata->copld, bmdata->bmcl->CopLStart, FALSE) - (IPTR)bmdata->bmcl->CopLStart) >> 2;

            if (bmdata->interlace != 0)
            {
                if (!(bmdata->bmcl->CopSStart))
                {
                    bmdata->bmcl->CopSStart = AllocVec((bmdata->bmcl->MaxCount << 2), MEMF_CLEAR | MEMF_CHIP);
                    D(bug("[AmigaVideo:Hidd] %s:    allocated new interlaced copperlist data buffer\n", __func__);)
                }
                D(bug("[AmigaVideo:Hidd] %s:    interlaced copperlist data @ 0x%p\n", __func__, bmdata->bmcl->CopSStart);)
                populatebmcopperlist(csd, bmdata, &bmdata->copsd, bmdata->bmcl->CopSStart, TRUE);
            }
            else
            {
                FreeVec(bmdata->bmcl->CopSStart);
                bmdata->bmcl->CopSStart = NULL;
            }

            setfmode(csd, bmdata);
            setcoppercolors(csd, bmdata, bmdata->palette,
                            (void *)vpd == (void *)msg->Data);

            /* handle the viewports 'struct UCopList *' */
            if (vpd->vpe->ViewPort->UCopIns && vpd->vpe->ViewPort->UCopIns->FirstCopList)
            {
#if USE_UCOP_DIRECT
                vpd->vpe->ViewPort->UCopIns->FirstCopList->CopLStart = bmdata->copld.copper2_tail;
                if (bmdata->interlace != 0)
                    vpd->vpe->ViewPort->UCopIns->FirstCopList->CopSStart = bmdata->copsd.copper2_tail;
#endif
                AmigaVideo_ParseCopperlist(csd, bmdata, vpd->vpe->ViewPort->UCopIns->FirstCopList);
            }
        }
    }

    return retval;
}

ULONG AmigaVideoCl__Hidd_Gfx__ShowViewPorts(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_ShowViewPorts *msg)
{
    struct amigavideo_staticdata *csd = CSD(cl);
    struct Library *OOPBase = csd->cs_OOPBase;
    struct amigagfx_data *data = OOP_INST_DATA(cl, o);
    struct pHidd_Compositor_BitMapStackChanged bscmsg =
    {
        mID : csd->mid_BitMapStackChanged,
    };
    bscmsg.data = msg->Data;

    D(bug("[AmigaVideo:Hidd] %s: first ViewPort @ 0x%p\n", __func__, msg->Data));

#if USE_FAST_BMSTACKCHANGE
        data->bmstackchange(data->bmstackchange_Class, data->compositor, (OOP_Msg)&bscmsg.mID);
#else
    OOP_DoMethod(data->compositor, (OOP_Msg)&bscmsg.mID);
#endif

    return TRUE;
}

VOID AmigaVideoCl__Hidd_Gfx__DisplayToBMCoords(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_DisplayToBMCoords *msg)
{
    struct amigavideo_staticdata *csd = CSD(cl);
    struct amigabm_data *tbmdata;

    D(bug("[AmigaVideo:Hidd] %s: Target BitMap Object @ 0x%p\n", __func__, msg->Target));

    tbmdata = OOP_INST_DATA(OOP_OCLASS(msg->Target), msg->Target);
    if ((csd->interlaced && (tbmdata->interlace != 0)) ||
        (!csd->interlaced && (tbmdata->interlace == 0)))
    {
        *msg->TargetY = msg->DispY;
    }
    else if (csd->interlaced && (tbmdata->interlace == 0))
    {
        *msg->TargetY = msg->DispY >> 1;
    }
    else
        *msg->TargetY = msg->DispY << 1;

    switch (tbmdata->res)
    {
        case 2:
            *msg->TargetX = msg->DispX;
            break;
        
        case 1:
            if (csd->displaywidth > (REZ_X_MIN << 1))
                *msg->TargetX = msg->DispX >> 1;
            else
                *msg->TargetX = msg->DispX;
            break;

        default:
            if (csd->displaywidth > (REZ_X_MIN << 1))
                *msg->TargetX = msg->DispX >> 2;
            if (csd->displaywidth > REZ_X_MIN)
                *msg->TargetX = msg->DispX >> 1;
            else
                *msg->TargetX = msg->DispX;
            break;
    }
    D(bug("[AmigaVideo:Hidd] %s: %d,%d -> %d,%d\n", __func__, msg->DispX, msg->DispY, *msg->TargetX, *msg->TargetY));
}

VOID AmigaVideoCl__Hidd_Gfx__BMToDisplayCoords(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_BMToDisplayCoords *msg)
{
    struct amigavideo_staticdata *csd = CSD(cl);
    struct amigabm_data *tbmdata;

    D(bug("[AmigaVideo:Hidd] %s: Target BitMap Object @ 0x%p\n", __func__, msg->Target));

    tbmdata = OOP_INST_DATA(OOP_OCLASS(msg->Target), msg->Target);
    if ((csd->interlaced && (tbmdata->interlace != 0)) ||
        (!csd->interlaced && (tbmdata->interlace == 0)))
    {
        *msg->DispY = msg->TargetY;
    }
    else if (csd->interlaced && (tbmdata->interlace == 0))
    {
        *msg->DispY = msg->TargetY << 1;
    }
    else
        *msg->DispY = msg->TargetY >> 1;

    switch (tbmdata->res)
    {
        case 2:
            *msg->DispX = msg->TargetX;
            break;
        
        case 1:
            if (csd->displaywidth > (REZ_X_MIN << 1))
                *msg->DispX = msg->TargetX << 1;
            break;

        default:
            if (csd->displaywidth > (REZ_X_MIN << 1))
                *msg->DispX = msg->TargetX << 2;
            if (csd->displaywidth > REZ_X_MIN)
                *msg->DispX = msg->TargetX << 1;
            else
                *msg->DispX = msg->TargetX;
            break;
    }
    D(bug("[AmigaVideo:Hidd] %s: %d,%d -> %d,%d\n", __func__, msg->TargetX, msg->TargetY, *msg->DispX, *msg->DispY));
}

static void freeattrbases(struct amigavideo_staticdata *csd)
{
    struct Library *OOPBase = csd->cs_OOPBase;

    OOP_ReleaseAttrBase(IID_Hidd);
    OOP_ReleaseAttrBase(IID_Hidd_BitMap);
    OOP_ReleaseAttrBase(IID_Hidd_PlanarBM);
    OOP_ReleaseAttrBase(IID_Hidd_BitMap_AmigaVideo);
    OOP_ReleaseAttrBase(IID_Hidd_GC);
    OOP_ReleaseAttrBase(IID_Hidd_Sync);
    OOP_ReleaseAttrBase(IID_Hidd_Gfx);
    OOP_ReleaseAttrBase(IID_Hidd_Compositor);
    OOP_ReleaseAttrBase(IID_Hidd_PixFmt);
    OOP_ReleaseAttrBase(IID_Hidd_ColorMap);
}

int Init_AmigaVideoClass(LIBBASETYPEPTR LIBBASE)
{
    struct amigavideo_staticdata *csd = &LIBBASE->csd;
    struct Library *OOPBase = csd->cs_OOPBase;

    D(bug("[AmigaVideo:Hidd] %s()\n", __func__));
    __IHidd             = OOP_ObtainAttrBase(IID_Hidd);
    __IHidd_BitMap      = OOP_ObtainAttrBase(IID_Hidd_BitMap);
    __IHidd_PlanarBM    = OOP_ObtainAttrBase(IID_Hidd_PlanarBM);
    __IHidd_BitMap_AmigaVideo   = OOP_ObtainAttrBase(IID_Hidd_BitMap_AmigaVideo);
    __IHidd_GC          = OOP_ObtainAttrBase(IID_Hidd_GC);
    __IHidd_Sync        = OOP_ObtainAttrBase(IID_Hidd_Sync);
    __IHidd_Gfx         = OOP_ObtainAttrBase(IID_Hidd_Gfx);
    __IHidd_Compositor          = OOP_ObtainAttrBase(IID_Hidd_Compositor);
    __IHidd_PixFmt              = OOP_ObtainAttrBase(IID_Hidd_PixFmt);
    __IHidd_ColorMap    = OOP_ObtainAttrBase(IID_Hidd_ColorMap);

    if (!__IHidd || !__IHidd_BitMap || !__IHidd_PlanarBM || !__IHidd_BitMap_AmigaVideo ||
        !__IHidd_GC || !__IHidd_Sync || !__IHidd_Gfx || !__IHidd_PixFmt || !__IHidd_ColorMap)
    {
        D(bug("[AmigaVideo:Hidd] %s: failed to obtain attribute bases\n", __func__));
        freeattrbases(csd);
        return 0;
    }

    LIBBASE->csd.mid_BitMapStackChanged   =
        OOP_GetMethodID((STRPTR)IID_Hidd_Compositor, moHidd_Compositor_BitMapStackChanged);
    LIBBASE->csd.mid_BitMapPositionChanged   =
        OOP_GetMethodID((STRPTR)IID_Hidd_Compositor, moHidd_Compositor_BitMapPositionChanged);
    LIBBASE->csd.mid_BitMapRectChanged       =
        OOP_GetMethodID((STRPTR)IID_Hidd_Compositor, moHidd_Compositor_BitMapRectChanged);
    LIBBASE->csd.mid_ValidateBitMapPositionChange =
        OOP_GetMethodID((STRPTR)IID_Hidd_Compositor, moHidd_Compositor_ValidateBitMapPositionChange);

    return TRUE;
}

static int Expunge_AmigaVideoClass(LIBBASETYPEPTR LIBBASE)
{
    struct amigavideo_staticdata *csd = &LIBBASE->csd;
    D(bug("[AmigaVideo:Hidd] %s()\n", __func__));
    freeattrbases(csd);
    return TRUE;
}

ADD2EXPUNGELIB(Expunge_AmigaVideoClass, 1)
