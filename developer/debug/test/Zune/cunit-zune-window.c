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

#define ENABLE_CLICK
#include "../test-util.h"

#if defined(__AROS__)
#include <CUnit/CUnitCI.h>
#endif

/* Needed for compilation with m68-amigaos-gcc */
struct Library *MUIMasterBase = NULL;

struct Data
{
    LONG dummy;
};

struct MUI_CustomClass *mcc_String;

#define EVENT_LOOP                                          \
    Click(w, xcoord[j], ycoord[j]);                         \
                                                            \
    sigs = 0;                                               \
    do {                                                    \
        DoMethod(app, MUIM_Application_NewInput, &sigs);    \
    } while (sigs == 0);                                    \
    Delay(2);

LONG global_GoActive    =  0;
LONG global_GoInactive  =  0;

static IPTR mGoActive(struct IClass *cl, Object *obj, Msg msg)
{
    global_GoActive++;

    return DoSuperMethodA(cl, obj, msg);
}

static IPTR mGoInactive(struct IClass *cl, Object *obj, Msg msg)
{
    global_GoInactive++;

    return DoSuperMethodA(cl, obj, msg);
}

BOOPSI_DISPATCHER(IPTR, dispatcher, cl, obj, msg)
{
    switch (msg->MethodID)
    {
        case MUIM_GoActive:     return mGoActive(cl, obj, (APTR)msg);
        case MUIM_GoInactive:   return mGoInactive(cl, obj, (APTR)msg);
    }
    return DoSuperMethodA(cl, obj, msg);
}
BOOPSI_DISPATCHER_END

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

    mcc_String = MUI_CreateCustomClass(NULL, MUIC_String, NULL, sizeof(struct Data), dispatcher);

    return CUE_SUCCESS;
}

CU_SUITE_TEARDOWN()
{
    MUI_DeleteCustomClass(mcc_String);
    CloseLibrary(MUIMasterBase);
    return CUE_SUCCESS;
}

static void test_window_not_copying_title()
{
    TEXT wintitle[] = "TITLE";

    Object *app = ApplicationObject, End;

    Object *win = WindowObject,
           MUIA_Window_Title, wintitle,
           MUIA_Window_RootObject, RectangleObject,
               End,
           End;
    DoMethod(app, OM_ADDMEMBER, win);
    set(win, MUIA_Window_Open, TRUE);

    CU_ASSERT_EQUAL(wintitle, (APTR)nget(win, MUIA_Window_Title));

    wintitle[0] = 'B';
    CU_ASSERT_STRING_EQUAL("BITLE", (APTR)nget(win, MUIA_Window_Title));

    MUI_DisposeObject(app);
}

static void test_window_width_minmax()
{
#if defined(__AROS__)
    const ULONG expwidth = 171;
#else
    const ULONG expwidth = 134;
#endif
    Object *app = ApplicationObject, End;

    Object *win = WindowObject,
            MUIA_Window_Title, "Width_MinMax",
            MUIA_Window_Width, MUIV_Window_Width_MinMax(10),
            WindowContents,
                VGroup,
                    Child, HGroup,MUIA_Weight, 9999,
                        Child, HSpace(0),
                        Child, SimpleButton("BUTTON"),
                        Child, HSpace(0),
                    End,
                End,
            End;
    DoMethod(app, OM_ADDMEMBER, win);
    set(win, MUIA_Window_Open, TRUE);

    CU_ASSERT_EQUAL(expwidth, (ULONG)nget(win, MUIA_Window_Width));

    MUI_DisposeObject(app);
}

