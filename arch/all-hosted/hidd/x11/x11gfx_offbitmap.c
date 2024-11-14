/*
    Copyright (C) 1995-2017, The AROS Development Team. All rights reserved.

    Desc: Offscreen bitmap class for X11 hidd.
*/

/****************************************************************************************/

#include "x11_debug.h"

#include <proto/utility.h>
#include <hidd/gfx.h>

#include "x11_types.h"
#include "x11.h"
#include "x11_hostlib.h"
#include "x11gfx_bitmap.h"

/****************************************************************************************/

BOOL X11BM_InitPM(OOP_Class *cl, OOP_Object *o, struct TagItem *attrList)
{
    OOP_Object *friend;
    Drawable friend_drawable = 0;
    IPTR depth;
    IPTR private1;
    struct bitmap_data *data = OOP_INST_DATA(cl, o);

    D(bug("[X11OffBm] %s()\n", __PRETTY_FUNCTION__));

    /* Retrieve bitmap size from superclass */
    OOP_GetAttr(o, aHidd_BitMap_Width , &data->width);
    OOP_GetAttr(o, aHidd_BitMap_Height, &data->height);
    OOP_GetAttr(o, aHidd_BitMap_Depth , &depth);

    friend = (OOP_Object *)GetTagData(aHidd_BitMap_Friend, 0, attrList);
    if (friend)
    {
        /* Get the X11 window from the friend bitmap */
        OOP_GetAttr(friend, aHidd_BitMap_X11_Drawable, &friend_drawable);
    }
    
    if (!friend_drawable)
    {
        /* If no friend, or friend is not X11 bitmap, use default friend drawable */
        friend_drawable = XSD(cl)->dummy_window_for_creating_pixmaps;
    }

    /*
     * We must only create depths that are supported by the friend drawable
     * Currently we only support the default depth, and depth 1
     */
    if (depth != 1)
    {
        depth = DefaultDepth(data->display, data->screen);
    }
    else
    {
        /* Need this because of stipple bug in XFree86 :-( */
        data->width += 32;
    }

    D(bug("[X11OffBm] %s: Creating X Pixmap, 0x%p, %ld, %ld, %ld\n", __PRETTY_FUNCTION__, friend_drawable, data->width, data->height, depth));

    HostLib_Lock();

    DRAWABLE(data) = XCALL(XCreatePixmap, data->display, friend_drawable, data->width, data->height, depth);
    XCALL(XFlush, data->display);

    HostLib_Unlock();

    private1 = GetTagData(aHidd_BitMap_Private1, 0, attrList);

    if (private1 != 0)
    {
        data->flags |= BMDF_FRAMEBUFFER;
        WINDRAWABLE(data) = private1; // needed for list of handled windows to work
        X11BM_NotifyFB(cl, o); // add window to list of handled windows
    }

    return DRAWABLE(data) ? TRUE : FALSE;
}

/****************************************************************************************/

VOID X11BM_DisposePM(struct bitmap_data *data)
{
    D(bug("[X11OffBm] %s()\n", __PRETTY_FUNCTION__));

    if (DRAWABLE(data))
    {
        HostLib_Lock();

        XCALL(XFreePixmap, GetSysDisplay(), DRAWABLE(data));
        XCALL(XFlush, GetSysDisplay());

        HostLib_Unlock();
    }
}

/****************************************************************************************/

VOID X11BM_ClearPM(struct bitmap_data *data, HIDDT_Pixel bg)
{
    D(bug("[X11OffBm] %s()\n", __PRETTY_FUNCTION__));

    XCALL(XSetForeground, data->display, data->gc, bg);
    XCALL(XFillRectangle, data->display, DRAWABLE(data), data->gc, 0, 0, data->width, data->height);
}

/****************************************************************************************/

BOOL X11BM_ResizePMIfNeeded(OOP_Class *cl, OOP_Object *o, UWORD reqwidth, UWORD reqheight, UWORD *actwidth, UWORD *actheight)
{
    struct bitmap_data *data = OOP_INST_DATA(cl, o);
    ULONG newwidth = data->width, newheight = data->height;

    while ((newwidth) < reqwidth) { newwidth *= 130; newwidth /= 100; }
    while ((newheight) < reqheight) { newheight *= 130; newheight /= 100; }

    if (data->width != newwidth || data->height != newheight)
    {
        Drawable newd = XCALL(XCreatePixmap, data->display, DRAWABLE(data), newwidth, newheight, DefaultDepth(data->display, data->screen));
        Drawable oldd = DRAWABLE(data);

        DRAWABLE(data) = newd;

        XCALL(XSetFunction, data->display, data->gc, GXcopy);
        XCALL(XCopyArea, data->display, oldd, newd, data->gc, 0, 0, data->width, data->height, 0, 0);

        XCALL(XFreePixmap, GetSysDisplay(), oldd);
        XCALL(XFlush, data->display);

        struct TagItem attrs[] =
        {
            { aHidd_BitMap_Width, newwidth },
            { aHidd_BitMap_Height, newheight},
            { TAG_DONE, TAG_DONE }
        };

        OOP_SetAttrs(o, attrs);

        *actwidth = data->width = newwidth;
        *actheight = data->height = newheight;

        return TRUE;
    }

    return FALSE;
}
