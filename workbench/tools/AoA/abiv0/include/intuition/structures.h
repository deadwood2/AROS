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

struct WindowV0
{
    APTR32 NextWindow;

    WORD LeftEdge;
    WORD TopEdge;
    WORD Width;
    WORD Height;

    WORD MouseX; /* ABI_V0 compatibility */
    WORD MouseY; /* ABI_V0 compatibility */

    WORD MinWidth;
    WORD MinHeight;
    UWORD MaxWidth;
    UWORD MaxHeight;

    ULONG Flags;

    APTR32 MenuStrip;
    APTR32 Title;
    APTR32 FirstRequest;
    APTR32 DMRequest;

    WORD ReqCount;

    APTR32 WScreen;
    APTR32 RPort;

    BYTE	      BorderLeft;
    BYTE	      BorderTop;
    BYTE	      BorderRight;
    BYTE	      BorderBottom;
    APTR32 BorderRPort;

    APTR32 FirstGadget;
    APTR32 Parent;
    APTR32 Descendant;

    APTR32 Pointer;
    BYTE    PtrHeight;
    BYTE    PtrWidth;
    BYTE    XOffset;
    BYTE    YOffset;

    ULONG		  IDCMPFlags;
    APTR32 UserPort;
    APTR32 WindowPort;
    APTR32 MessageKey;

    UBYTE	   DetailPen;
    UBYTE	   BlockPen;
    APTR32 CheckMark;
    APTR32 ScreenTitle;

    WORD GZZMouseX;
    WORD GZZMouseY;
    WORD GZZWidth;
    WORD GZZHeight;

    APTR32 ExtData;
    APTR32 UserData;

    APTR32 WLayer;
    APTR32 IFont;

    ULONG MoreFlags;
    
    WORD RelLeftEdge; // relative coordinates of the window
    WORD RelTopEdge;  // to its parent window. If it is 
                      // a window on the screen then these
                      // are the same as LeftEdge and TopEdge.
    
    APTR32 firstchild;  // pointer to first child
    APTR32 prevchild;   // if window is a child of a window
    APTR32 nextchild;   // then they are concatenated here.
    APTR32 parent;      // parent of this window
};

			   /***** Images *****/

struct ImageV0
{
    WORD LeftEdge;
    WORD TopEdge;
    WORD Width;
    WORD Height;

    WORD    Depth;
    APTR32 ImageData;
    UBYTE   PlanePick;
    UBYTE   PlaneOnOff;

    APTR32 NextImage;
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

		       /***** Intuition Message *****/

struct IntuiMessageV0
{
    struct MessageV0 ExecMessage;

    ULONG Class;
    UWORD Code;
    UWORD Qualifier;
    APTR32  IAddress;

    WORD  MouseX;
    WORD  MouseY;
    ULONG Seconds;
    ULONG Micros;

    APTR32 IDCMPWindow;
    APTR32 SpecialLink;

    /* IntIntuiMessage */
    APTR32 prevCodeQuals;
};

struct GadgetInfoV0
{
    APTR32              gi_Screen;
    APTR32              gi_Window;
    APTR32              gi_Requester;
    APTR32              gi_RastPort;
    APTR32              gi_Layer;
    struct IBox         gi_Domain;

    struct
    {
        UBYTE DetailPen;
        UBYTE BlockPen;
    } gi_Pens;

    APTR32              gi_DrInfo;

    ULONG gi_Reserved[6];
};

struct NewWindowV0
{
    WORD LeftEdge;
    WORD TopEdge;
    WORD Width;
    WORD Height;

    UBYTE DetailPen;
    UBYTE BlockPen;

    ULONG IDCMPFlags;
    ULONG Flags;

    APTR32 FirstGadget;
    APTR32 CheckMark;
    APTR32 Title;
    APTR32 Screen;    /* ignored if Type != CUSTOMSCREEN */
    APTR32 BitMap;

    WORD  MinWidth;
    WORD  MinHeight;
    UWORD MaxWidth;
    UWORD MaxHeight;