static void test_window_activate_object_only_once()
{
    Object *app = ApplicationObject, End;

    Object *string = NewObject(mcc_String->mcc_Class, NULL, TAG_DONE);

    Object *win = WindowObject,
            MUIA_Window_Title, "ActivateObject",
            MUIA_Window_Width, 100,
            WindowContents,
                VGroup,
                    Child, HGroup,
                        Child, HSpace(0),
                        Child, string,
                    End,
                End,
            End;
    DoMethod(app, OM_ADDMEMBER, win);
    set(win, MUIA_Window_Open, TRUE);

    set(win, MUIA_Window_ActiveObject, (IPTR)string);
    CU_ASSERT_EQUAL(1, global_GoActive);
    CU_ASSERT_EQUAL(0, global_GoInactive);
    set(win, MUIA_Window_ActiveObject, (IPTR)string);
    CU_ASSERT_EQUAL(1, global_GoActive);
    CU_ASSERT_EQUAL(0, global_GoInactive);

    MUI_DisposeObject(app);
}

/* This test is not passing on AmigaOS 3.1 + MUI 3.9. String object is not accepting simulated clicks*/
static void test_window_activate_object_again_after_click()
{
#if defined(__AROS__)
    const LONG xcoord[] = { 50, 130, 200, 130 };
    const LONG ycoord[] = { 35, 35, 35, 35 };
#else
    const LONG xcoord[] = { 50, 160, 220, 160 };
    const LONG ycoord[] = { 24, 24, 24, 24 };
#endif

    global_GoActive = 0;
    global_GoInactive = 0;

    Object *app = ApplicationObject, End;

    Object *string = NewObject(mcc_String->mcc_Class, NULL, TAG_DONE);

    Object *win = WindowObject,
            MUIA_Window_Title, "ActivateObjectAfterClick",
            WindowContents,
                VGroup,
                    Child, HGroup,
                        Child, SimpleButton("BUTTON"),
                        Child, string,
                        Child, StringObject, StringFrame, End,
                    End,
                End,
            End;
    DoMethod(app, OM_ADDMEMBER, win);
    set(win, MUIA_Window_Open, TRUE);

    set(win, MUIA_Window_ActiveObject, (IPTR)string);
    CU_ASSERT_EQUAL(1, global_GoActive);
    CU_ASSERT_EQUAL(0, global_GoInactive);

    ULONG sigs = 0;
    struct Window *w = _window(string);

    for (int j = 0; j < 4; j++) {

        EVENT_LOOP

        switch(j)
        {
        case 0:
            /* Click on the button. String stays active */
            CU_ASSERT_EQUAL((IPTR)string, nget(win, MUIA_Window_ActiveObject));
            CU_ASSERT_EQUAL(1, global_GoActive);
            CU_ASSERT_EQUAL(0, global_GoInactive);
            break;
        case 1:
            /* Click on the string. String stays active */
            CU_ASSERT_EQUAL((IPTR)string, nget(win, MUIA_Window_ActiveObject));
            CU_ASSERT_EQUAL(1, global_GoActive);
            CU_ASSERT_EQUAL(0, global_GoInactive);
            break;
        case 2:
            /* Click on the second string. String becomes inactive */
            CU_ASSERT_NOT_EQUAL((IPTR)string, nget(win, MUIA_Window_ActiveObject));
            CU_ASSERT_EQUAL(1, global_GoActive);
            CU_ASSERT_EQUAL(1, global_GoInactive);
            break;
        case 3:
            /* Click on the first string. String becomes active */
            CU_ASSERT_EQUAL((IPTR)string, nget(win, MUIA_Window_ActiveObject));
            CU_ASSERT_EQUAL(2, global_GoActive);
            CU_ASSERT_EQUAL(1, global_GoInactive);
            break;
        }
    }

    MUI_DisposeObject(app);
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(MUIC_Window_Suite, __cu_suite_setup, __cu_suite_teardown, NULL, NULL);
    CUNIT_CI_TEST(test_window_not_copying_title);
    CUNIT_CI_TEST(test_window_width_minmax);
    CUNIT_CI_TEST(test_window_activate_object_only_once);
    CUNIT_CI_TEST(test_window_activate_object_again_after_click);

    return CU_CI_RUN_SUITES();
}
