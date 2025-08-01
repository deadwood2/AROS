/*
    Copyright (C) 1995-2025, The AROS Development Team. All rights reserved.
*/

/*********************************************************************************************/

#include <proto/icon.h>
#include <exec/rawfmt.h>

#include "global.h"
#include "compilerspecific.h"
//#define DEBUG 1
#include "debug.h"
#include "arossupport.h"
#include "catalogs/catalog_version.h"

#include <setjmp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern struct NewMenu nm[];
extern struct NewMenu nmpict[];
extern struct NewMenu nmtext[];

/*********************************************************************************************/

/* Many datatype classes seem to rely on OM_NOTIFY calls coming back to the datatype object
   as OM_UPDATE :-\ */
   
#define BACK_CONNECTION 1

/*********************************************************************************************/

#define ARG_TEMPLATE    "FILE,CLIPBOARD/S,CLIPUNIT/K/N,SCREEN/S,PUBSCREEN/K,REQUESTER/S," \
                        "BOOKMARK/S,FONTNAME/K,FONTSIZE/K/N,CHARSET/K,BACKDROP/S,WINDOW/S," \
                        "PORTNAME/K,IMMEDIATE/S,REPEAT/S,PRTUNIT/K/N," \
                        "WINDOWLEFT/K/N,WINDOWTOP/K/N,WINDOWWIDTH/K/N,WINDOWHEIGHT/K/N," \
                        "AUTORESIZE/S"

#define ARG_FILE         0
#define ARG_CLIPBOARD    1
#define ARG_CLIPUNIT     2
#define ARG_SCREEN       3
#define ARG_PUBSCREEN    4
#define ARG_REQUESTER    5
#define ARG_BOOKMARK     6
#define ARG_FONTNAME     7
#define ARG_FONTSIZE     8
#define ARG_CHARSET      9
#define ARG_BACKDROP     10
#define ARG_WINDOW       11
#define ARG_PORTNAME     12
#define ARG_IMMEDIATE    13
#define ARG_REPEAT       14
#define ARG_PRTUNIT      15
#define ARG_WINDOWLEFT   16
#define ARG_WINDOWTOP    17
#define ARG_WINDOWWIDTH  18
#define ARG_WINDOWHEIGHT 19
#define ARG_AUTORESIZE   20

#define NUM_ARGS        21

/*********************************************************************************************/

static struct libinfo
{
    APTR        var;
    STRPTR      name;
    WORD        version;
} libtable[] =
{
    {&IntuitionBase     , "intuition.library"           , 39    },
    {&GfxBase           , "graphics.library"            , 39    },
    {&GadToolsBase      , "gadtools.library"            , 39    },
    {&LayersBase        , "layers.library"              , 39    },
    {&UtilityBase       , "utility.library"             , 39    },
    {&KeymapBase        , "keymap.library"              , 39    },
    {&DataTypesBase     , "datatypes.library"           , 39    },
    {&DiskfontBase      , "diskfont.library"            , 39    },
    {NULL                                                       }
};

static struct TextAttr  textattr;
static struct TextFont  *font;
static struct RDArgs    *myargs;
static IPTR             args[NUM_ARGS];
static UBYTE            fontname[256];
static WORD             winwidth, winheight;
static WORD             sizeimagewidth, sizeimageheight;
static BOOL             model_has_members;
static BOOL             FromWB;
static BOOL             bClipBoard = FALSE;
static BOOL             bRequester = TRUE;
static BOOL             bWindow = FALSE;
static APTR             clipunit = 0;
static STRPTR           pubScreen;
static jmp_buf exit_buf;

/*********************************************************************************************/

static void OpenLibs(void);
static void CloseLibs(void);
static void LoadFont(void);
static void InitDefaultFont(void);
static void KillFont(void);
static void GetArguments(void);
static void FreeArguments(void);
static void GetFileToolTypes(STRPTR fname);
static void KillICObjects(void);
static void GetVisual(void);
static void FreeVisual(void);
static void MakeGadgets(void);
static void KillGadgets(void);
static void CloseDTO(void);
static void KillWindow(void);
static void ScrollTo(UWORD dir, UWORD quali);
static void FitToWindow(void);

char *cmdexport = NULL;

/*********************************************************************************************/

void OutputMessage(CONST_STRPTR msg)
{
    struct EasyStruct es;
    
    D(bug("[MultiView] %s()\n", __func__));

    if (msg)
    {
        if ( IntuitionBase && !((struct Process *)FindTask(NULL))->pr_CLI )
        {
            es.es_StructSize   = sizeof(es);
            es.es_Flags        = 0;
            es.es_Title        = "MultiView";
            es.es_TextFormat   = msg;
            es.es_GadgetFormat = MSG(MSG_OK);

            EasyRequestArgs(win, &es, NULL, NULL);
        }
        else
        {
            Printf("[MultiView] %s\n", msg);
        }
    }
}

/*********************************************************************************************/

void WinCleanup(void)
{
    D(bug("[MultiView] %s()\n", __func__));

    if (win)
    {
        wincoords.MinX = win->LeftEdge;
        wincoords.MinY = win->TopEdge;
        wincoords.MaxX = win->Width;
        wincoords.MaxY = win->Height;
    }
    D(bug("[Multiview] WinCleanup() MinX = %d  MinY = %d  MaxX = %d  MaxY = %d\n",
        wincoords.MinX, wincoords.MinY, wincoords.MaxX, wincoords.MaxY));

    if (msgport)
    {
        DeleteMsgPort(msgport);
        msgport = NULL;
        D(bug("[Multiview] removed msgport\n"));
    }
    if (appwindow)
    {
        RemoveAppWindow(appwindow);
        appwindow = NULL;
        D(bug("[Multiview] removed appwindow\n"));
    }
    
    KillWindow();
    KillMenus();
    KillGadgets();
    FreeVisual();
    CloseDTO();
    KillICObjects();
    KillFont();
}

/*********************************************************************************************/

void Cleanup(CONST_STRPTR msg)
{
    struct ScreenNotifyMessage *snmsg;

    D(bug("[MultiView] %s()\n", __func__));

    OutputMessage(msg);

    while (!EndScreenNotify (isnstarted))
        Delay (10);
    
    if (isnport)
    {
        while ((snmsg = (struct ScreenNotifyMessage *) GetMsg (isnport)))
        {
            ReplyMsg((struct Message *)snmsg);
        }
        DeleteMsgPort(isnport);
    }

    WinCleanup();

    FreeArguments();

    if (cd != BNULL)
        CurrentDir(cd); /* restore current directory */

    CleanupLocale();
    CloseLibs();

    longjmp(exit_buf, 0);
}

/*********************************************************************************************/

static void OpenLibs(void)
{
    struct libinfo *li;

    D(bug("[MultiView] %s()\n", __func__));

    for(li = libtable; li->var; li++)
    {
        if (!((*(struct Library **)li->var) = OpenLibrary(li->name, li->version)))
        {
            __sprintf(s, MSG(MSG_CANT_OPEN_LIB), li->name, li->version);
            Cleanup(s);
        }
    }
       
}

/*********************************************************************************************/

static void CloseLibs(void)
{
    struct libinfo *li;

    D(bug("[MultiView] %s()\n", __func__));
    
    for(li = libtable; li->var; li++)
    {
        if (*(struct Library **)li->var) CloseLibrary((*(struct Library **)li->var));
    }
}

/*********************************************************************************************/

static void LoadFont(void)
{
    D(bug("[MultiView] %s()\n", __func__));

    font = OpenDiskFont(&textattr);

    if (!font)
    {
        InitDefaultFont();
        font = OpenDiskFont(&textattr);
    }

    if (!font)
    {
        textattr.ta_Name  = "topaz.font";
        textattr.ta_YSize = 8;
        textattr.ta_Style = 0;
        textattr.ta_Flags = 0;
        
        font = OpenFont(&textattr);
    }
}

/*********************************************************************************************/

static void KillFont(void)
{
    D(bug("[MultiView] %s()\n", __func__));

    if (font) CloseFont(font);
}

/*********************************************************************************************/

static void InitDefaultFont(void)
{
    struct TextFont *defaultfont = GfxBase->DefaultFont;

    D(bug("[MultiView] %s()\n", __func__));

    /* This might be a bit problematic depending on how default system font
       switching through Font prefs program works and if then the previous
       default system font is closed or not. So this is very likely only safe
       when in such a case the previous font is not closed (means -> the font
       will remain in memory in any case)

       ClipView example program on Amiga Dev CD also does it like this. So ... */

    textattr.ta_Name  = defaultfont->tf_Message.mn_Node.ln_Name;
    textattr.ta_YSize = defaultfont->tf_YSize;
    textattr.ta_Style = defaultfont->tf_Style;
    textattr.ta_Flags = defaultfont->tf_Flags;
}

