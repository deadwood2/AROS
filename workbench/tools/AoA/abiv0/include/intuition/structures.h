#ifndef ABIV0_INTUITION_STRUCTURES_H
#define ABIV0_INTUITION_STRUCTURES_H

#include "../exec/structures.h"
#include "../graphics/structures.h"

struct ScreenV0
{
    APTR32 NextScreen;
    APTR32 FirstWindow;

    WORD LeftEdge;
    WORD TopEdge;
    WORD Width;
    WORD Height;

    WORD MouseX; /* ABI_V0 compatibility */
    WORD MouseY; /* ABI_V0 compatibility */

    UWORD   Flags;
    APTR32 Title;
    APTR32 DefaultTitle;

    BYTE BarHeight;
    BYTE BarVBorder;
    BYTE BarHBorder;
    BYTE MenuVBorder;
    BYTE MenuHBorder;
    BYTE WBorTop;
    BYTE WBorLeft;
    BYTE WBorRight;
    BYTE WBorBottom;

    APTR32 Font;

    struct ViewPortV0 ViewPort;
    struct RastPortV0 RastPort;
    struct BitMapV0   BitMap_OBSOLETE;    /* OBSOLETE */
    struct Layer_InfoV0 LayerInfo;

    APTR32 FirstGadget;

    UBYTE DetailPen;
    UBYTE BlockPen;

    UWORD          SaveColor0;
    APTR32         BarLayer;
    APTR32         ExtData;
    APTR32         UserData;
};

struct DrawInfoV0
{
    UWORD             dri_Version; /* see below */
    UWORD             dri_NumPens;
    APTR32            dri_Pens;    /* see below */
    APTR32            dri_Font;
    UWORD             dri_Depth;

    struct
    {
        UWORD X;
        UWORD Y;
    } dri_Resolution;

    ULONG dri_Flags; /* see below */

    /* The following fields are present if dri_Version >= 2 */
    APTR32         dri_CheckMark;
    APTR32         dri_AmigaKey;  

    /*
     * The following fields are compatible with AmigaOS v4.
     * Present if dri_Version >= 3.
     */
    APTR32         dri_Screen;
    APTR32         dri_Prefs;

    ULONG          dri_Reserved[3];
};

#endif
