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

static IPTR nget(Object *obj, ULONG attr)
{
    IPTR val = 0;

    get(obj, attr, &val);
    return val;
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
    CU_CI_DEFINE_SUITE("MUIC_Window_Suite", NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_window_not_copying_title);

    return CU_CI_RUN_SUITES();
}