/*********************************************************************************************/

static void GetArguments(void)
{
    D(bug("[MultiView] %s()\n", __func__));

    if (!filename)
        filename = (STRPTR)args[ARG_FILE];
    
    if (args[ARG_REQUESTER]) /* Open file requester if using file and have no filename supplied */
        bRequester = TRUE;
        
    if (args[ARG_CLIPBOARD]) /* Start with Clipboard data as input, as opposed to file */
    {
        bClipBoard = TRUE;
        bRequester = FALSE; /* Mutually Exclusive options */
        bWindow    = FALSE; /* Mutually Exclusive options */
    }

    if (args[ARG_WINDOW]) /* Start with empty Window with no input */
    {
        bWindow    = TRUE;
        bRequester = FALSE; /* Mutually Exclusive options */
        bClipBoard = FALSE; /* Mutually Exclusive options */
    }

    if (args[ARG_CLIPUNIT])
        clipunit = *(APTR *)args[ARG_CLIPUNIT];

    if (args[ARG_FONTNAME])
    {
        strncpy(fontname, (char *)args[ARG_FONTNAME], 255 - 5);
        if (!strstr(fontname, ".font")) strcat(fontname, ".font");

        textattr.ta_Name = fontname;
    }

    if (args[ARG_FONTSIZE])
        textattr.ta_YSize = *(LONG *)args[ARG_FONTSIZE];

    if (args[ARG_WINDOWLEFT])
        wincoords.MinX = *(LONG *)args[ARG_WINDOWLEFT];

    if (args[ARG_WINDOWTOP])
        wincoords.MinY = *(LONG *)args[ARG_WINDOWTOP];
    
    if (args[ARG_WINDOWWIDTH])
        wincoords.MaxX = *(LONG *)args[ARG_WINDOWWIDTH];

    if (args[ARG_WINDOWHEIGHT])
        wincoords.MaxY = *(LONG *)args[ARG_WINDOWHEIGHT];

    if (args[ARG_PUBSCREEN])
        pubScreen = (STRPTR)args[ARG_PUBSCREEN];
}

static void GetFileToolTypes(STRPTR fname)
{
    struct DiskObject *dobj;
    char **toolarray;
    char *s;

    D(bug("[MultiView] %s()\n", __func__));
    if (!fname)
        return;
    
    D(bug("[Multiview] Checking ToolTypes in filename = '%s.info'\n", fname));
    
    dobj=GetDiskObject(fname);
    if (!dobj)
    {
        char errtext[81];
        Fault(IoErr(), "GetDiskObject() Failed", errtext, 80);
    }
    else
    {
        /* We have read the DiskObject (icon) for this arg */
        toolarray = (char **)dobj->do_ToolTypes; 

        if (s = (char *)FindToolType((CONST STRPTR *)toolarray,"REQUESTER"))
            bRequester = TRUE;  

        if (s = (char *)FindToolType((CONST STRPTR *)toolarray,"CLIPBOARD"))
        {
            bClipBoard = TRUE;
            bRequester = FALSE; /* Mutually Exclusive options */
            bWindow    = FALSE; /* Mutually Exclusive options */
        }

        if (s = (char *)FindToolType((CONST STRPTR *)toolarray,"WINDOW"))
        {
            bWindow    = TRUE;
            bRequester = FALSE; /* Mutually Exclusive options */
            bClipBoard = FALSE; /* Mutually Exclusive options */
        }
        
        if (s = (char *)FindToolType((CONST STRPTR *)toolarray,"CLIPUNIT"))
            clipunit = (APTR)(IPTR)atoi(s);
        
        if (s = (char *)FindToolType((CONST STRPTR *)toolarray,"PUBSCREEN"))
            pubScreen = (STRPTR)strdup(s);

        if (s = (char *)FindToolType((CONST STRPTR *)toolarray,"FONTNAME"))
        {
            strncpy(fontname, s, 255 - 5);
            if (!strstr(fontname, ".font")) strcat(fontname, ".font");
            textattr.ta_Name = fontname;
        }
        if (s = (char *)FindToolType((CONST STRPTR *)toolarray,"FONTSIZE"))
            textattr.ta_YSize = (LONG)atoi(s);

        if (s = (char *)FindToolType((CONST STRPTR *)toolarray,"WINDOWLEFT"))
            wincoords.MinX = (LONG)atoi(s);

        if (s = (char *)FindToolType((CONST STRPTR *)toolarray,"WINDOWTOP"))
            wincoords.MinY = (LONG)atoi(s);

        if (s = (char *)FindToolType((CONST STRPTR *)toolarray,"WINDOWWIDTH"))
            wincoords.MaxX = (LONG)atoi(s);

        if (s = (char *)FindToolType((CONST STRPTR *)toolarray,"WINDOWHEIGHT"))
            wincoords.MaxY = (LONG)atoi(s);

        FreeDiskObject(dobj);
    }
}

/*********************************************************************************************/

static void FreeArguments(void)
{
    D(bug("[MultiView] %s()\n", __func__));
    if (myargs) FreeArgs(myargs);
}

/*********************************************************************************************/

static void MakeICObjects(void)
{
    static const struct TagItem dto_to_vert_map[] =
    {
        {DTA_TopVert            , PGA_Top       },
        {DTA_VisibleVert        , PGA_Visible   },
        {DTA_TotalVert          , PGA_Total     },
        {TAG_DONE                               }
    };
    static const struct TagItem dto_to_horiz_map[] =
    {
        {DTA_TopHoriz           , PGA_Top       },
        {DTA_VisibleHoriz       , PGA_Visible   },
        {DTA_TotalHoriz         , PGA_Total     },
        {TAG_DONE                               }
    };
    static const struct TagItem vert_to_dto_map[] =
    {
        {PGA_Top                , DTA_TopVert   },
        {TAG_DONE                               }
    };
    static const struct TagItem horiz_to_dto_map[] =
    {
        {PGA_Top                , DTA_TopHoriz  },
        {TAG_DONE                               }
    };

    D(bug("[MultiView] %s()\n", __func__));

    model_obj           = NewObject(NULL, MODELCLASS, ICA_TARGET, ICTARGET_IDCMP,
                                                      TAG_DONE);
    dto_to_vert_ic_obj  = NewObject(NULL, ICCLASS, ICA_MAP, (IPTR)dto_to_vert_map,
                                                   TAG_DONE);
    dto_to_horiz_ic_obj = NewObject(NULL, ICCLASS, ICA_MAP, (IPTR)dto_to_horiz_map,
                                                   TAG_DONE);
    vert_to_dto_ic_obj  = NewObject(NULL, ICCLASS, ICA_MAP, (IPTR)vert_to_dto_map,
                                                   TAG_DONE);
    horiz_to_dto_ic_obj = NewObject(NULL, ICCLASS, ICA_MAP, (IPTR)horiz_to_dto_map,
                                                   TAG_DONE);
#if BACK_CONNECTION
    model_to_dto_ic_obj = NewObject(NULL, ICCLASS, TAG_DONE);
#endif
    
    if (!model_obj              ||
        !dto_to_vert_ic_obj     ||
        !dto_to_horiz_ic_obj    ||
        !vert_to_dto_ic_obj     ||
        !horiz_to_dto_ic_obj 
    #if BACK_CONNECTION
        || !model_to_dto_ic_obj
    #endif
       )
    {
        Cleanup(MSG(MSG_CANT_CREATE_IC));
    }

    DoMethod(model_obj, OM_ADDMEMBER, (IPTR) dto_to_vert_ic_obj);
    DoMethod(model_obj, OM_ADDMEMBER, (IPTR) dto_to_horiz_ic_obj);
#if BACK_CONNECTION
    DoMethod(model_obj, OM_ADDMEMBER, (IPTR) model_to_dto_ic_obj);
#endif
    
    model_has_members = TRUE;

}

/*********************************************************************************************/

static void KillICObjects(void)
{
    D(bug("[MultiView] %s()\n", __func__));

    if (!model_has_members)
    {
        if (dto_to_vert_ic_obj) DisposeObject(dto_to_vert_ic_obj);
        if (dto_to_horiz_ic_obj) DisposeObject(dto_to_horiz_ic_obj);
    #if BACK_CONNECTION
        if (model_to_dto_ic_obj) DisposeObject(model_to_dto_ic_obj);
    #endif
    }

    if (model_obj) DisposeObject(model_obj);
    if (vert_to_dto_ic_obj) DisposeObject(vert_to_dto_ic_obj);
    if (horiz_to_dto_ic_obj) DisposeObject(horiz_to_dto_ic_obj);
}

/*********************************************************************************************/

