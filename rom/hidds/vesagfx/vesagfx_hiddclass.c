/*
    Copyright (C) 1995-2023, The AROS Development Team. All rights reserved.

    Desc: Class for VESA.
*/

#include <aros/asmcall.h>
#include <proto/exec.h>
#include <proto/oop.h>
#include <proto/utility.h>
#include <aros/symbolsets.h>
#include <devices/inputevent.h>
#include <exec/alerts.h>
#include <exec/memory.h>
#include <hardware/custom.h>
#include <hidd/hidd.h>
#include <hidd/gfx.h>
#include <oop/oop.h>
#include <clib/alib_protos.h>
#include <string.h>

#define DEBUG 0
#include <aros/debug.h>

#include "vesagfx_hidd.h"
#include "vesagfx_support.h"

#include LC_LIBDEFS_FILE

AROS_INTH1(ResetHandler, struct HWData *, hwdata)
{
    AROS_INTFUNC_INIT

    ClearBuffer(hwdata);

    return FALSE;

    AROS_INTFUNC_EXIT
}

OOP_Object *VESAGfx__Root__New(OOP_Class *cl, OOP_Object *o, struct pRoot_New *msg)
{
    struct TagItem pftags[] =
    {
        {aHidd_PixFmt_RedShift,     0}, /*  0 */
        {aHidd_PixFmt_GreenShift,   0}, /*  1 */
        {aHidd_PixFmt_BlueShift,    0}, /*  2 */
        {aHidd_PixFmt_AlphaShift,   0}, /*  3 */
        {aHidd_PixFmt_RedMask,      0}, /*  4 */
        {aHidd_PixFmt_GreenMask,    0}, /*  5 */
        {aHidd_PixFmt_BlueMask,     0}, /*  6 */
        {aHidd_PixFmt_AlphaMask,    0}, /*  7 */
        {aHidd_PixFmt_ColorModel,   0}, /*  8 */
        {aHidd_PixFmt_Depth,        0}, /*  9 */
        {aHidd_PixFmt_BytesPerPixel,0}, /* 10 */
        {aHidd_PixFmt_BitsPerPixel, 0}, /* 11 */
        {aHidd_PixFmt_StdPixFmt,    vHidd_StdPixFmt_Native}, /* 12 */
        {aHidd_PixFmt_CLUTShift,    0}, /* 13 */
        {aHidd_PixFmt_CLUTMask,     0}, /* 14 */
        {aHidd_PixFmt_BitMapType,   vHidd_BitMapType_Chunky}, /* 15 */
        {TAG_DONE, 0UL }
    };
    struct TagItem sync_mode[] =
    {
        {aHidd_Sync_PixelClock,         0                       },
        {aHidd_Sync_HTotal,             0                       },
        {aHidd_Sync_HDisp,              0                       },
        {aHidd_Sync_VDisp,              0                       },
        {aHidd_Sync_HMax,               16384                   },
        {aHidd_Sync_VMax,               16384                   },
        {aHidd_Sync_Description,        (IPTR)"VESA:%hx%v"      },
        {TAG_DONE,                      0UL                     }
    };
    struct TagItem modetags[] =
    {
        {aHidd_Gfx_PixFmtTags, (IPTR)pftags},
        {aHidd_Gfx_SyncTags,   (IPTR)sync_mode},
        {TAG_DONE, 0UL}
    };
    struct TagItem msgNewTags[] =
    {
        {aHidd_Gfx_ModeTags, (IPTR)modetags},
        { aHidd_Name            , (IPTR)"vesagfx.hidd"     },
        { aHidd_HardwareName    , (IPTR)"VESA Compatible Controller"   },
        { aHidd_ProducerName    , (IPTR)"vesa.org"  },
        {TAG_MORE, 0UL}
    };
    struct pRoot_New msgNew;

    EnterFunc(bug("VESAGfx::New()\n"));

    /* Protect against some stupid programmer wishing to
       create one more VESA driver */
    if (XSD(cl)->vesagfxhidd)
        return NULL;

    pftags[0].ti_Data = XSD(cl)->data.redshift;
    pftags[1].ti_Data = XSD(cl)->data.greenshift;
    pftags[2].ti_Data = XSD(cl)->data.blueshift;
    pftags[4].ti_Data = XSD(cl)->data.redmask;
    pftags[5].ti_Data = XSD(cl)->data.greenmask;
    pftags[6].ti_Data = XSD(cl)->data.bluemask;
    pftags[8].ti_Data = (XSD(cl)->data.depth > 8) ? vHidd_ColorModel_TrueColor : vHidd_ColorModel_Palette;
    pftags[9].ti_Data = (XSD(cl)->data.depth > 24) ? 24 : XSD(cl)->data.depth;
    pftags[10].ti_Data = XSD(cl)->data.bytesperpixel;
    pftags[11].ti_Data = (XSD(cl)->data.bitsperpixel > 24) ? 24 : XSD(cl)->data.bitsperpixel;
    pftags[14].ti_Data = (1 << XSD(cl)->data.depth) - 1;

    sync_mode[0].ti_Data = 60 * XSD(cl)->data.width * XSD(cl)->data.height;
    sync_mode[1].ti_Data = XSD(cl)->data.width;
    sync_mode[2].ti_Data = XSD(cl)->data.width;
    sync_mode[3].ti_Data = XSD(cl)->data.height;

    if ((msgNewTags[4].ti_Data = (IPTR)msg->attrList) == 0)
        msgNewTags[4].ti_Tag = TAG_DONE;

    msgNew.mID = msg->mID;
    msgNew.attrList = msgNewTags;

    o = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg)&msgNew);
    if (o)
    {
        struct VESAGfxHiddData *data = OOP_INST_DATA(cl, o);

        D(bug("Got object from super\n"));
        XSD(cl)->vesagfxhidd = o;

        data->ResetInterrupt.is_Node.ln_Name = (char *)msgNewTags[1].ti_Data;
        data->ResetInterrupt.is_Code = (VOID_FUNC)ResetHandler;
        data->ResetInterrupt.is_Data = &XSD(cl)->data;
        AddResetCallback(&data->ResetInterrupt);
    }
    ReturnPtr("VESAGfx::New", OOP_Object *, o);
}

