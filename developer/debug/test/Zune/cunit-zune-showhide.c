/*
    Copyright (C) 2022, The AROS Development Team.
    All rights reserved.
*/


#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <clib/alib_protos.h>

#include <libraries/mui.h>

#include "../test-util.h"

#if defined(__AROS__)
#include <CUnit/CUnitCI.h>
#endif

struct Library *MUIMasterBase;

struct Data
{
    LONG dummy;
};

struct MUI_CustomClass *mcc_Button;

ULONG global_Setup;
ULONG global_Cleanup;
ULONG global_Show;
ULONG global_Hide;

static int globalReset()
{
    global_Setup    = 0;
    global_Cleanup  = 0;
    global_Show     = 0;
    global_Hide     = 0;
}

IPTR mOM_NEW(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct TagItem *tagsmore = msg->ops_AttrList;
    struct TagItem tags[] =
    {
        { MUIA_Font, MUIV_Font_Button } ,
        { MUIA_Text_PreParse, (IPTR)"\33c" } ,
        { MUIA_InputMode    , MUIV_InputMode_RelVerify },
        { MUIA_Background   , MUII_ButtonBack },
        { MUIA_CycleChain,    1 },
        { MUIA_Frame, MUIV_Frame_Button },
        { TAG_MORE, 0}
    };
    tags[6].ti_Data = (IPTR)tagsmore;
    msg->ops_AttrList = tags;

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

static IPTR mSetup(struct IClass *cl, Object *obj, Msg msg)
{
    if (!DoSuperMethodA(cl, obj, msg))
        return FALSE;

    global_Setup = 1;

    return TRUE;
}

static IPTR mCleanup(struct IClass *cl, Object *obj, Msg msg)
{
    global_Cleanup = 1;

    return DoSuperMethodA(cl, obj, msg);
}

static IPTR mShow(struct IClass *cl, Object *obj, Msg msg)
{
    global_Show = 1;

    return DoSuperMethodA(cl, obj, msg);
}

static IPTR mHide(struct IClass *cl, Object *obj, Msg msg)
{
    global_Hide = 1;

    return DoSuperMethodA(cl, obj, msg);
}

BOOPSI_DISPATCHER(IPTR, dispatcher, cl, obj, msg)
{
    switch (msg->MethodID)
    {
        case OM_NEW: return mOM_NEW(cl, obj, (struct opSet *)msg);
        case MUIM_Setup: return mSetup(cl, obj, (APTR)msg);
        case MUIM_Cleanup: return mCleanup(cl, obj, (APTR)msg);
        case MUIM_Show: return mShow(cl, obj, (APTR)msg);
        case MUIM_Hide: return mHide(cl, obj, (APTR)msg);
    }
    return DoSuperMethodA(cl, obj, msg);
}
BOOPSI_DISPATCHER_END


CU_SUITE_SETUP()
{
    MUIMasterBase = OpenLibrary((STRPTR)MUIMASTER_NAME, 0);
    if (!MUIMasterBase)
        CUE_SINIT_FAILED;

    mcc_Button = MUI_CreateCustomClass(NULL, MUIC_Text, NULL, sizeof(struct Data), dispatcher);

    return CUE_SUCCESS;
}

CU_SUITE_TEARDOWN()
{
    CloseLibrary(MUIMasterBase);
    return CUE_SUCCESS;
}

static void test_methods_called_during_showme()
{
    Object *wnd;
    Object *app;
    Object *btn1, *btn2;

    btn2 = NewObject(mcc_Button->mcc_Class, NULL,
            MUIA_Text_Contents, "BTN2",
            TAG_DONE);

    app = ApplicationObject,
        SubWindow, wnd = WindowObject,
            MUIA_Window_Activate, TRUE,
            WindowContents, HGroup,
                GroupFrame,
                Child, btn1 = MUI_MakeObject(MUIO_Button,(IPTR)"BTN1"),
                Child, btn2,
            End,
        End,
    End;


    if (app)
    {
        globalReset();
        set(wnd,MUIA_Window_Open, TRUE);

        CU_ASSERT_EQUAL(1, global_Setup);
        CU_ASSERT_EQUAL(1, global_Show);
        CU_ASSERT_EQUAL(0, global_Cleanup);
        CU_ASSERT_EQUAL(0, global_Hide);

        globalReset();
        set(btn2, MUIA_ShowMe, FALSE);
        CU_ASSERT_EQUAL(0, global_Setup);
        CU_ASSERT_EQUAL(0, global_Show);
        CU_ASSERT_EQUAL(0, global_Cleanup);
        CU_ASSERT_EQUAL(1, global_Hide);

        globalReset();
        set(btn2, MUIA_ShowMe, TRUE);
        CU_ASSERT_EQUAL(0, global_Setup);
        CU_ASSERT_EQUAL(1, global_Show);
        CU_ASSERT_EQUAL(0, global_Cleanup);
        /* CU_ASSERT_EQUAL(0, global_Hide); - incompatibility of Zune */

        globalReset();
        set(wnd,MUIA_Window_Open, FALSE);
        CU_ASSERT_EQUAL(0, global_Setup);
        CU_ASSERT_EQUAL(0, global_Show);
        CU_ASSERT_EQUAL(1, global_Cleanup);
        CU_ASSERT_EQUAL(1, global_Hide);

        MUI_DisposeObject(app);
    }
}

static void test_hidden_object_dimensions_dont_change()
{
    Object *wnd;
    Object *app;
    Object *btn1, *btn2;

    app = ApplicationObject,
        SubWindow, wnd = WindowObject,
            MUIA_Window_Activate, TRUE,
            WindowContents, HGroup,
                GroupFrame,
                Child, btn1 = MUI_MakeObject(MUIO_Button,(IPTR)"BTN1"),
                Child, btn2 = MUI_MakeObject(MUIO_Button,(IPTR)"BTN2"),
            End,
        End,
    End;

    if (app)
    {
        LONG p_width, i_width, i_mwidth;
        LONG p_height, i_height, i_mheight;
        LONG p_left, i_left, i_mleft;
        LONG p_right, i_right, i_mright;
        LONG p_top, i_top, i_mtop;
        LONG p_bottom, i_bottom, i_mbottom;

        set(wnd,MUIA_Window_Open, TRUE);

        p_width     = (LONG)XGET(btn2, MUIA_Width);
        i_width     = _width(btn2);
        i_mwidth    = _mwidth(btn2);

        p_height    = (LONG)XGET(btn2, MUIA_Height);
        i_height    = _height(btn2);
        i_mheight   = _mheight(btn2);

        p_left      = (LONG)XGET(btn2, MUIA_LeftEdge);
        i_left      = _left(btn2);
        i_mleft     = _mleft(btn2);

        p_right     = (LONG)XGET(btn2, MUIA_RightEdge);
        i_right     = _right(btn2);
        i_mright    = _mright(btn2);

        p_top       = (LONG)XGET(btn2, MUIA_TopEdge);
        i_top       = _top(btn2);
        i_mtop      = _mtop(btn2);

        p_bottom    = (LONG)XGET(btn2, MUIA_BottomEdge);
        i_bottom    = _bottom(btn2);
        i_mbottom   = _mbottom(btn2);

        set(btn2, MUIA_ShowMe, FALSE);

        CU_ASSERT_EQUAL(p_width, (LONG)XGET(btn2, MUIA_Width));

        CU_ASSERT_EQUAL(i_width, _width(btn2));
        CU_ASSERT_EQUAL(i_mwidth, _mwidth(btn2));

        CU_ASSERT_EQUAL(p_height, (LONG)XGET(btn2, MUIA_Height));
        CU_ASSERT_EQUAL(i_height, _height(btn2));
        CU_ASSERT_EQUAL(i_mheight, _mheight(btn2));

        CU_ASSERT_EQUAL(p_left, (LONG)XGET(btn2, MUIA_LeftEdge));
        CU_ASSERT_EQUAL(i_left, _left(btn2));
        CU_ASSERT_EQUAL(i_mleft, _mleft(btn2));

        CU_ASSERT_EQUAL(p_right, (LONG)XGET(btn2, MUIA_RightEdge));
        CU_ASSERT_EQUAL(i_right, _right(btn2));
        CU_ASSERT_EQUAL(i_mright, _mright(btn2));

        CU_ASSERT_EQUAL(p_top, (LONG)XGET(btn2, MUIA_TopEdge));
        CU_ASSERT_EQUAL(i_top, _top(btn2));
        CU_ASSERT_EQUAL(i_mtop, _mtop(btn2));

        CU_ASSERT_EQUAL(p_bottom, (LONG)XGET(btn2, MUIA_BottomEdge));
        CU_ASSERT_EQUAL(i_bottom, _bottom(btn2));
        CU_ASSERT_EQUAL(i_mbottom, _mbottom(btn2));

        set(wnd,MUIA_Window_Open, FALSE);

        MUI_DisposeObject(app);
    }
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(MUIA_ShowMe_Suite, __cu_suite_setup, __cu_suite_teardown, NULL, NULL);
    CUNIT_CI_TEST(test_hidden_object_dimensions_dont_change);
    CUNIT_CI_TEST(test_methods_called_during_showme);
    return CU_CI_RUN_SUITES();
}
