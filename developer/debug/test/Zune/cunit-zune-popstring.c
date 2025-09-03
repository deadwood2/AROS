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

static struct Hook strackhook;
static struct Hook closehook;
static struct Hook openhook;

#define EVENT_LOOP                                          \
    sigs = 0;                                               \
    do {                                                    \
        DoMethod(app, MUIM_Application_NewInput, &sigs);    \
    } while (sigs == 0);                                    \

LONG global_StringAckHookVal = 0;

static void notifyfunction(struct Hook *hook, Object *obj, APTR msg)
{
    global_StringAckHookVal++;
}

static ULONG closefunc(struct Hook *hook, Object *obj, IPTR *msg)
{
    Object *string = (Object *) msg[0];
    IPTR suc = (IPTR) msg[1];

    if (suc)
        set(string, MUIA_String_Contents, (IPTR)"ACK");

    return 0;
}

static ULONG openfunc(struct Hook *hook, Object *obj, APTR msg)
{
    return 1;
}

CU_SUITE_SETUP()
{
    MUIMasterBase = OpenLibrary((STRPTR)MUIMASTER_NAME, 0);
    if (!MUIMasterBase)
        CUE_SINIT_FAILED;

    strackhook.h_Entry = HookEntry;
    strackhook.h_SubEntry = (HOOKFUNC)notifyfunction;

    closehook.h_Entry = HookEntry;
    closehook.h_SubEntry = (HOOKFUNC)closefunc;

    openhook.h_Entry = HookEntry;
    openhook.h_SubEntry = (HOOKFUNC)openfunc;

    return CUE_SUCCESS;
}

CU_SUITE_TEARDOWN()
{
    CloseLibrary(MUIMasterBase);
    return CUE_SUCCESS;
}

static void test_popstring_string_acknowledged_when_closed_with_success()
{
    Object *wnd;
    Object *app;
    Object *pop;
    Object *str;

    app = ApplicationObject,
        SubWindow, wnd = WindowObject,
            MUIA_Window_Activate, TRUE,
            WindowContents, HGroup,
                GroupFrame,
                Child, pop = PopstringObject,
                    MUIA_Popstring_String, str = MUI_MakeObject(MUIO_String, NULL, 200),
                    MUIA_Popstring_Button, PopButton(MUII_PopUp),
                    MUIA_Popstring_OpenHook, &openhook,
                    MUIA_Popstring_CloseHook, &closehook,
                End,
            End,
        End,
    End;

    if (app)
    {
        UBYTE *tmp;
        IPTR sigs;

        DoMethod
        (
            pop, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
            (IPTR)pop, 3, MUIM_CallHook, &strackhook, MUIV_TriggerValue
        );

        set(str, MUIA_String_Contents, (IPTR)"ABCD");
        set(wnd, MUIA_Window_Open, TRUE);


        tmp = NULL; get(pop, MUIA_String_Contents, &tmp);
        CU_ASSERT_NOT_EQUAL_FATAL(NULL, tmp);
        CU_ASSERT_STRING_EQUAL("ABCD", tmp);

        tmp = NULL; get(pop, MUIA_String_Acknowledge, &tmp);
        CU_ASSERT_NOT_EQUAL_FATAL(NULL, tmp);
        CU_ASSERT_STRING_EQUAL("ABCD", tmp);

        /* Open, Close with not success */
        DoMethod(pop, MUIM_Popstring_Open);
        DoMethod(pop, MUIM_Popstring_Close, FALSE);
        EVENT_LOOP

        CU_ASSERT_EQUAL(0, global_StringAckHookVal);

        tmp = NULL; get(pop, MUIA_String_Contents, &tmp);
        CU_ASSERT_NOT_EQUAL_FATAL(NULL, tmp);
        CU_ASSERT_STRING_EQUAL("ABCD", tmp);

        tmp = NULL; get(pop, MUIA_String_Acknowledge, &tmp);
        CU_ASSERT_NOT_EQUAL_FATAL(NULL, tmp);
        CU_ASSERT_STRING_EQUAL("ABCD", tmp);

        /* Open, Close with success */
        DoMethod(pop, MUIM_Popstring_Open);
        DoMethod(pop, MUIM_Popstring_Close, TRUE);
        EVENT_LOOP

        CU_ASSERT_EQUAL(0, global_StringAckHookVal);

        tmp = NULL; get(pop, MUIA_String_Contents, &tmp);
        CU_ASSERT_NOT_EQUAL_FATAL(NULL, tmp);
        CU_ASSERT_STRING_EQUAL("ACK", tmp);

        tmp = NULL; get(pop, MUIA_String_Acknowledge, &tmp);
        CU_ASSERT_NOT_EQUAL_FATAL(NULL, tmp);
        CU_ASSERT_STRING_EQUAL("ACK", tmp);

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
    CU_CI_DEFINE_SUITE(MUIC_Popstring_Suite, __cu_suite_setup, __cu_suite_teardown, NULL, NULL);
    CUNIT_CI_TEST(test_popstring_string_acknowledged_when_closed_with_success);
    return CU_CI_RUN_SUITES();
}
