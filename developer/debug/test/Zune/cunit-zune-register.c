/*
    Copyright (C) 2023, The AROS Development Team.
    All rights reserved.
*/


#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <clib/alib_protos.h>

#include <libraries/mui.h>

#define ENABLE_CLICK
#include "../test-util.h"

#if defined(__AROS__)
#include <CUnit/CUnitCI.h>
#endif
/* Needed for compilation with m68-amigaos-gcc */
struct Library *MUIMasterBase = NULL;

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

/* While invisible objects will be receive events, they should only be handled
   by currently visibile ones. This test creates two buttons on two tabs. At
   any point in time only one button is visible and it should be the one that
   is "clicked" */

static void test_register_not_copying_titles()
{
    Object *wnd;
    Object *app;
    Object *reg;
    char *first = "First";
    char *tabs[] = { "", "Second", NULL };
    tabs[0] = first;

    app = ApplicationObject,
        SubWindow, wnd = WindowObject,
            MUIA_Window_Activate, TRUE,
            WindowContents, reg = RegisterGroup((IPTR)tabs),
                Child, (IPTR)HGroup,
                        GroupFrame,
                        Child, (IPTR)(RectangleObject, End),
                    End,
                Child, (IPTR)HGroup,
                        GroupFrame,
                        Child, (IPTR)(RectangleObject, End),
                End,
            End,
        End,
    End;

    if (app)
    {

        set(wnd, MUIA_Window_Open, TRUE);

        char **tmp = (char **)XGET(reg, MUIA_Register_Titles);

        CU_ASSERT(tmp == tabs);
        CU_ASSERT(tmp[0] == first);

        set(wnd, MUIA_Window_Open, FALSE);

        MUI_DisposeObject(app);
    }
}


int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(MUIC_Register_Suite, __cu_suite_setup, __cu_suite_teardown, NULL, NULL);
    CUNIT_CI_TEST(test_register_not_copying_titles);
    return CU_CI_RUN_SUITES();
}
