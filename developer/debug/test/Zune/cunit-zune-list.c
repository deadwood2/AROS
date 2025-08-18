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

ULONG lvLA;
ULONG lvSC;
ULONG liLA;
ULONG liSC;

static void reset_globals()
{
    lvLA = 0;
    lvSC = 0;
    liLA = 0;
    liSC = 0;
}

static void setup_list(Object **app, Object **win, struct Window **w, Object **liout)
{
    Object *lv, *li;

    *app = ApplicationObject, End;

    *win = WindowObject,
            MUIA_Window_Title, "List",
            WindowContents, VGroup,
                Child, lv = ListviewObject,
                    MUIA_Listview_List,
                        li = ListObject, InputListFrame, End,
                    End,
                End,
            End;

    DoMethod(lv, MUIM_List_InsertSingle, "one", MUIV_List_Insert_Active);
    DoMethod(lv, MUIM_List_InsertSingle, "two", MUIV_List_Insert_Active);
    DoMethod(lv, MUIM_List_InsertSingle, "three", MUIV_List_Insert_Active);


    DoMethod(*app, OM_ADDMEMBER, *win);

    DoMethod(lv, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime, *app, 3, MUIM_WriteLong, 1, &lvLA);
    DoMethod(lv, MUIM_Notify, MUIA_Listview_SelectChange, TRUE, *app, 3, MUIM_WriteLong, 1, &lvSC);

    DoMethod(li, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime, *app, 3, MUIM_WriteLong, 1, &liLA);
    DoMethod(li, MUIM_Notify, MUIA_Listview_SelectChange, TRUE, *app, 3, MUIM_WriteLong, 1, &liSC);

    set(*win, MUIA_Window_Open, TRUE);

    *w = _window(lv);
    if (liout) *liout = li;
}

static void test_list_active_not_selected()
{
    Object *app, *win;
    struct Window *w;

    setup_list(&app, &win, &w, NULL);

    reset_globals();

    Click(w, 50, 40);

    LONG sigs = 0;
    do {
        DoMethod(app, MUIM_Application_NewInput, &sigs);
    } while (sigs == 0);

    CU_ASSERT_EQUAL(1, lvLA);
    CU_ASSERT_EQUAL(0, lvSC);
    CU_ASSERT_EQUAL(1, liLA);
    CU_ASSERT_EQUAL(0, liSC);

    MUI_DisposeObject(app);
}

static void test_list_active_selected()
{
    Object *app, *win, *li;
    struct Window *w;

    setup_list(&app, &win, &w, &li);

    reset_globals();

    Click(w, 50, 40);

    LONG sigs = 0;
    do {
        DoMethod(app, MUIM_Application_NewInput, &sigs);
    } while (sigs == 0);

    DoMethod(li, MUIM_List_Select, MUIV_List_Select_All, MUIV_List_Select_On, NULL);

    CU_ASSERT_EQUAL(1, lvLA);
    CU_ASSERT_EQUAL(0, lvSC);
    CU_ASSERT_EQUAL(1, liLA);
    CU_ASSERT_EQUAL(0, liSC);

    MUI_DisposeObject(app);
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE("MUIC_List_Suite", __cu_suite_setup, __cu_suite_teardown, NULL, NULL);
    CUNIT_CI_TEST(test_list_active_not_selected);
    CUNIT_CI_TEST(test_list_active_selected);

    return CU_CI_RUN_SUITES();
}
