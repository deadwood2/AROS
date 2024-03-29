/*
    Copyright (C) 1995-2020, The AROS Development Team. All rights reserved.

    Desc: VESA Gfx Hidd for standalone i386 AROS
*/

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/oop.h>

#include <exec/types.h>
#include <exec/lists.h>
#include <graphics/driver.h>
#include <graphics/gfxbase.h>
#include <hidd/gfx.h>
#include <oop/oop.h>
#include <utility/utility.h>
#include <aros/symbolsets.h>

#include "vesagfx_support.h"
#include "vesagfx_hidd.h"

#include LC_LIBDEFS_FILE

#include <aros/debug.h>

/*
 * The following two functions are candidates for inclusion into oop.library.
 * For slightly other implementation see incomplete Android-hosted graphics driver.
 */
static void FreeAttrBases(const STRPTR *iftable, OOP_AttrBase *bases, ULONG num)
{
    ULONG i;
    
    for (i = 0; i < num; i++)
    {
        if (bases[i])
            OOP_ReleaseAttrBase(iftable[i]);
    }
}

static BOOL GetAttrBases(const STRPTR *iftable, OOP_AttrBase *bases, ULONG num)
{
    ULONG i;

    for (i = 0; i < num; i++)
    {
        bases[i] = OOP_ObtainAttrBase(iftable[i]);
        if (!bases[i])
        {
            FreeAttrBases(iftable, bases, i);
            return FALSE;
        }
    }

    return TRUE;
}

/* These must stay in the same order as attrBases[] entries assignment in vesagfxclass.h */
static const STRPTR interfaces[ATTRBASES_NUM] =
{
    IID_Hidd_ChunkyBM,
    IID_Hidd_BitMap,
    IID_Hidd_Gfx,
    IID_Hidd_PixFmt,
    IID_Hidd_Sync,
    IID_Hidd
};

static int VESAGfx_Init(LIBBASETYPEPTR LIBBASE)
{
    struct VESAGfx_staticdata *xsd = &LIBBASE->vsd;
    struct GfxBase *GfxBase;
    ULONG err;
    int res = FALSE;

    /*
     * Open graphics.library ourselves because we will close it
     * after adding the driver.
     * Autoinit code would close it only upon driver expunge.
     */
    GfxBase = (struct GfxBase *)TaggedOpenLibrary(TAGGEDOPEN_GRAPHICS);
    if (GfxBase)
    {
        if (initVesaGfxHW(&xsd->data))
        {
            if (GetAttrBases(interfaces, xsd->attrBases, ATTRBASES_NUM))
            {
                xsd->basebm = OOP_FindClass(CLID_Hidd_BitMap);
                D(bug("[VESAGfx] BitMap class @ 0x%p\n", xsd->basebm));

                InitSemaphore(&xsd->framebufferlock);
                InitSemaphore(&xsd->HW_acc);

                D(bug("[VESAGfx] Init: Everything OK, installing driver\n"));
                
                /*
                 * It is unknown (and no way to know) what hardware part this driver uses.
                 * In order to avoid conflicts with disk-based native-mode hardware
                 * drivers it needs to be removed from the system when some other driver
                 * is installed.
                 * This is done by graphics.library if DDRV_BootMode is set to TRUE.
                 */
                err = AddDisplayDriver(xsd->vesagfxclass, NULL, DDRV_BootMode, TRUE, TAG_DONE);

                D(bug("[VESAGfx] AddDisplayDriver() result: %u\n", err));
                if (!err)
                {
                    /* expunge protection */
                    LIBBASE->library.lib_OpenCnt = 1;
                    res = TRUE;
                }
            }
        }
        CloseLibrary(&GfxBase->LibNode);
    }
    else
    {
        D(bug("[VESAGfx] Failed to open graphics.library!\n"));
    }
    return res;
}

ADD2INITLIB(VESAGfx_Init, 0)