static void GetVisual(void)
{
    D(bug("[MultiView] %s()\n", __func__));

    //scr = LockPubScreen((CONST_STRPTR)args[ARG_PUBSCREEN]);
    scr = LockPubScreen((CONST_STRPTR)pubScreen);
    if (!scr) Cleanup(MSG(MSG_CANT_LOCK_SCR));

    dri = GetScreenDrawInfo(scr);
    if (!dri) Cleanup(MSG(MSG_CANT_GET_DRI));

    vi = GetVisualInfoA(scr, NULL);
    if (!vi) Cleanup(MSG(MSG_CANT_GET_VI));
}

/*********************************************************************************************/

static void FreeVisual(void)
{
    D(bug("[MultiView] %s()\n", __func__));

    if (vi)  FreeVisualInfo(vi);
    if (dri) FreeScreenDrawInfo(scr, dri);
    if (scr) UnlockPubScreen(NULL, scr);
}

/*********************************************************************************************/

static void MakeGadgets(void)
{
    static WORD img2which[] =
    {
        UPIMAGE,
        DOWNIMAGE,
        LEFTIMAGE,
        RIGHTIMAGE,
        SIZEIMAGE
    };

    IPTR imagew[NUM_IMAGES], imageh[NUM_IMAGES];
    WORD v_offset, h_offset, btop, i;

    D(bug("[MultiView] %s()\n", __func__));

    for(i = 0; i < NUM_IMAGES; i++)
    {
        img[i] = NewObject(NULL, SYSICLASS, SYSIA_DrawInfo      , (IPTR)( dri ),
                                            SYSIA_Which         , (IPTR)( img2which[i] ),
                                            TAG_DONE);

        if (!img[i]) Cleanup(MSG(MSG_CANT_CREATE_SYSIMAGE));

        GetAttr(IA_Width,(Object *)img[i],&imagew[i]);
        GetAttr(IA_Height,(Object *)img[i],&imageh[i]);
    }

    sizeimagewidth  = imagew[IMG_SIZE];
    sizeimageheight = imageh[IMG_SIZE];

    btop = scr->WBorTop + dri->dri_Font->tf_YSize + 1;

    v_offset = imagew[IMG_DOWNARROW] / 4;
    h_offset = imageh[IMG_LEFTARROW] / 4;

    gad[GAD_UPARROW] = NewObject(NULL, BUTTONGCLASS,
            GA_Image            , (IPTR)( img[IMG_UPARROW] ),
            GA_RelRight         , (IPTR)( -imagew[IMG_UPARROW] + 1 ),
            GA_RelBottom        , (IPTR)( -imageh[IMG_DOWNARROW] - imageh[IMG_UPARROW] - imageh[IMG_SIZE] + 1 ),
            GA_ID               , (IPTR)( GAD_UPARROW ),
            GA_RightBorder      , (IPTR)TRUE,
            GA_Immediate        , (IPTR)TRUE,
            GA_RelVerify        , (IPTR)TRUE,
            TAG_DONE);

    gad[GAD_DOWNARROW] = NewObject(NULL, BUTTONGCLASS,
            GA_Image            , (IPTR)( img[IMG_DOWNARROW] ),
            GA_RelRight         , (IPTR)( -imagew[IMG_UPARROW] + 1 ),
            GA_RelBottom        , (IPTR)( -imageh[IMG_UPARROW] - imageh[IMG_SIZE] + 1 ),
            GA_ID               , (IPTR)( GAD_DOWNARROW ),
            GA_RightBorder      , (IPTR)TRUE,
            GA_Previous         , (IPTR)( gad[GAD_UPARROW] ),
            GA_Immediate        , (IPTR)TRUE,
            GA_RelVerify        , (IPTR)TRUE,
            TAG_DONE);

    gad[GAD_VERTSCROLL] = NewObject(NULL, PROPGCLASS,
            GA_Top              , (IPTR)( btop + 1 ),
            GA_RelRight         , (IPTR)( -imagew[IMG_DOWNARROW] + v_offset + 1 ),
            GA_Width            , (IPTR)( imagew[IMG_DOWNARROW] - v_offset * 2 ),
            GA_RelHeight        , (IPTR)( -imageh[IMG_DOWNARROW] - imageh[IMG_UPARROW] - imageh[IMG_SIZE] - btop -2 ),
            GA_ID               , (IPTR)( GAD_VERTSCROLL ),
            GA_Previous         , (IPTR)( gad[GAD_DOWNARROW] ),
            GA_RightBorder      , (IPTR)TRUE,
            GA_RelVerify        , (IPTR)TRUE,
            GA_Immediate        , (IPTR)TRUE,
            PGA_NewLook         , (IPTR)TRUE,
            PGA_Borderless      , (IPTR)TRUE,
            PGA_Total           , (IPTR)100,
            PGA_Visible         , (IPTR)100,
            PGA_Freedom         , (IPTR)FREEVERT,
            PGA_NotifyBehaviour , (IPTR)PG_BEHAVIOUR_NICE,
            TAG_DONE);

    gad[GAD_RIGHTARROW] = NewObject(NULL, BUTTONGCLASS,
            GA_Image            , (IPTR)( img[IMG_RIGHTARROW] ),
            GA_RelRight         , (IPTR)( -imagew[IMG_SIZE] - imagew[IMG_RIGHTARROW] + 1 ),
            GA_RelBottom        , (IPTR)( -imageh[IMG_RIGHTARROW] + 1 ),
            GA_ID               , (IPTR)( GAD_RIGHTARROW ),
            GA_BottomBorder     , (IPTR)TRUE,
            GA_Previous         , (IPTR)( gad[GAD_VERTSCROLL] ),
            GA_Immediate        , (IPTR)TRUE,
            GA_RelVerify        , (IPTR)TRUE,
            TAG_DONE);

    gad[GAD_LEFTARROW] = NewObject(NULL, BUTTONGCLASS,
            GA_Image            , (IPTR)( img[IMG_LEFTARROW] ),
            GA_RelRight         , (IPTR)( -imagew[IMG_SIZE] - imagew[IMG_RIGHTARROW] - imagew[IMG_LEFTARROW] + 1 ),
            GA_RelBottom        , (IPTR)( -imageh[IMG_RIGHTARROW] + 1 ),
            GA_ID               , (IPTR)( GAD_LEFTARROW ),
            GA_BottomBorder     , (IPTR)TRUE,
            GA_Previous         , (IPTR)( gad[GAD_RIGHTARROW] ),
            GA_Immediate        , (IPTR)TRUE,
            GA_RelVerify        , (IPTR)TRUE,
            TAG_DONE);

    gad[GAD_HORIZSCROLL] = NewObject(NULL, PROPGCLASS,
            GA_Left             , (IPTR)( scr->WBorLeft ),
            GA_RelBottom        , (IPTR)( -imageh[IMG_LEFTARROW] + h_offset + 1 ),
            GA_RelWidth         , (IPTR)( -imagew[IMG_LEFTARROW] - imagew[IMG_RIGHTARROW] - imagew[IMG_SIZE] - scr->WBorRight - 2 ),
            GA_Height           , (IPTR)( imageh[IMG_LEFTARROW] - (h_offset * 2) ),
            GA_ID               , (IPTR)( GAD_HORIZSCROLL ),
            GA_Previous         , (IPTR)( gad[GAD_LEFTARROW] ),
            GA_BottomBorder     , (IPTR)TRUE,
            GA_RelVerify        , (IPTR)TRUE,
            GA_Immediate        , (IPTR)TRUE,
            PGA_NewLook         , (IPTR)TRUE,
            PGA_Borderless      , (IPTR)TRUE,
            PGA_Total           , (IPTR)100,
            PGA_Visible         , (IPTR)100,
            PGA_Freedom         , (IPTR)FREEHORIZ,
            PGA_NotifyBehaviour , (IPTR)PG_BEHAVIOUR_NICE,
            TAG_DONE);

    for(i = 0;i < NUM_GADGETS;i++)
    {
        if (!gad[i]) Cleanup(MSG(MSG_CANT_CREATE_GADGET));
    }
    
    SetAttrs(gad[GAD_VERTSCROLL] , ICA_TARGET, (IPTR)vert_to_dto_ic_obj, TAG_DONE);
    SetAttrs(gad[GAD_HORIZSCROLL], ICA_TARGET, (IPTR)horiz_to_dto_ic_obj, TAG_DONE);
    SetAttrs(dto_to_vert_ic_obj  , ICA_TARGET, (IPTR)gad[GAD_VERTSCROLL], TAG_DONE);
    SetAttrs(dto_to_horiz_ic_obj , ICA_TARGET, (IPTR)gad[GAD_HORIZSCROLL], TAG_DONE);
}

/*********************************************************************************************/

