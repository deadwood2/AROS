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

#include "../test-util.h"

#if defined(__AROS__)
#include <CUnit/CUnitCI.h>
#endif

struct Library *MUIMasterBase;

static struct Hook str1ackhook;
static struct Hook str2ackhook;

#define EVENT_LOOP                                          \
    sigs = 0;                                               \
    do {                                                    \
        DoMethod(app, MUIM_Application_NewInput, &sigs);    \
    } while (sigs == 0);                                    \

LONG global_String1AckHookVal = 0;

static void notify1function(struct Hook *hook, Object *obj, APTR msg)
{
    global_String1AckHookVal++;
}

LONG global_String2AckHookVal = 0;

static void notify2function(struct Hook *hook, Object *obj, APTR msg)
{
    global_String2AckHookVal++;
}

CU_SUITE_SETUP()
{
    MUIMasterBase = OpenLibrary((STRPTR)MUIMASTER_NAME, 0);
    if (!MUIMasterBase)
        CUE_SINIT_FAILED;

    str1ackhook.h_Entry = HookEntry;
    str1ackhook.h_SubEntry = (HOOKFUNC)notify1function;

    str2ackhook.h_Entry = HookEntry;
    str2ackhook.h_SubEntry = (HOOKFUNC)notify2function;

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

static void test_string_string_acknowledged()
{
    Object *wnd;
    Object *app;
    Object *str1, *str2;

    app = ApplicationObject,
        SubWindow, wnd = WindowObject,
            MUIA_Window_Activate, TRUE,
            WindowContents, HGroup,
                GroupFrame,
                Child, str1 = MUI_MakeObject(MUIO_String, NULL, 200),
                Child, str2 = StringObject, StringFrame, MUIA_String_MaxLen, 200, MUIA_String_Contents, "XYZ", End,
            End,
        End,
    End;

    if (app)
    {
        UBYTE *tmp;

        DoMethod
        (
            str1, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
            (IPTR)str1, 3, MUIM_CallHook, &str1ackhook, MUIV_TriggerValue
        );

        DoMethod
        (
            str2, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
            (IPTR)str2, 3, MUIM_CallHook, &str2ackhook, MUIV_TriggerValue
        );

        CU_ASSERT_EQUAL(0, global_String1AckHookVal);
        tmp = NULL; get(str1, MUIA_String_Contents, &tmp);
        CU_ASSERT_NOT_EQUAL_FATAL(NULL, tmp);
        CU_ASSERT_STRING_EQUAL("", tmp);
        tmp = NULL; get(str1, MUIA_String_Acknowledge, &tmp);
        CU_ASSERT_NOT_EQUAL_FATAL(NULL, tmp);
        CU_ASSERT_STRING_EQUAL("", tmp);

        CU_ASSERT_EQUAL(0, global_String2AckHookVal);
        tmp = NULL; get(str2, MUIA_String_Contents, &tmp);
        CU_ASSERT_NOT_EQUAL_FATAL(NULL, tmp);
        CU_ASSERT_STRING_EQUAL("XYZ", tmp);
        tmp = NULL; get(str2, MUIA_String_Acknowledge, &tmp);
        CU_ASSERT_NOT_EQUAL_FATAL(NULL, tmp);
        CU_ASSERT_STRING_EQUAL("XYZ", tmp);

        set(wnd, MUIA_Window_Open, TRUE);

        set(str1, MUIA_String_Contents, (IPTR)"ABC");
        CU_ASSERT_EQUAL(0, global_String1AckHookVal);
        tmp = NULL; get(str1, MUIA_String_Contents, &tmp);
        CU_ASSERT_NOT_EQUAL_FATAL(NULL, tmp);
        CU_ASSERT_STRING_EQUAL("ABC", tmp);
        tmp = NULL; get(str1, MUIA_String_Acknowledge, &tmp);
        CU_ASSERT_NOT_EQUAL_FATAL(NULL, tmp);
        CU_ASSERT_STRING_EQUAL("ABC", tmp);

        set(str2, MUIA_String_Contents, NULL);
        CU_ASSERT_EQUAL(0, global_String2AckHookVal);
        tmp = NULL; get(str2, MUIA_String_Contents, &tmp);
        CU_ASSERT_NOT_EQUAL_FATAL(NULL, tmp);
        CU_ASSERT_STRING_EQUAL("", tmp);
        tmp = NULL; get(str2, MUIA_String_Acknowledge, &tmp);
        CU_ASSERT_NOT_EQUAL_FATAL(NULL, tmp);
        CU_ASSERT_STRING_EQUAL("", tmp);

        set(wnd, MUIA_Window_Open, FALSE);
        MUI_DisposeObject(app);
    }
    else
    {
        CU_ASSERT(0);
    }
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE("MUIC_String_Suite", __cu_suite_setup, __cu_suite_teardown, __cu_test_setup, __cu_test_teardown);
    CUNIT_CI_TEST(test_string_string_acknowledged);
    return CU_CI_RUN_SUITES();
}
