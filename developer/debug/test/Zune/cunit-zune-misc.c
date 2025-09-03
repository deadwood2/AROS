/*
    Copyright © 2025, The AROS Development Team. All rights reserved.
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

static void test_mui_makeobject_escape_seq()
{
    Object *btn1, *btn2;

    Object *app = ApplicationObject, End;

    Object *win = WindowObject,
            WindowContents, HGroup,
                GroupFrame,
                Child, btn1 = SimpleButton("\33I[5:SYS:AAAA_A]\33I[5:SYS:AAA_B] _CCC"),
                Child, btn2 = SimpleButton("_DDD"),
            End,
        End;

    DoMethod(app, OM_ADDMEMBER, win);
    set(win, MUIA_Window_Open, TRUE);

    /* This case looks like a bug, but MUI 3.9 and MUI 5.x on MorphOS behave like that */
    CU_ASSERT_EQUAL((LONG)'a', (LONG)nget(btn1, MUIA_ControlChar));

    CU_ASSERT_EQUAL((LONG)'d', (LONG)nget(btn2, MUIA_ControlChar));

    MUI_DisposeObject(app);
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(MUI_Misc_Suite, __cu_suite_setup, __cu_suite_teardown, NULL, NULL);
    CUNIT_CI_TEST(test_mui_makeobject_escape_seq);

    return CU_CI_RUN_SUITES();
}