static void KillGadgets(void)
{
    WORD i;

    D(bug("[MultiView] %s()\n", __func__));

    for(i = 0; i < NUM_GADGETS;i++)
    {
        if (win) RemoveGadget(win, (struct Gadget *)gad[i]);
        if (gad[i]) DisposeObject(gad[i]);
        gad[i] = 0;
    }
    
    for(i = 0; i < NUM_IMAGES;i++)
    {
        if (img[i]) DisposeObject(img[i]);
        img[i] = NULL;
    }
}

/*********************************************************************************************/

void AddDTOToWin(void)
{
    D(bug("[MultiView] %s()\n", __func__));

#if (0)
    /* add checkered background to see transparency in images */
    ULONG drawwidth = win->Width - 1 - win->BorderRight - win->BorderLeft;
    ULONG drawheight = win->Height - 1 - win->BorderBottom - win->BorderTop;
    ULONG drawoffsetx = win->BorderLeft;
    ULONG drawoffsety = win->BorderTop;
    WORD fillpen;
    UWORD x, y;
    UBYTE step = drawwidth / 16;
    if (step < 4) step = 4;

    for (x = 0; x < drawwidth; x += step)
    {
        for (y = 0; y < drawheight; y += step)
        {
            UBYTE fillw = step, fillh = step;
            if (x + fillw > drawwidth) fillw = drawwidth - x;
            if (y + fillh > drawheight) fillh = drawheight - y;

            if ((((x / step) + (y / step)) % 2) == 0)
                fillpen = 1;
            else
                fillpen = 2;

            SetAPen(win->RPort, fillpen);
            RectFill(win->RPort, drawoffsetx + x, drawoffsety + y, drawoffsetx + x + fillw, drawoffsety + y + fillh);
        }
    }
#else

    EraseRect(win->RPort, win->BorderLeft,
                          win->BorderTop,
                          win->Width - 1 - win->BorderRight,
                          win->Height - 1 - win->BorderBottom);
#endif

    SetDTAttrs (dto, NULL, NULL, GA_Left        , win->BorderLeft + 2                           ,
                                 GA_Top         , win->BorderTop + 2                            ,
                                 GA_RelWidth    , - win->BorderLeft - win->BorderRight - 4      ,
                                 GA_RelHeight   , - win->BorderTop - win->BorderBottom - 4      ,
                                 TAG_DONE);

    AddDTObject(win, NULL, dto, -1);
    RefreshDTObjects(dto, win, NULL, TAG_DONE); // seems to be needed by text datatype to render more than first line at start...
}

/*********************************************************************************************/

static void OpenDTO(void)
{
    struct DTMethod *triggermethods;
    ULONG           *methods;
    STRPTR          objname = NULL;
    IPTR            val;
    struct DataType *dt;

    D(bug("[MultiView] %s()\n", __func__));

    old_dto = dto;

    do
    {
        D(bug("[MultiView] calling NewDTObject\n"));

        if (!old_dto && bClipBoard)
        {
            dto = NewDTObject(clipunit, ICA_TARGET    , (IPTR)model_obj,
                                    GA_ID         , 1000           ,
                                    DTA_SourceType, DTST_CLIPBOARD ,
                                    DTA_TextAttr  , (IPTR)&textattr,
                                    TAG_DONE);

        }
        else
        {
             dto = NewDTObject(filename, ICA_TARGET      , (IPTR)model_obj,
                                    GA_ID           , 1000           ,
                                    DTA_TextAttr    , (IPTR)&textattr,
                                    TAG_DONE);
        }
        D(bug("[MultiView] NewDTObject returned %x\n", dto));

        if (!dto)
        {
            ULONG errnum = IoErr();

            if (errnum == DTERROR_UNKNOWN_DATATYPE)
            {
                BPTR lock = Lock(filename,ACCESS_READ);
                if (lock)
                {
                    struct DataType *dtn;
                    if ((dtn = ObtainDataTypeA(DTST_FILE, (APTR)lock, NULL)))
                    {
                        D(bug("[MultiView] %s ObtainDataTypeA returned %x\n", __func__, dtn));
                        if (!Stricmp(dtn->dtn_Header->dth_Name, "directory"))
                        {
                            /* file is a directory and no directory.datatype is installed */
                            strncpy(filenamebuffer, (filename ? filename : (STRPTR)""), 298);
                            filenamebuffer[298]=0;

                            if (strlen(filenamebuffer) &&
                                filenamebuffer[strlen(filenamebuffer)-1] != ':' &&
                                filenamebuffer[strlen(filenamebuffer)-1] != '/')
                            {
                                strcat(filenamebuffer,"/");
                            }

                            filename = GetFileName(MSG_ASL_OPEN_TITLE);
                            if (filename)
                            {
                                D(bug("[MultiView] %s calling ReleaseDataType(%x)\n", __func__, dtn));
                                ReleaseDataType(dtn);
                                UnLock(lock);
                                continue;
                            }
                        }
                        D(bug("[MultiView] %s calling ReleaseDataType(%x)\n", __func__, dtn));
                        ReleaseDataType(dtn);
                    }
                    UnLock(lock);
		        }
            }

            if (errnum >= DTERROR_UNKNOWN_DATATYPE)
                __sprintf(s, GetDTString(errnum), filename);
            else
                Fault(errnum, 0, s, 256);

            if (!old_dto)
            {
                /* Check if file is 0-length, fail silently (AOS confirmed) */
                STRPTR msg = s;
                BPTR lock = Lock(filename, ACCESS_READ);
                if (lock)
                {
                    struct FileInfoBlock * fib = AllocDosObject(DOS_FIB, NULL);
                    if (Examine(lock, fib))
                    {
                        if (fib->fib_Size == 0)
                        {
                            /* prog_exitcode = 20; */ /* ABIV1 Multiview no longer exits in Cleanup */
                            msg = NULL;
                        }
                    }
                    FreeDosObject(DOS_FIB, fib);
                    UnLock(lock);
                }

                Cleanup(msg);
            }
            dto = old_dto;
            return;
        }
    } while (!dto);

    strncpy(filenamebuffer, (filename ? filename : (STRPTR)""), 299);

    SetAttrs(vert_to_dto_ic_obj, ICA_TARGET, (IPTR)dto, TAG_DONE);
    SetAttrs(horiz_to_dto_ic_obj, ICA_TARGET, (IPTR)dto, TAG_DONE);
#if BACK_CONNECTION
    SetAttrs(model_to_dto_ic_obj, ICA_TARGET, (IPTR)dto, TAG_DONE);
#endif

    val = 0;
    GetDTAttrs(dto, DTA_NominalHoriz, (IPTR)&val, TAG_DONE);
    pdt_origwidth = winwidth = (WORD)val;
    GetDTAttrs(dto, DTA_NominalVert , (IPTR)&val, TAG_DONE);
    pdt_origheight = winheight = (WORD)val;
    pdt_zoom = 1;

    /*
     *  Add 4 Pixels for border around DataType-Object
     *  See AddDTOToWin() for details
     */
    if(winwidth)
    {
     winwidth += 4;
    }

    if(winheight)
    {
     winheight += 4;
    }

    GetDTAttrs(dto, DTA_ObjName, (IPTR)&objname, TAG_DONE);
    strncpy(objnamebuffer, objname ? objname : filenamebuffer, 299);
    
    dt = NULL;
    dto_subclass_gid = 0;
    if (GetDTAttrs(dto, DTA_DataType, (IPTR)&dt, TAG_DONE))
    {
        if (dt)
        {
            dto_subclass_gid = dt->dtn_Header->dth_GroupID;
        }
    }

    dto_supports_write = FALSE;
    dto_supports_write_iff = FALSE;
    dto_supports_print = FALSE;
    dto_supports_copy = FALSE;
    dto_supports_selectall = FALSE;
    dto_supports_clearselected = FALSE;

    if (DoWriteMethod(NULL, DTWM_RAW)) dto_supports_write = TRUE;       /* probe raw saving */
    if ((methods = GetDTMethods(dto)))
    {
        if (FindMethod(methods, DTM_WRITE)) dto_supports_write_iff = TRUE;
        if (FindMethod(methods, DTM_PRINT)) dto_supports_print = TRUE;
        if (FindMethod(methods, DTM_COPY)) dto_supports_copy = TRUE;
        if (FindMethod(methods, DTM_SELECT)) dto_supports_selectall = TRUE;
        if (FindMethod(methods, DTM_CLEARSELECTED)) dto_supports_clearselected = TRUE;
    }

    dto_supports_activate_field =  FALSE;
    dto_supports_next_field =  FALSE;
    dto_supports_prev_field =  FALSE;
    dto_supports_retrace =  FALSE;
    dto_supports_browse_next =  FALSE;
    dto_supports_browse_prev =  FALSE;
    dto_supports_search =  FALSE;
    dto_supports_search_next =  FALSE;
    dto_supports_search_prev =  FALSE;
    
    if ((triggermethods = (struct DTMethod *)GetDTTriggerMethods(dto)))
    {
        if (FindTriggerMethod(triggermethods, NULL, STM_ACTIVATE_FIELD)) dto_supports_activate_field = TRUE;
        if (FindTriggerMethod(triggermethods, NULL, STM_NEXT_FIELD))     dto_supports_next_field     = TRUE;
        if (FindTriggerMethod(triggermethods, NULL, STM_PREV_FIELD))     dto_supports_prev_field     = TRUE;
        if (FindTriggerMethod(triggermethods, NULL, STM_RETRACE))        dto_supports_retrace        = TRUE;
        if (FindTriggerMethod(triggermethods, NULL, STM_BROWSE_NEXT))    dto_supports_browse_next    = TRUE;
        if (FindTriggerMethod(triggermethods, NULL, STM_BROWSE_PREV))    dto_supports_browse_prev    = TRUE;
        if (FindTriggerMethod(triggermethods, NULL, STM_SEARCH))         dto_supports_search         = TRUE;
        if (FindTriggerMethod(triggermethods, NULL, STM_SEARCH_NEXT))    dto_supports_search_next    = TRUE;
        if (FindTriggerMethod(triggermethods, NULL, STM_SEARCH_PREV))    dto_supports_search_prev    = TRUE;
    }

    D(bug("\nMultiview: Found Methods:%s%s%s%s%s%s\n",
        dto_supports_write ? " DTM_WRITE->RAW" : "",
        dto_supports_write_iff ? " DTM_WRITE->IFF" : "",
        dto_supports_print ? " DTM_PRINT" : "",
        dto_supports_copy ? " DTM_COPY" : "",
        dto_supports_selectall ? " DTM_SELECT" : "",
        dto_supports_clearselected ? " DTM_CLEARSELECTED" : ""));
    
    D(bug("[MultiView] Found Triggers:%s%s%s%s%s%s%s\n\n",
        dto_supports_activate_field ? " STM_ACTIVATE_FIELD" : "",
        dto_supports_next_field ? " STM_NEXT_FIELD" : "",
        dto_supports_prev_field ? " STM_PREV_FIELD" : "",
        dto_supports_retrace ? " STM_RETRACE" : "",
        dto_supports_browse_next ? " STM_BROWSE_NEXT" : "",
        dto_supports_browse_prev ? " STM_BROWSE_PREV" : "",
        dto_supports_search ? " STM_SEARCH" : "",
        dto_supports_search_next ? " STM_SEARCH_NEXT" : "",
        dto_supports_search_prev ? " STM_SEARCH_PREV" : ""));
    
    if (old_dto)
    {
        if (win) RemoveDTObject(win, old_dto);
        D(bug("[MultiView] %s: calling DisposeDTOObject(%x)\n", __func__, old_dto));
        DisposeDTObject(old_dto);

        if (win)
        {
            AddDTOToWin();
            SetWindowTitles(win, objnamebuffer, (UBYTE *)~0);
            SetMenuFlags();

            // adopt object to current settings
            if (dto_subclass_gid == GID_TEXT)
            {
                SetDTAttrs (dto, NULL, NULL,
                            TDTA_WordWrap, tdt_text_wordwrap,
                            TAG_DONE);
                DoLayout(TRUE);
            }
            else if (dto_subclass_gid == GID_PICTURE)
            {
                // zoom has been set to 1 above
                FitToWindow();
                D(bug("[MultiView] %s: FitToWindow returned\n", __func__));

                SetDTAttrs (dto, NULL, NULL,
                            PDTA_DestMode, (pdt_force_map) ? PMODE_V42 : PMODE_V43,
                            TAG_DONE);
                SetDTAttrs (dto, NULL, NULL,
                            PDTA_DitherQuality, pdt_pict_dither ? 4 : 0,
                            TAG_DONE);
                DoLayout(TRUE);
            }
        }
    }
    D(bug("[MultiView] %s: finished\n", __func__));
}

