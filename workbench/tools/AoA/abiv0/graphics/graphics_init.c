#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <aros/debug.h>


#include "../include/exec/structures.h"
#include "../include/exec/functions.h"
#include "../include/aros/cpu.h"
#include "../include/aros/proxy.h"
#include "../include/graphics/structures.h"
#include "../include/graphics/proxy_structures.h"
#include "../include/utility/structures.h"

struct ExecBaseV0 *Gfx_SysBaseV0;

struct LibraryV0 *abiv0_Gfx_OpenLib(ULONG version, struct LibraryV0 *GfxBaseV0)
{
    return GfxBaseV0;
}
MAKE_PROXY_ARG_2(Gfx_OpenLib)

ULONG abiv0_GetBitMapAttr(struct BitMapV0 *bitmap, ULONG attribute, struct LibraryV0 *GfxBaseV0)
{
bug("abiv0_GetBitMapAttr: STUB\n");
    if (attribute == BMA_DEPTH)
    {
        return 24;
    }
asm("int3");
}
MAKE_PROXY_ARG_3(GetBitMapAttr)

void abiv0_GetRGB32(struct ColorMapV0 * cm, ULONG firstcolor, ULONG ncolors, ULONG *table, struct LibraryV0 *GfxBaseV0)
{
    struct ColorMapProxy *proxy = (struct ColorMapProxy *)cm;
    return GetRGB32(proxy->native, firstcolor, ncolors, table);
}
MAKE_PROXY_ARG_5(GetRGB32)

APTR abiv0_OpenFont(APTR textAttr, struct LibraryV0 *GfxBaseV0)
{
bug("abiv0_OpenFont: STUB\n");
    return NULL;
}
MAKE_PROXY_ARG_3(OpenFont)

LONG abiv0_ObtainBestPenA(struct ColorMapV0 *cm, ULONG r, ULONG g, ULONG b, struct TagItemV0 *tags, struct LibraryV0 *GfxBaseV0)
{
    struct ColorMapProxy *proxy = (struct ColorMapProxy *)cm;

    if (tags == NULL)
    {
        return ObtainBestPenA(proxy->native, r, g, b, NULL);
    }
    else
    {
        if (tags[0].ti_Tag == OBP_FailIfBad)
        {
            struct TagItem tagtmp[] =
            {
                { OBP_FailIfBad, FALSE },
                { TAG_DONE, 0L}
            };

            tagtmp[0].ti_Data = tags[0].ti_Data;
            return ObtainBestPenA(proxy->native, r, g, b, tagtmp);
        }
        else
        {
asm("int3");
        }
    }
    return 0;
}
MAKE_PROXY_ARG_6(ObtainBestPenA)

struct LibraryV0 *shallow_InitResident32(struct ResidentV0 *resident, BPTR segList, struct ExecBaseV0 *SysBaseV0);
BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);
struct ResidentV0 * findResident(BPTR seg, CONST_STRPTR name);
APTR abiv0_DOS_OpenLibrary(CONST_STRPTR name, ULONG version, struct ExecBaseV0 *SysBaseV0);

void init_graphics(struct ExecBaseV0 *SysBaseV0)
{
    BPTR graphicsseg = LoadSeg32("SYS:Libs32/partial/graphics.library", DOSBase);
    struct ResidentV0 *graphicsres = findResident(graphicsseg, NULL);
    struct GfxBaseV0 *abiv0GfxBase = (struct GfxBaseV0 *)shallow_InitResident32(graphicsres, graphicsseg, SysBaseV0);
    Gfx_SysBaseV0 = SysBaseV0;
    /* Remove all vectors for now */
    const ULONG graphicsjmpsize = 202 * sizeof(APTR32);
    APTR32 *graphicsjmp = AllocMem(graphicsjmpsize, MEMF_CLEAR);
    CopyMem((APTR)abiv0GfxBase - graphicsjmpsize, graphicsjmp, graphicsjmpsize);
    for (int i = 1; i <= 201; i++) __AROS_SETVECADDRV0(abiv0GfxBase, i, 0);
    abiv0GfxBase->ExecBase = (APTR32)(IPTR)SysBaseV0;
    *(ULONG *)((IPTR)abiv0GfxBase + 0x4b0) = (APTR32)(IPTR)abiv0_DOS_OpenLibrary("utility.library", 0L, SysBaseV0);
    NEWLISTV0(&abiv0GfxBase->TextFonts);

    __AROS_SETVECADDRV0(abiv0GfxBase,   1, (APTR32)(IPTR)proxy_Gfx_OpenLib);
    __AROS_SETVECADDRV0(abiv0GfxBase,  12, (APTR32)(IPTR)proxy_OpenFont);
    __AROS_SETVECADDRV0(abiv0GfxBase, 160, (APTR32)(IPTR)proxy_GetBitMapAttr);
    __AROS_SETVECADDRV0(abiv0GfxBase, 150, (APTR32)(IPTR)proxy_GetRGB32);
    __AROS_SETVECADDRV0(abiv0GfxBase, 140, (APTR32)(IPTR)proxy_ObtainBestPenA);
    __AROS_SETVECADDRV0(abiv0GfxBase,  33, graphicsjmp[202 -  33]);  // InitRastPort
    __AROS_SETVECADDRV0(abiv0GfxBase,  11, graphicsjmp[202 -  11]);  // SetFont
    __AROS_SETVECADDRV0(abiv0GfxBase, 134, graphicsjmp[202 - 134]);  // WeighTAMatch
    __AROS_SETVECADDRV0(abiv0GfxBase, 136, graphicsjmp[202 - 136]);  // ExtendFont
    __AROS_SETVECADDRV0(abiv0GfxBase,  80, graphicsjmp[202 -  80]);  // AddFont
    __AROS_SETVECADDRV0(abiv0GfxBase,   9, graphicsjmp[202 -   9]);  // TextLength
}
