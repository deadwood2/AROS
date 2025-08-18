/*
    Copyright © 2022, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <libraries/mui.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <clib/alib_protos.h>

#include "../test-util.h"

#if defined(__AROS__)
#include <CUnit/CUnitCI.h>
#endif

/* Needed for compilation with m68-amigaos-gcc */
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

static void test_nesting_application_sleep()
{
    /* MUI 3.8 always returns 0 */
    Object *app = ApplicationObject, End;
    CU_ASSERT_EQUAL(0, nget(app, MUIA_Application_Sleep));

    set(app, MUIA_Application_Sleep, TRUE);
    CU_ASSERT_EQUAL(0, nget(app, MUIA_Application_Sleep));

    set(app, MUIA_Application_Sleep, TRUE);
    CU_ASSERT_EQUAL(0, nget(app, MUIA_Application_Sleep));

    set(app, MUIA_Application_Sleep, FALSE);
    CU_ASSERT_EQUAL(0, nget(app, MUIA_Application_Sleep));

    set(app, MUIA_Application_Sleep, FALSE);
    CU_ASSERT_EQUAL(0, nget(app, MUIA_Application_Sleep));

    MUI_DisposeObject(app);
}

static void test_nesting_window_sleep()
{
    Object *app = ApplicationObject, End;
    Object *win = WindowObject,
           MUIA_Window_Title, "A window",
           MUIA_Window_RootObject, RectangleObject,
               End,
           End;
    DoMethod(app, OM_ADDMEMBER, win);
    CU_ASSERT_EQUAL(0, nget(win, MUIA_Window_Sleep));

    set(win, MUIA_Window_Sleep, TRUE);
    CU_ASSERT_EQUAL(0, nget(win, MUIA_Window_Sleep));

    set(win, MUIA_Window_Open, TRUE);

    set(win, MUIA_Window_Sleep, TRUE);
    CU_ASSERT_EQUAL(1, nget(win, MUIA_Window_Sleep));

    set(win, MUIA_Window_Sleep, TRUE);
    CU_ASSERT_EQUAL(1, nget(win, MUIA_Window_Sleep));

    set(win, MUIA_Window_Sleep, FALSE);
    CU_ASSERT_EQUAL(1, nget(win, MUIA_Window_Sleep));

    set(win, MUIA_Window_Sleep, FALSE);
    CU_ASSERT_EQUAL(0, nget(win, MUIA_Window_Sleep));

    MUI_DisposeObject(app);
}

static void test_dont_underflow_window_sleep()
{
    Object *app = ApplicationObject, End;
    set(app, MUIA_Application_Sleep, TRUE);

    Object *win = WindowObject,
           MUIA_Window_Title, "A window",
           MUIA_Window_RootObject, RectangleObject,
               End,
           End;
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

    Object *win1 = WindowObject,
           MUIA_Window_Title, "A window",
           MUIA_Window_RootObject, RectangleObject,
               End,
           End;
    Object *win2 = WindowObject,
           MUIA_Window_Title, "A window",
           MUIA_Window_RootObject, RectangleObject,
               End,
           End;
    DoMethod(app, OM_ADDMEMBER, win1);
    DoMethod(app, OM_ADDMEMBER, win2);

    set(win1, MUIA_Window_Open, TRUE);

    CU_ASSERT_EQUAL(0, nget(win1, MUIA_Window_Sleep));
    CU_ASSERT_EQUAL(0, nget(win2, MUIA_Window_Sleep));

    set(app, MUIA_Application_Sleep, TRUE);

    CU_ASSERT_EQUAL(1, nget(win1, MUIA_Window_Sleep));
    CU_ASSERT_EQUAL(0, nget(win2, MUIA_Window_Sleep));

    set(win2, MUIA_Window_Open, TRUE);
    set(app, MUIA_Application_Sleep, TRUE);
    CU_ASSERT_EQUAL(1, nget(win1, MUIA_Window_Sleep));
    CU_ASSERT_EQUAL(1, nget(win2, MUIA_Window_Sleep));

    set(app, MUIA_Application_Sleep, FALSE);
    CU_ASSERT_EQUAL(1, nget(win1, MUIA_Window_Sleep));
    CU_ASSERT_EQUAL(0, nget(win2, MUIA_Window_Sleep));

    set(app, MUIA_Application_Sleep, FALSE);
    CU_ASSERT_EQUAL(0, nget(win1, MUIA_Window_Sleep));
    CU_ASSERT_EQUAL(0, nget(win2, MUIA_Window_Sleep));

    MUI_DisposeObject(app);
}

static void test_put_only_open_window_to_sleep_2()
{
    Object *app = ApplicationObject, End;

    Object *win = WindowObject,
           MUIA_Window_Title, "A window",
           MUIA_Window_RootObject, RectangleObject,
               End,
           End;
    DoMethod(app, OM_ADDMEMBER, win);

    // Testing MUIA_Application_Sleep
    CU_ASSERT_EQUAL(0, nget(win, MUIA_Window_Sleep));

    set(app, MUIA_Application_Sleep, TRUE);
    CU_ASSERT_EQUAL(0, nget(win, MUIA_Window_Sleep));
    set(app, MUIA_Application_Sleep, FALSE);

    set(win, MUIA_Window_Open, TRUE);
    set(app, MUIA_Application_Sleep, TRUE);
    CU_ASSERT_EQUAL(1, nget(win, MUIA_Window_Sleep));

    set(app, MUIA_Application_Sleep, TRUE);
    CU_ASSERT_EQUAL(1, nget(win, MUIA_Window_Sleep));

    set(app, MUIA_Application_Sleep, FALSE);
    CU_ASSERT_EQUAL(1, nget(win, MUIA_Window_Sleep));

    set(win, MUIA_Window_Open, FALSE);
    set(app, MUIA_Application_Sleep, FALSE);
    CU_ASSERT_EQUAL(0, nget(win, MUIA_Window_Sleep));

    // Testing MUIA_Window_Sleep
    CU_ASSERT_EQUAL(0, nget(win, MUIA_Window_Sleep));

    set(win, MUIA_Window_Sleep, TRUE);
    CU_ASSERT_EQUAL(0, nget(win, MUIA_Window_Sleep));
    set(win, MUIA_Window_Sleep, FALSE);

    set(win, MUIA_Window_Open, TRUE);
    set(win, MUIA_Window_Sleep, TRUE);
    CU_ASSERT_EQUAL(1, nget(win, MUIA_Window_Sleep));

    set(win, MUIA_Window_Sleep, TRUE);
    CU_ASSERT_EQUAL(1, nget(win, MUIA_Window_Sleep));

    set(win, MUIA_Window_Sleep, FALSE);
    CU_ASSERT_EQUAL(1, nget(win, MUIA_Window_Sleep));

    set(win, MUIA_Window_Open, FALSE);
    set(win, MUIA_Window_Sleep, FALSE);
    CU_ASSERT_EQUAL(0, nget(win, MUIA_Window_Sleep));

    MUI_DisposeObject(app);
}


int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE("MUIA_Sleep_Suite", __cu_suite_setup, __cu_suite_teardown, NULL, NULL);
    CUNIT_CI_TEST(test_dont_underflow_window_sleep);
    CUNIT_CI_TEST(test_put_only_open_window_to_sleep);
    CUNIT_CI_TEST(test_put_only_open_window_to_sleep_2);
    CUNIT_CI_TEST(test_nesting_application_sleep);
    CUNIT_CI_TEST(test_nesting_window_sleep);

    return CU_CI_RUN_SUITES();
}
