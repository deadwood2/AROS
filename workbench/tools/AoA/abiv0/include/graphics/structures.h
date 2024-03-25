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

struct RegionV0
{
    struct Rectangle         bounds;
    APTR32                   RegionRectangle;
};

struct GfxBaseV0
{
    struct LibraryV0 LibNode;				/* Standard Library Node */

    APTR32          ActiView;				/* Currently displayed View */
    APTR32          copinit;				/* Initial copperlist */
    APTR32          cia;
    APTR32          blitter;
    APTR32          LOFlist;				/* Copperlists currently on display (pointers to raw instruction streams) */
    APTR32          SHFlist;
    APTR32          blthd;
    APTR32          blttl;
    APTR32          bsblthd;
    APTR32          bsblttl;
    struct InterruptV0 vbsrv;				/* VBLank IntServer */
    struct InterruptV0 timsrv;				/* Timer IntServer */
    struct InterruptV0 bltsrv;				/* Blitter IntServer */

/* Fonts */
    struct ListV0     TextFonts;
    APTR32            DefaultFont;			/* System default font */

    UWORD Modes;					/* Modes of current display (taken from ActiView->Modes) */
    BYTE  VBlank;
    BYTE  Debug;
    WORD  BeamSync;
    WORD  system_bplcon0;
    UBYTE SpriteReserved;
    UBYTE bytereserved;
    UWORD Flags;
    WORD  BlitLock;
    WORD  BlitNest;

    struct ListV0 BlitWaitQ;
    APTR32        BlitOwner;
    struct ListV0 TOF_WaitQ;

    UWORD                  DisplayFlags;  		/* see below */
    APTR32                 SimpleSprites;

    UWORD MaxDisplayRow;
    UWORD MaxDisplayColumn;
    UWORD NormalDisplayRows;
    UWORD NormalDisplayColumns;
    UWORD NormalDPMX;
    UWORD NormalDPMY;

    APTR32 LastChanceMemory;

    APTR32  LCMptr;
    UWORD   MicrosPerLine;
    UWORD   MinDisplayColumn;
    UBYTE   ChipRevBits0;     				/* see below */
    UBYTE   MemType;					/* CHIP memory type, see below */
    UBYTE   crb_reserved[4];
    UWORD   monitor_id;

    ULONG hedley[8];
    ULONG hedley_sprites[8];
    ULONG hedley_sprites1[8];
    WORD  hedley_count;
    UWORD hedley_flags;
    WORD  hedley_tmp;

    APTR32 hash_table;					/* Hashtable used for GfxAssociate() and GfxLookup() (private!) */
    UWORD  current_tot_rows;
    UWORD  current_tot_cclks;
    UBYTE  hedley_hint;
    UBYTE  hedley_hint2;
    ULONG  nreserved[4];
    APTR32 a2024_sync_raster;
    UWORD  control_delta_pal;
    UWORD  control_delta_ntsc;

    APTR32                  current_monitor;		/* MonitorSpec used for current display   */
    struct ListV0           MonitorList;		/* List of all MonitorSpecs in the system */
    APTR32                  default_monitor;		/* MonitorSpec of "default.monitor"	  */
    APTR32                  MonitorListSemaphore;	/* Semaphore for MonitorList access       */

    APTR32                  DisplayInfoDataBase;	/* NULL, unused by AROS			  */
    UWORD                   TopLine;
    APTR32                  ActiViewCprSemaphore;	/* Semaphore for active view access	  */

    APTR32                  UtilBase;				/* Library Bases */
    APTR32                  ExecBase;
};

#endif
