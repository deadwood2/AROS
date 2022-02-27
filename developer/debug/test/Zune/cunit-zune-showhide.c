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



CU_SUITE_SETUP()
{
    MUIMasterBase = OpenLibrary((STRPTR)MUIMASTER_NAME, 0);
    if (!MUIMasterBase)
        CUE_SINIT_FAILED;

    return CUE_SUCCESS;
}

CU_SUITE_TEARDOWN()
{
    CloseLibrary(MUIMasterBase);
    return CUE_SUCCESS;
}

CU_TEST_SETUP()
{
}

CU_TEST_TEARDOWN()
{
}

static void test_handleevent_hidden_object_dimensions_dont_change()
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
    CU_CI_DEFINE_SUITE("MUIA_ShowMe_Suite", __cu_suite_setup, __cu_suite_teardown, __cu_test_setup, __cu_test_teardown);
    CUNIT_CI_TEST(test_handleevent_hidden_object_dimensions_dont_change);
    return CU_CI_RUN_SUITES();
}