/*********************************************************************************************/

static void CloseDTO(void)
{
    D(bug("[MultiView] %s()\n", __func__));

    if (dto)
    {
        if (win) RemoveDTObject(win, dto);
        D(bug("[MultiView] %s: calling DisposeDTOObject(%x)\n", __func__, dto));
        DisposeDTObject(dto);
        dto = NULL;
    }
}

/*********************************************************************************************/

static void MakeWindow(void)
{
    WORD minwidth, minheight;

    D(bug("[MultiView] %s()\n", __func__));

    if (pdt_fit_win)
        winwidth = winheight = 0;

    if (wincoords.MinX == 0)
        wincoords.MinX = (- scr->LeftEdge);
    if (wincoords.MinY == 0)
        wincoords.MinY = ( (- scr->TopEdge) < (scr->BarHeight + 1) ) ? (scr->BarHeight + 1) : (- scr->TopEdge);
    if (wincoords.MaxX == 0)
        wincoords.MaxX = scr->ViewPort.DWidth;
    if (wincoords.MaxY == 0)
        wincoords.MaxY = scr->ViewPort.DHeight - scr->BarHeight - 1;
    
    minwidth  = ( (winwidth) && (winwidth < 50) ) ? winwidth : 50;
    minheight = ( (winheight) && (winheight < 50) ) ? winheight : 50;

    win = OpenWindowTags(0, WA_PubScreen        , (IPTR)scr             ,
                            WA_Title            , (IPTR)objnamebuffer   ,
                            WA_CloseGadget      , TRUE                  ,
                            WA_DepthGadget      , TRUE                  ,
                            WA_DragBar          , TRUE                  ,
                            WA_SizeGadget       , TRUE                  ,
                            WA_Activate         , TRUE                  ,
                            WA_SimpleRefresh    , TRUE                  ,
                            WA_NoCareRefresh    , TRUE                  ,
                            WA_NewLookMenus     , TRUE                  ,
                            WA_Left             , wincoords.MinX        ,
                            WA_Top              , wincoords.MinY        ,
               ( winwidth ? WA_InnerWidth
                          : WA_Width )          ,
               ( winwidth ?                       winwidth
                          :                       wincoords.MaxX )      ,
              ( winheight ? WA_InnerHeight
                          : WA_Height )         ,
              ( winheight ?                       winheight
                          :                       wincoords.MaxY )      ,
                            WA_AutoAdjust       , TRUE                  ,
                            WA_MinWidth         , minwidth              ,
                            WA_MinHeight        , minheight             ,
                            WA_MaxWidth         , 16383                 ,
                            WA_MaxHeight        , 16383                 ,
                            WA_Gadgets          , (IPTR)gad[GAD_UPARROW],
                            WA_IDCMP            , IDCMP_CLOSEWINDOW |
                                                  IDCMP_GADGETUP    |
                                                  IDCMP_GADGETDOWN  |
                                                  IDCMP_MOUSEMOVE   |
                                                  IDCMP_VANILLAKEY  |
                                                  IDCMP_RAWKEY      |
                                                  IDCMP_IDCMPUPDATE |
                                                  IDCMP_MENUPICK    |
                                                  IDCMP_NEWSIZE     |
                                                  IDCMP_INTUITICKS      ,
                            TAG_DONE);

    if (!win) Cleanup(MSG(MSG_CANT_CREATE_WIN));

    AddDTOToWin();
        
    SetMenuStrip(win, menus);
    
    winmask = 1L << win->UserPort->mp_SigBit;
    if (!(msgport = CreateMsgPort()))
    {
        Cleanup(MSG(MSG_CANT_CREATE_MSGPORT));
    }
    if (!(appwindow = AddAppWindow(0, 0, win, msgport, NULL)))
    {
        Cleanup(MSG(MSG_CANT_ADD_APPWINDOW));
    }
    msgmask = 1L << msgport->mp_SigBit;
}

/*********************************************************************************************/

static void KillWindow(void)
{
    D(bug("[MultiView] %s()\n", __func__));

    if (win)
    {
        if (dto) RemoveDTObject(win, dto);
        if (menus) ClearMenuStrip(win);
        CloseWindow(win);
        win = NULL;

        winwidth = winheight = 0;
    }
}

/*********************************************************************************************/

static void InitIScreenNotify(void)
{
    D(bug("[MultiView] %s()\n", __func__));

    if (!(isnport = CreateMsgPort()))
    {
        Cleanup(MSG(MSG_CANT_CREATE_MSGPORT));
    }
    if ( (isnstarted = StartScreenNotifyTags(SNA_Notify,   SNOTIFY_WAIT_REPLY     |
                                                           SNOTIFY_BEFORE_CLOSEWB |
                                                           SNOTIFY_AFTER_OPENWB,
                                             SNA_MsgPort,  isnport,
                                             SNA_Priority, 0,
                                             TAG_END                               )) )
    {
        isnmask = 1L << isnport->mp_SigBit;
    }
}

