#ifndef ABIV0_GRAPHICS_STRUCTURES_H
#define ABIV0_GRAPHICS_STRUCTURES_H

#include "../exec/structures.h"

/*          *** ColorMap ***
 *
 * This structure is the primary storage for palette data.
 * 
 * Color data itself is stored in two tables: ColorTable and LowColorBits.
 * These fields are actually pointer to arrays of UWORDs. Each UWORD corresponds
 * to one color.
 * Number of UWORDs in these arrays is equal to Count value in this structure.
 * ColorTable stores upper nibbles of RGB values, LowColorBits stores low nibbles.
 *
 * Example:
 *  color number 4, value: 0x00ABCDEF
 *  ColorTable  [4] = 0x0ACE,
 *  LowColorBits[4] = 0x0BDF
 *
 * SpriteBase fields keep bank number, not a color number. On m68k Amiga colors are divided into
 * banks, 16 per each. So bank number is color number divided by 16. Base color is a number which
 * is added to all colors of the sprite in order to look up the actual palette entry.
 * AROS may run on different hardware where sprites may have base colors that do not divide by 16.
 * In order to cover this bank numbers have a form: ((c & 0x0F) << 8 ) | (c >> 4), where c is actual
 * color number (i. e. remainder is stored in a high byte of UWORD).
 * 
 */

struct ColorMapV0
{
    UBYTE Flags;      				/* see below */
    UBYTE Type;       				/* Colormap type (reflects version), see below */
    UWORD Count;				/* Number of palette entries */
    APTR32 ColorTable;				/* Table of high nibbles of color values (see description above) */

    /* The following fields are present only if Type >= COLORMAP_TYPE_V36 */

    APTR32 cm_vpe;		/* ViewPortExtra, for faster access */

    APTR32 LowColorBits;			/* Table of low nibbles of color values (see above) */
    UBYTE TransparencyPlane;
    UBYTE SpriteResolution;			/* see below */
    UBYTE SpriteResDefault;
    UBYTE AuxFlags;

    APTR32 cm_vp;			/* Points back to a ViewPort this colormap belongs to */

    APTR NormalDisplayInfo;
    APTR CoerceDisplayInfo;

    APTR32  cm_batch_items;
    ULONG                 VPModeID;

    /* The following fields are present only if Type >= COLORMAP_TYPE_V39 */
    
    APTR32  PalExtra;		/* Structure controlling palette sharing */

    UWORD SpriteBase_Even;			/* Color bank for even sprites (see above) */
    UWORD SpriteBase_Odd;			/* The same for odd sprites		   */
    UWORD Bp_0_base;
    UWORD Bp_1_base;
};

/*          *** ViewPort ***
 *
 * Describes a displayed bitmap (or logical screen).
 *
 * Copperlists are relevant only to Amiga(tm) chipset, for other hardware they are NULL.
 *
 */

struct ViewPortV0
{
    APTR32 Next;	 /* Pointer to a next ViewPort in the view (NULL for the last ViewPort) */

    APTR32 ColorMap;  /* Points to a ColorMap */
    APTR32 DspIns;	 /* Preliminary partial display copperlist */
    APTR32 SprIns;    /* Preliminary partial sprite copperlist */
    APTR32 ClrIns;
    APTR32 UCopIns;   /* User-defined part of the copperlist */

    WORD  DWidth;		 /* Width of currently displayed part in pixels */
    WORD  DHeight;		 /* Height of currently displayed part in pixels */
    WORD  DxOffset;		 /* Displacement from the (0, 0) of the physical screen to (0, 0) of the raster */
    WORD  DyOffset;
    UWORD Modes;		 /* The same as in View */

    UBYTE SpritePriorities;
    UBYTE ExtendedModes;

    APTR32 RasInfo;	/* Playfield specification */
};

struct RastPortV0
{
    APTR32 Layer;
    APTR32 BitMap;
    APTR32 AreaPtrn;
    APTR32 TmpRas;
    APTR32 AreaInfo;
    APTR32 GelsInfo;
    UBYTE             Mask;
    BYTE              FgPen;
    BYTE              BgPen;
    BYTE              AOlPen;
    BYTE              DrawMode;
    BYTE              AreaPtSz;
    BYTE              linpatcnt;
    BYTE              dummy;
    UWORD             Flags;
    UWORD             LinePtrn;
    WORD              cp_x;
    WORD              cp_y;
    UBYTE             minterms[8];
    WORD              PenWidth;
    WORD              PenHeight;
    APTR32 Font;
    UBYTE             AlgoStyle;
    UBYTE             TxFlags;
    UWORD             TxHeight;
    UWORD             TxWidth;
    UWORD             TxBaseline;
    WORD              TxSpacing;
    APTR32 RP_User;
    /*
     * 30 bytes (on 32-bit architectures) of private space follow.
     * On 64-bit architectures this space is doubled.
     * Do not touch!!!
     */
    APTR32            RP_Extra;
    ULONG             longreserved[6];
    UBYTE             reserved[2];
};

struct BitMapV0
{
    UWORD    BytesPerRow;
    UWORD    Rows;
    UBYTE    Flags;
    UBYTE    Depth;
    UWORD    pad;
    APTR32   Planes[8];
};

struct Layer_InfoV0
{
    APTR32 top_layer;
    APTR32 check_lp;
    APTR32 obs;
    APTR32 FreeClipRects;

    LONG PrivateReserve1;
    LONG PrivateReserve2;

    struct SignalSemaphoreV0 Lock;
    struct MinListV0         gs_Head;

    WORD   PrivateReserve3;
    APTR32 PrivateReserve4;

    UWORD  Flags;
    BYTE   fatten_count;
    BYTE   LockLayersCount;
    WORD   PrivateReserve5;
    APTR32 BlankHook;
    APTR32 LayerInfo_extra;
};

/* Text Attributes */
struct TextAttrV0
{
    APTR32 ta_Name;
    UWORD  ta_YSize;
    UBYTE  ta_Style;
    UBYTE  ta_Flags;
};

#endif
