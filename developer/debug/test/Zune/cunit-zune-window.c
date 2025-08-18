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

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE("MUIC_Window_Suite", __cu_suite_setup, __cu_suite_teardown, NULL, NULL);
    CUNIT_CI_TEST(test_window_not_copying_title);

    return CU_CI_RUN_SUITES();
}