/************************************************************************************/
/* Handle Intuition's ScreenNotify signals                                          */

static void HandleIScreenNotify(void)
{
    struct ScreenNotifyMessage *isnmsg;

    D(bug("[MultiView] %s()\n", __func__));

    while ((isnmsg = (struct ScreenNotifyMessage *) GetMsg (isnport)))
    {
        IPTR isnmclass = isnmsg->snm_Class;
    
        switch (isnmclass)
        {
            case SNOTIFY_BEFORE_CLOSEWB:
                D(bug("[Multiview] received isn before close WB msg\n"));
                if (win)
                {
                    WinCleanup();
                }
                ReplyMsg ((struct Message *) isnmsg);
                break;
            case SNOTIFY_AFTER_OPENWB:
                ReplyMsg ((struct Message *) isnmsg);
                D(bug("[Multiview] received isn after open WB msg\n"));
                if (!win)
                {
                    InitWin();
                }
                break;
            default:
                ReplyMsg ((struct Message *) isnmsg);
                D(bug("[Multiview] received unexpected msg!\n"));
                break;
      }
    }
}

/*********************************************************************************************/

static void ScrollTo(UWORD dir, UWORD quali)
{
    IPTR val;
    LONG oldtop, top, total, visible, delta = 1;
    BOOL horiz;
    BOOL inc;

    D(bug("[MultiView] %s()\n", __func__));

#ifdef __AROS__
    switch(dir)
    {
        case RAWKEY_NM_WHEEL_UP:
            dir = CURSORUP;
            delta = 3;
            break;

        case RAWKEY_NM_WHEEL_DOWN:
            dir = CURSORDOWN;
            delta = 3;
            break;

        case RAWKEY_NM_WHEEL_LEFT:
            dir = CURSORLEFT;
            delta = 3;
            break;

        case RAWKEY_NM_WHEEL_RIGHT:
            dir = CURSORRIGHT;
            delta = 3;
            break;
    }
#endif

    if ((dir == CURSORUP) || (dir == CURSORDOWN))
    {
        horiz = FALSE;
        if (dir == CURSORUP) inc = FALSE; else inc = TRUE;

        GetDTAttrs(dto, DTA_TopVert, (IPTR)&val, TAG_DONE);
        top = (LONG)val;
        GetDTAttrs(dto, DTA_TotalVert, (IPTR)&val, TAG_DONE);
        total = (LONG)val;
        GetDTAttrs(dto, DTA_VisibleVert, (IPTR)&val, TAG_DONE);
        visible = (LONG)val;
    }
    else
    {
        horiz = TRUE;
        if (dir == CURSORLEFT) inc = FALSE; else inc = TRUE;

        GetDTAttrs(dto, DTA_TopHoriz, (IPTR)&val, TAG_DONE);
        top = (LONG)val;
        GetDTAttrs(dto, DTA_TotalHoriz, (IPTR)&val, TAG_DONE);
        total = (LONG)val;
        GetDTAttrs(dto, DTA_VisibleHoriz, (IPTR)&val, TAG_DONE);
        visible = (LONG)val;

    }

    oldtop = top;
    if (quali & (IEQUALIFIER_LALT | IEQUALIFIER_RALT | IEQUALIFIER_CONTROL))
    {
        if (inc) top = total; else top = 0;
    }
    else
    if (quali & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
    {
        if (inc) top += visible - 1; else top -= visible - 1;
    }
    else
    {
        if (inc) top += delta; else top -= delta;
    }

    if (top + visible > total) top = total - visible;
    if (top < 0) top = 0;

    if (top != oldtop)
    {
        struct Gadget *g;

        if (horiz)
        {
            g = (struct Gadget *)gad[GAD_HORIZSCROLL];
        }
        else
        {
            g = (struct Gadget *)gad[GAD_VERTSCROLL];
        }

        SetGadgetAttrs(g, win, NULL, PGA_Top, top,
                                     TAG_DONE);

#ifdef __MORPHOS__
        /* Looks like setting PGA_Top on Amiga does not cause OM_NOTIFIEs
           to be sent (to dto). Or something like that. */

        SetDTAttrs(dto, win, NULL, (horiz ? DTA_TopHoriz : DTA_TopVert), top, TAG_DONE);
#endif

    } /* if (top != oldtop) */

}

/*********************************************************************************************/

static void FitToWindow(void)
{
    D(bug("[MultiView] %s()\n", __func__));

    if( pdt_fit_win )
    {
        int x, y;
        
        x = win->Width - (win->BorderLeft + win->BorderRight + 4);
        y = win->Height - (win->BorderTop + win->BorderBottom + 4);
        D(bug("=> width %ld height %ld\n", x, y));
        DoScaleMethod(x, y, pdt_keep_aspect);
//      DoLayout(TRUE); seems to be done by intuition ?
    }
    D(bug("[MultiView] %s: done\n", __func__));
}

/*********************************************************************************************/

static void HandleAll(void)
{
    struct IntuiMessage *msg;
    struct TagItem *tstate, *tags;
    struct TagItem      *tag;
    struct MenuItem     *item;
    struct Gadget       *activearrowgad = NULL;
    WORD                arrowticker = 0, activearrowkind = 0;
    IPTR                tidata;
    UWORD               men;
    BOOL                quitme = FALSE;
    const STRPTR        not_supported = "Sorry, not supported yet\n";
    ULONG               sigs;

    D(bug("[MultiView] %s()\n", __func__));

    while (!quitme)
    {
//        if ( (sigs & winmask) || (sigs & msgmask) )
        TEXT                editorvarbuffer[300];
        struct AppMessage  *appmsg;

        sigs = Wait(msgmask | winmask | isnmask);

        if (sigs & isnmask)
        {
            HandleIScreenNotify();
        }

        while ( (msgport) && (appmsg = (struct AppMessage *) GetMsg(msgport)) )
        {
            if (appmsg->am_Type == AMTYPE_APPWINDOW)
            {
                if (appmsg->am_NumArgs >= 1)
                {
                    NameFromLock(appmsg->am_ArgList->wa_Lock, filenamebuffer, 299);
                    AddPart(filenamebuffer, appmsg->am_ArgList->wa_Name, 299);
                    filename = filenamebuffer;
                    D(bug("[Multiview] appwindow received message: filename = %s\n", filename));
                }
            }

            ReplyMsg ((struct Message *) appmsg);
            ActivateWindow(win);

            if (filename)
            {
                OpenDTO();
                FitToWindow();
            }

        } /* while ((appmsg = (struct AppMessage *) GetMsg(msgport))) */
        
        while( (win) && (msg = (struct IntuiMessage *)GetMsg(win->UserPort)) )
        {
//          D(if (msg->Class!=IDCMP_INTUITICKS) bug("  Msg Class %08lx\n", (long)msg->Class));
            switch (msg->Class)
            {
                case IDCMP_CLOSEWINDOW:
                    quitme = TRUE;
                    break;

                case IDCMP_VANILLAKEY:
                    D(bug("[Multiview] Vanillakey %d\n", (int)msg->Code));
                    switch(msg->Code)
                    {
                        case 27: /* ESC */
                            quitme = TRUE;
                            break;
                            
                        case 13: /* RETURN */
                            if (dto_supports_activate_field) DoTrigger(STM_ACTIVATE_FIELD);
                            else if (dto_supports_search) DoTrigger(STM_SEARCH);
                            RefreshDTObjects (dto, win, NULL, TAG_DONE);
                            break;
                            
                        case 9: /* TAB */
                            if (dto_supports_next_field) DoTrigger(STM_NEXT_FIELD);
                            else if (dto_supports_search_next) DoTrigger(STM_SEARCH_NEXT);
                            break;
                            
                        case 8: /* Backspace */
                            if (dto_supports_retrace) DoTrigger(STM_RETRACE);
                            else ScrollTo(CURSORUP, IEQUALIFIER_LSHIFT);
                            break;

                        case ' ':
                            ScrollTo(CURSORDOWN, IEQUALIFIER_LSHIFT);
                            break;

                        case '>':
                            if (dto_supports_browse_next) DoTrigger(STM_BROWSE_NEXT);
                            break;

                        case '<':
                            if (dto_supports_browse_prev) DoTrigger(STM_BROWSE_PREV);
                            break;

                    } /* switch(msg->Code) */
                    if (strchr(MSG(MSG_SHORTCUT_EDITOR), ToUpper(msg->Code)))
                    {
                        if ( (GetVar("editor", (STRPTR) editorvarbuffer, 299, GVF_GLOBAL_ONLY)) != -1L )
                        {
                            __sprintf(s, "Run QUIET \"%s\" \"%s\"", editorvarbuffer, filename );
                            D(bug("[Multiview] editor command: '%s'\n", s));
                            if (SystemTags(s, TAG_END))
                                DisplayBeep(NULL);
                        }
                    }
                    break;

                case IDCMP_RAWKEY:
                    switch(msg->Code)
                    {
                    #ifdef __AROS__
                        case RAWKEY_NM_WHEEL_UP:
                        case RAWKEY_NM_WHEEL_DOWN:
                        case RAWKEY_NM_WHEEL_LEFT:
                        case RAWKEY_NM_WHEEL_RIGHT:
                    #endif
                        case CURSORUP:
                        case CURSORDOWN:
                        case CURSORRIGHT:
                        case CURSORLEFT:
                            ScrollTo(msg->Code, msg->Qualifier);
                            break;
                        
                    #ifdef __AROS__
                        case RAWKEY_HOME: /* HOME */
                            ScrollTo(CURSORUP, IEQUALIFIER_LALT);
                            break;
                            
                        case RAWKEY_END: /* END */
                            ScrollTo(CURSORDOWN, IEQUALIFIER_LALT);
                            break;
                            
                        case RAWKEY_PAGEUP: /* PAGE UP */
                            ScrollTo(CURSORUP, IEQUALIFIER_LSHIFT);
                            break;
                            
                        case RAWKEY_PAGEDOWN: /* PAGE DOWN */
                            ScrollTo(CURSORDOWN, IEQUALIFIER_LSHIFT);
                            break;
                    #endif
                      
                        case 0x42: /* SHIFT TAB? */
                            if (msg->Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
                            {
                                if (dto_supports_prev_field) DoTrigger(STM_PREV_FIELD);
                                else if (dto_supports_search_prev) DoTrigger(STM_SEARCH_PREV);
                            }
                            break;
                            
                    } /* switch(msg->Code) */
                    break;
                
                case IDCMP_GADGETDOWN:
                    arrowticker = 3;
                    activearrowgad = (struct Gadget *)msg->IAddress;
                    switch(activearrowgad->GadgetID)
                    {
                        case GAD_UPARROW:
                            activearrowkind = CURSORUP;
                            ScrollTo(CURSORUP, 0);
                            break;

                        case GAD_DOWNARROW:
                            activearrowkind = CURSORDOWN;
                            ScrollTo(CURSORDOWN, 0);
                            break;
                            
                        case GAD_LEFTARROW:
                            activearrowkind = CURSORLEFT;
                            ScrollTo(CURSORLEFT, 0);
                            break;

                        case GAD_RIGHTARROW:
                            activearrowkind = CURSORRIGHT;
                            ScrollTo(CURSORRIGHT, 0);
                            break;
                            
                        default:
                            activearrowkind = 0;
                            break;
                            
                    }
                    break;
                
                case IDCMP_INTUITICKS:
                    if (activearrowkind)
                    {
                        if (arrowticker)
                        {
                            arrowticker--;
                        }
                        else if (activearrowgad->Flags & GFLG_SELECTED)
                        {
                            ScrollTo(activearrowkind, 0);
                        }
                    }
                    break;
                    
                case IDCMP_GADGETUP:
                    switch(((struct Gadget *)msg->IAddress)->GadgetID)
                    {
                        case GAD_UPARROW:
                        case GAD_DOWNARROW:
                        case GAD_LEFTARROW:
                        case GAD_RIGHTARROW:
                            activearrowkind = 0;
                            break;
                    }
                    break;
                        
                case IDCMP_MENUPICK:
                    men = msg->Code;            
//                  D(bug(" * MV: men %08lx\n", (long)men));
                    while(men != MENUNULL)
                    {
                        if ((item = ItemAddress(menus, men)))
                        {
//                          D(bug(" * MV: item %08lx  menus %08lx\n", (long)item, (long)menus));
                            switch((IPTR)GTMENUITEM_USERDATA(item))
                            {
                                case MSG_MEN_PROJECT_OPEN:
                                    filename = GetFileName(MSG_ASL_OPEN_TITLE);
                                    if (filename) OpenDTO();
                                    break;

                                case MSG_MEN_PROJECT_EXPORT:
                                    {
                                        UBYTE *autocon="CON:0/40/640/150/Export.../auto/close/wait";

                                        struct TagItem stags[5];
                                        char command[1024];
                                        char filepath[256];
                                        BPTR tmpLock;

                                        if ((tmpLock = Lock(filename, ACCESS_READ)) != BNULL)
                                        {
                                            NameFromLock(tmpLock, filepath, 256);
                                            UnLock(tmpLock);
                                        }
                                        else
                                        {
                                            NameFromLock(cd, filepath, 256);
                                            AddPart(filepath, filename, 256);
                                        }

                                        stags[0].ti_Data = (IPTR)filepath;
                                        RawDoFmt(cmdexport, (RAWARG)&stags[0].ti_Data, RAWFMTFUNC_STRING, command);
                                        if (tmpLock = Open(autocon, MODE_OLDFILE))
                                        {
                                            stags[0].ti_Tag = SYS_Input;
                                            stags[0].ti_Data = (IPTR)tmpLock;
                                            stags[1].ti_Tag = SYS_Output;
                                            stags[1].ti_Data = 0;
                                            stags[2].ti_Tag = SYS_Asynch;
                                            stags[2].ti_Data = TRUE;
                                            stags[3].ti_Tag = SYS_UserShell;
                                            stags[3].ti_Data = TRUE;
                                            stags[4].ti_Tag = TAG_END;

                                            System(command, stags);
                                        }
                                    }
                                    break;

                                case MSG_MEN_PROJECT_SAVEAS_IFF:
                                    filename = GetFileName(MSG_ASL_SAVE_TITLE);
                                    if (filename) DoWriteMethod(filename, DTWM_IFF);
                                    break;

                                case MSG_MEN_PROJECT_PRINT:
                                    DoPrintMethod();
                                    break;

                                case MSG_MEN_PROJECT_ABOUT:
                                    About();
                                    break;

                                case MSG_MEN_PROJECT_QUIT:
                                    quitme = TRUE;
                                    break;

                                case MSG_MEN_EDIT_MARK:
                                #if defined(__AROS__) && !defined(__MORPHOS__)
                                    if (StartDragSelect(dto))
                                #else
                                    {
                                        struct DTSpecialInfo *si;

                                        /*
                                        ** ClipView example on AmigaDev CD does just the following.
                                        ** None of the checks AROS datatypes.library/StartDragSelect()
                                        ** does.
                                        */
                                           
                                        si = (struct DTSpecialInfo *)(((struct Gadget *)dto)->SpecialInfo);
                                        si->si_Flags |= DTSIF_DRAGSELECT;
                                    }
                                #endif
                                    {
                                        //TODO: change mouse pointer to crosshair
                                    }
                                    break;

                                case MSG_MEN_EDIT_COPY:
                                    {
                                        struct dtGeneral dtg;
                                        
                                        dtg.MethodID = DTM_COPY;
                                        dtg.dtg_GInfo = NULL;
                                        
                                        DoDTMethodA(dto, win, NULL, (Msg)&dtg);
                                    }
                                    break;
                                
                                case MSG_MEN_EDIT_SELECTALL:
                                    OutputMessage(not_supported);
                                    break;
                                    
                                case MSG_MEN_EDIT_CLEARSELECTED:
                                    {
                                        struct dtGeneral dtg;

                                        dtg.MethodID = DTM_CLEARSELECTED;
                                        dtg.dtg_GInfo = NULL;
                                        
                                        DoDTMethodA(dto, win, NULL, (Msg)&dtg);
                                    }
                                    break;

                                case MSG_MEN_WINDOW_SEPSCREEN:
                                    OutputMessage(not_supported);
                                    break;

                                case MSG_MEN_WINDOW_MINIMIZE:
                                    OutputMessage(not_supported);
                                    break;

                                case MSG_MEN_WINDOW_NORMAL:
                                    OutputMessage(not_supported);
                                    break;

                                case MSG_MEN_WINDOW_MAXIMIZE:
                                    OutputMessage(not_supported);
                                    break;

                                case MSG_MEN_SETTINGS_SAVEDEF:
                                    OutputMessage(not_supported);
                                    break;
                                    
                                case MSG_MEN_PICT_ZOOM_IN:
                                    pdt_zoom++;
                                    if (pdt_zoom == -1 ) pdt_zoom = 1;
                                    DoZoom(pdt_zoom);
                                    break;

                                case MSG_MEN_PICT_ZOOM_OUT:
                                    pdt_zoom--;
                                    if (pdt_zoom == 0 ) pdt_zoom = -2;
                                    DoZoom(pdt_zoom);
                                    break;

                                case MSG_MEN_PICT_RESET:
                                    pdt_zoom = 1;
                                    DoZoom(pdt_zoom);
                                    break;

                                case MSG_MEN_PICT_FIT_WIN:
                                    pdt_fit_win = (item->Flags & CHECKED) ? TRUE : FALSE;
                                    FitToWindow();
                                    DoLayout(TRUE);
                                    break;

                                case MSG_MEN_PICT_KEEP_ASPECT:
                                    pdt_keep_aspect = (item->Flags & CHECKED) ? TRUE : FALSE;
                                    FitToWindow();
                                    DoLayout(TRUE);
                                    break;

                                case MSG_MEN_PICT_FORCE_MAP:
                                    pdt_force_map = (item->Flags & CHECKED) ? TRUE : FALSE;
                                    SetDTAttrs (dto, NULL, NULL,
                                                PDTA_DestMode, (pdt_force_map) ? PMODE_V42 : PMODE_V43,
                                                TAG_DONE);
                                    DoLayout(TRUE);
                                    break;

                                case MSG_MEN_PICT_DITHER:
                                    pdt_pict_dither = (item->Flags & CHECKED) ? TRUE : FALSE;
                                    SetDTAttrs (dto, NULL, NULL,
                                                PDTA_DitherQuality, pdt_pict_dither ? 4 : 0,
                                                TAG_DONE);
                                    DoLayout(TRUE);
                                    break;

                                case MSG_MEN_TEXT_WORDWRAP:
                                    tdt_text_wordwrap = (item->Flags & CHECKED) ? TRUE : FALSE;
                                    D(
                                        if (tdt_text_wordwrap)
                                            bug("wordwrap enabled\n");
                                        else
                                            bug("wordwrap disabled\n");
                                    )
                                    SetDTAttrs (dto, NULL, NULL,
                                                TDTA_WordWrap, tdt_text_wordwrap,
                                                TAG_DONE);
                                    DoLayout(TRUE);
                                    break;

                                case MSG_MEN_TEXT_SEARCH:
                                    if (dto_supports_search) DoTrigger(STM_SEARCH);
                                    break;

                                case MSG_MEN_TEXT_SEARCH_PREV:
                                    if (dto_supports_search_prev) DoTrigger(STM_SEARCH_PREV);
                                    break;

                                case MSG_MEN_TEXT_SEARCH_NEXT:
                                    if (dto_supports_search_next) DoTrigger(STM_SEARCH_NEXT);
                                    break;

                            } /* switch(GTMENUITEM_USERDATA(item)) */
                            
                            men = item->NextSelect;
                        }
                        else
                        {
                            men = MENUNULL;
                        }
                        
                    } /* while(men != MENUNULL) */
                    break;
                    
                case IDCMP_NEWSIZE:
                    D(bug("IDCMP NEWSIZE\n"));
                    FitToWindow();
                    break;

                case IDCMP_IDCMPUPDATE:
                    tstate = tags = (struct TagItem *) msg->IAddress;
                    while ((tag = NextTagItem(&tstate)) != NULL)
                    {
                        tidata = tag->ti_Data;
//                      D(bug("IDCMP UPDATE %08lx %08lx\n", (long)tag->ti_Tag, (long)tag->ti_Data));
                        switch (tag->ti_Tag)
                        {
                            /* Change in busy state */
                            case DTA_Busy:
                                if (tidata)
                                    SetWindowPointer (win, WA_BusyPointer, TRUE, TAG_DONE);
                                else
                                    SetWindowPointer (win, WA_Pointer, (IPTR) NULL, TAG_DONE);
                                break;

                            case DTA_Title:
                                SetWindowTitles(win, (UBYTE *)tidata, (UBYTE *)~0);
                                break;

                            /* Error message */
                            case DTA_ErrorLevel:
/*                              if (tidata)
                                {
                                    errnum = GetTagData (DTA_ErrorNumber, NULL, tags);
                                    PrintErrorMsg (errnum, (STRPTR) options[OPT_NAME]);
                                }*/
                                break;

                            /* Time to refresh */
                            case DTA_Sync:
                                /* Refresh the DataType object,
                                   unless we wait for initial rendering */
                                D(bug("[MultiView] DTA_SYNC\n"));
                                RefreshDTObjects (dto, win, NULL, TAG_DONE);
                                break;

                        } /* switch (tag->ti_Tag) */

                    } /* while ((tag = NextTagItem ((const struct TagItem **)&tstate))) */
                    break;

            } /* switch (msg->Class) */

            ReplyMsg((struct Message *)msg);

        } /* while((msg = (struct IntuiMessage *)GetMsg(win->UserPort))) */

    } /* while (!quitme) */
}

/*********************************************************************************************/

void InitWin(void)
{
    D(bug("[MultiView] %s()\n", __func__));

    LoadFont();
    MakeICObjects();
    OpenDTO();
    GetVisual();
    MakeGadgets();
    menus = MakeMenus(nm);
    pictmenus = MakeMenus(nmpict);
    textmenus = MakeMenus(nmtext);
    SetMenuFlags();
    MakeWindow();
    SetDTAttrs (dto, NULL, NULL,
                PDTA_DestMode, (pdt_force_map) ? PMODE_V42 : PMODE_V43,
                PDTA_DitherQuality, (pdt_pict_dither) ? 4 : 0,
                TDTA_WordWrap, tdt_text_wordwrap,
                TAG_DONE);
    FitToWindow();
}

/*********************************************************************************************/

int main(int argc, char **argv)
{
    struct WBStartup *WBenchMsg;
    struct WBArg *wbarg;
    int rc;

    D(bug("[MultiView] %s()\n", __func__));

    BOOL FromWb = (argc == 0 ) ? TRUE : FALSE;
    filename = NULL;

    /* This is for when Cleanup() is called */
    rc = setjmp(exit_buf);
    if (rc)
        return rc;

    wincoords.MinX = 0;
    wincoords.MinY = 0;
    wincoords.MaxX = 0;
    wincoords.MaxY = 0;
    
    pdt_fit_win       = FALSE;
    pdt_keep_aspect   = FALSE;
    pdt_force_map     = FALSE;
    pdt_pict_dither   = TRUE;
    tdt_text_wordwrap = TRUE;
    separate_screen   = FALSE;

    OpenLibs();
    InitDefaultFont(); /* May be overridden in GetArguments() if user specifies a font */
    InitLocale("System/Utilities/MultiView.catalog", 2);
    InitMenus(nm);
    InitMenus(nmpict);
    InitMenus(nmtext);

    if (FromWb)
    {
        WBenchMsg = (struct WBStartup *) argv;
        char filepath[256];

        if ( WBenchMsg->sm_NumArgs > 0)
        {
            wbarg = WBenchMsg->sm_ArgList;
            /* Look at icon for Multiview program */
            if (wbarg->wa_Lock)
            {
                NameFromLock(wbarg->wa_Lock, filepath, 256);
                AddPart(filepath, wbarg->wa_Name, 256);
                GetFileToolTypes(filepath); /*defaults from Multiview Icon*/
            }
            else
                D(bug("[MultiView] No wa_lock exits for program = %s\n", wbarg->wa_Name));
        }
        if ( WBenchMsg->sm_NumArgs > 1)
        {
            wbarg++; /* skip to input file */
            D(bug("[MultiView] filename = %s\n", wbarg->wa_Name));
            if (wbarg->wa_Lock)
            {   
                NameFromLock(wbarg->wa_Lock, filepath, 256);
                AddPart(filepath, wbarg->wa_Name, 256);
                filename = filepath;
                D(bug("[MultiView] Fully qualified filename = %s\n", filename));
            }
            else
                D(bug("[MultiView] No lock exits for filename = %s\n", wbarg->wa_Name));
        }
    }
    else /* from CLI */
    {
        if (!(myargs = ReadArgs(ARG_TEMPLATE, args, NULL)))
        {
            Fault(IoErr(), 0, s, 256);
            Cleanup(s);
        }
        GetArguments();
    }

    /* if running with no filename, then maybe bring up a requester */
    if (!filename && bRequester && !bWindow && !bClipBoard)
        filename = GetFileName(MSG_ASL_OPEN_TITLE);

    /* did we select a file ? If so, check to see if it has an icon to get options from */
    if (filename) 
        GetFileToolTypes(filename);

    /* ensure CLI parms (other than filename) beat icon parms */
    if (!FromWb)
        GetArguments();

    if (!filename &&!bWindow && !bClipBoard)
        Cleanup(NULL);

    InitIScreenNotify();
    InitWin();

    HandleAll();
    Cleanup(NULL);
    
    return 0;
}
