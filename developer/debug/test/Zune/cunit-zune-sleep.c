/*
    Copyright © 2022, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <libraries/mui.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/exec.h>
#include <proto/dos.h>

#include <CUnit/CUnitCI.h>

struct Library *MUIMasterBase = NULL;

static IPTR nget(Object *obj, ULONG attr)
{
    IPTR val = 0;

    get(obj, attr, &val);
    return val;
}

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

static void test_dont_underflow_window_sleep()
{
    Object *app = ApplicationObject, End;
    set(app, MUIA_Application_Sleep, TRUE);

    Object *win = WindowObject, End;
    DoMethod(app, OM_ADDMEMBER, win);
    set(win, MUIA_Window_Open, TRUE);

    CU_ASSERT_EQUAL(0, nget(win, MUIA_Window_Sleep));

    set(app, MUIA_Application_Sleep, FALSE);
    CU_ASSERT_EQUAL(0, nget(app, MUIA_Application_Sleep));
    CU_ASSERT_EQUAL(0, nget(win, MUIA_Window_Sleep));

    MUI_DisposeObject(app);
}

static void test_put_only_open_window_to_sleep()
{
    Object *app = ApplicationObject, End;

    Object *win1 = WindowObject, End;
    Object *win2 = WindowObject, End;
    DoMethod(app, OM_ADDMEMBER, win1);
    DoMethod(app, OM_ADDMEMBER, win2);

    set(win1, MUIA_Window_Open, TRUE);

    CU_ASSERT_EQUAL(0, nget(win1, MUIA_Window_Sleep));
    CU_ASSERT_EQUAL(0, nget(win2, MUIA_Window_Sleep));

    set(app, MUIA_Application_Sleep, TRUE);


    CU_ASSERT_EQUAL(1, nget(win1, MUIA_Window_Sleep));
    CU_ASSERT_EQUAL(0, nget(win2, MUIA_Window_Sleep));

    set(app, MUIA_Application_Sleep, FALSE);
    set(win1, MUIA_Window_Open, FALSE);

    MUI_DisposeObject(app);
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE("MUIM_HandleEvent_Suite", __cu_suite_setup, __cu_suite_teardown, __cu_test_setup, __cu_test_teardown);
    CUNIT_CI_TEST(test_dont_underflow_window_sleep);
    CUNIT_CI_TEST(test_put_only_open_window_to_sleep);
    return CU_CI_RUN_SUITES();
}