    UWORD Type;
};

			   /***** Gadgets *****/

struct GadgetV0
{
    APTR32 NextGadget;

    WORD LeftEdge;
    WORD TopEdge;
    WORD Width;
    WORD Height;

    UWORD Flags;      /* see below */
    UWORD Activation; /* see below */
    UWORD GadgetType; /* see below */

    APTR32       GadgetRender;
    APTR32       SelectRender;
    APTR32       GadgetText;

    ULONG MutualExclude; /* OBSOLETE */

    APTR32  SpecialInfo;
    UWORD GadgetID;
    APTR32  UserData;
};

#define STACKEDV0 __attribute__((aligned(4)))

struct opGetV0
{
    STACKEDV0 ULONG   MethodID;
    STACKEDV0 Tag     opg_AttrID;
    STACKEDV0 APTR32  opg_Storage;
};

struct gpRenderV0
{
    STACKEDV0 ULONG   MethodID;   /* GM_RENDER */
    STACKEDV0 APTR32  gpr_GInfo;  /* see <intuition/cghooks.h> */
    STACKEDV0 APTR32  gpr_RPort;  /* RastPort (see <graphics/rastport.h>) to
                                       render into. */
    STACKEDV0 LONG		gpr_Redraw; /* see below */
};

struct gpLayoutV0
{
    STACKEDV0 ULONG		 MethodID;    /* GM_LAYOUT */
    STACKEDV0 APTR32 gpl_GInfo;   /* see <intuition/cghooks.h> */
      /* Boolean that indicated, if this method was invoked, when you are added
         to a window (TRUE) or if it is called, because the window was resized
         (FALSE). */
    STACKEDV0 ULONG		 gpl_Initial;
};

struct gpHitTestV0
{
    STACKEDV0 ULONG		MethodID;   /* GM_HITEST or GM_HELPTEST */
    STACKEDV0 APTR32 gpht_GInfo; /* see <intuition/cghooks.h> */

      /* These values are relative to the gadget select box for GM_HITTEST. For
         GM_HELPTEST they are relative to the bounding box (which is often
         equal to the select box). */
    STACKEDV0 struct
    {
	STACKEDV0 WORD X;
	STACKEDV0 WORD Y;
    }			gpht_Mouse;
};

struct gpInputV0
{
    STACKEDV0 ULONG		 MethodID;        /* GM_GOACTIVE or GM_HANDLEINPUT */
    STACKEDV0 APTR32 gpi_GInfo;       /* see <intuition/cghooks.h> */
      /* Pointer to the InputEvent (see <devices/inputevent.h>) that caused
         the method to be invoked. */
    STACKEDV0 APTR32 gpi_IEvent;
      /* Pointer to a variable that is to be set by the gadget class, if
         GMR_VERIFY is returned. The lower 16 bits of this value are returned
         in the Code field of the IntuiMessage (see <intuition/intuition.h>)
         passed back to the application. */
    STACKEDV0 APTR32 gpi_Termination;

      /* This struct defines the current mouse position, relative to the
         gadgets' bounding box. */
    STACKEDV0 struct
    {
	STACKEDV0 WORD X;
	STACKEDV0 WORD Y;
    }			gpi_Mouse;
      /* Pointer to TabletData structure (see <intuition/intuition.h>) or NULL,
         if this input event did not originate from a tablet that is capable of
         sending IESUBCLASS_NEWTABLET events. */
    STACKEDV0 APTR32 gpi_TabletData;
};

struct gpGoInactiveV0
{
    STACKEDV0 ULONG		MethodID;   /* GM_GOINACTIVE */
    STACKEDV0 APTR32 gpgi_GInfo; /* see <intuition/cghooks.h> */
      /* Boolean field to indicate, who wanted the gadget to go inactive. If
         this is 1 this method was sent, because intution wants the gadget to
         go inactive, if it is 0, it was the gadget itself that wanted it. */
    STACKEDV0 ULONG		gpgi_Abort;
};

#endif