VOID VESAGfx__Root__Dispose(OOP_Class *cl, OOP_Object *o, OOP_Msg msg)
{
    struct VESAGfxHiddData *data = OOP_INST_DATA(cl, o);

    RemResetCallback(&data->ResetInterrupt);
    OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
    XSD(cl)->vesagfxhidd = NULL;
}

VOID VESAGfx__Root__Get(OOP_Class *cl, OOP_Object *o, struct pRoot_Get *msg)
{
    ULONG idx;

    if (IS_GFX_ATTR(msg->attrID, idx))
    {
        switch (idx)
        {
            case aoHidd_Gfx_NoFrameBuffer:
                *msg->storage = TRUE;
                return;
        }
    }
    OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
}

OOP_Object *VESAGfx__Hidd_Gfx__CreateObject(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_CreateObject *msg)
{
    OOP_Object      *object = NULL;

    D(bug("[VESA] %s()\n", __PRETTY_FUNCTION__));
    D(bug("[VESA] %s: requested class 0x%p\n", __PRETTY_FUNCTION__, msg->cl));
    D(bug("[VESA] %s: base bitmap class 0x%p\n", __PRETTY_FUNCTION__, XSD(cl)->basebm));

    if (msg->cl == XSD(cl)->basebm)
    {
        BOOL displayable;
        struct TagItem tags[2] =
        {
            {TAG_IGNORE, 0                  },
            {TAG_MORE  , (IPTR)msg->attrList}
        };
        struct pHidd_Gfx_CreateObject p;

        displayable = GetTagData(aHidd_BitMap_Displayable, FALSE, msg->attrList);
        if (displayable)
        {
            /* Only displayable bitmaps are bitmaps of our class */
            tags[0].ti_Tag  = aHidd_BitMap_ClassPtr;
            tags[0].ti_Data = (IPTR)XSD(cl)->bmclass;
        }
        else
        {
            /* Non-displayable friends of our bitmaps are plain chunky bitmaps */
            OOP_Object *friend = (OOP_Object *)GetTagData(aHidd_BitMap_Friend, 0, msg->attrList);

            if (friend && (OOP_OCLASS(friend) == XSD(cl)->bmclass))
            {
                tags[0].ti_Tag  = aHidd_BitMap_ClassID;
                tags[0].ti_Data = (IPTR)CLID_Hidd_ChunkyBM;
            }
        }

        p.mID = msg->mID;
        p.cl = msg->cl;
        p.attrList = tags;

        object = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg)&p);
    }
    else
        object = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);

    ReturnPtr("VESAGfx::CreateObject", OOP_Object *, object);
}

/*********  GfxHidd::Show()  ***************************/

OOP_Object *VESAGfx__Hidd_Gfx__Show(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_Show *msg)
{
    struct VESAGfx_staticdata *data = XSD(cl);
    struct TagItem tags[] = {
        {aHidd_BitMap_Visible, FALSE},
        {TAG_DONE            , 0    }
    };

    D(bug("[VESAGfx] Show(0x%p), old visible 0x%p\n", msg->bitMap, data->visible));

    LOCK_FRAMEBUFFER(data);

    /* Remove old bitmap from the screen */
    if (data->visible)
    {
        D(bug("[VESAGfx] Hiding old bitmap\n"));
        OOP_SetAttrs(data->visible, tags);
    }

    if (msg->bitMap)
    {
        /* If we have a bitmap to show, set it as visible */
        D(bug("[VESAGfx] Showing new bitmap\n"));
        tags[0].ti_Data = TRUE;
        OOP_SetAttrs(msg->bitMap, tags);
    }
    else
    {
        D(bug("[VESAGfx] Blanking screen\n"));
        /* Otherwise simply clear the framebuffer */
        ClearBuffer(&data->data);
    }

    data->visible = msg->bitMap;
    UNLOCK_FRAMEBUFFER(data);

    D(bug("[VESAGfx] Show() done\n"));
    return msg->bitMap;
}
