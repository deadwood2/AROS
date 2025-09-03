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

struct Library *MUIMasterBase;

static struct Hook notifyhook;

#define EVENT_LOOP                                          \
    globalReset();                                          \
                                                            \
    Click(w, xcoord[j], ycoord[j]);                         \
                                                            \
    sigs = 0;                                               \
    do {                                                    \
        DoMethod(app, MUIM_Application_NewInput, &sigs);    \
    } while (sigs == 0);                                    \
    Delay(2);

LONG global_NotifyHookVal = 0;

static void notifyfunction(struct Hook *hook, Object *obj, APTR msg)
{
    LONG val = *(LONG *)msg;
    global_NotifyHookVal = +val;
}

CU_SUITE_SETUP()
{
    MUIMasterBase = OpenLibrary((STRPTR)MUIMASTER_NAME, 0);
    if (!MUIMasterBase)
        CUE_SINIT_FAILED;

    notifyhook.h_Entry = HookEntry;
    notifyhook.h_SubEntry = (HOOKFUNC)notifyfunction;

    return CUE_SUCCESS;
}

CU_SUITE_TEARDOWN()
{
    CloseLibrary(MUIMasterBase);
    return CUE_SUCCESS;
}

static void globalReset()
{
    global_NotifyHookVal  =   0;
}

static void test_cycle_active_notify_on_click()
{
    Object *wnd;
    Object *app;
    Object *cyc;
    CONST_STRPTR cyclestrings[] = { "Zero", "One", "Two", "Three", NULL };
#if defined(__AROS__)
    const LONG xcoord[] = { 24, 24, 24, 24, 24};
    const LONG ycoord[] = { 38, 38, 38, 38, 38};
#else
    const LONG xcoord[] = { 17, 17, 17, 17, 17};
    const LONG ycoord[] = { 21, 21, 21, 21, 21};
#endif

    app = ApplicationObject,
        SubWindow, wnd = WindowObject,
            MUIA_Window_Activate, TRUE,
            WindowContents, HGroup,
                GroupFrame,
                Child, cyc = CycleObject,
                    MUIA_Cycle_Entries, cyclestrings,
                    MUIA_Cycle_Active,  0,
                End,
            End,
        End,
    End;

    if (app)
    {
        IPTR tmp;
        ULONG sigs = 0;

        DoMethod
        (
            cyc, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
            (IPTR)cyc, 3, MUIM_CallHook, &notifyhook, MUIV_TriggerValue
        );

        set(wnd,MUIA_Window_Open,TRUE);
        globalReset();

        get(cyc, MUIA_Cycle_Active, &tmp);
        CU_ASSERT_EQUAL(0, tmp);

        struct Window *w = _window(cyc);

        for (int j = 0; j < 5; j++) {

            EVENT_LOOP

            switch(j)
            {
            case 0:
                CU_ASSERT(global_NotifyHookVal == 1);
                break;
            case 1:
                CU_ASSERT(global_NotifyHookVal == 2);
                break;
            case 2:
                CU_ASSERT(global_NotifyHookVal == 3);
                break;
            case 3:
                CU_ASSERT(global_NotifyHookVal == 0);
                break;
            case 4:
                CU_ASSERT(global_NotifyHookVal == 1);
                break;
            }
        }

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
    CU_CI_DEFINE_SUITE(MUIC_Cycle_Suite, __cu_suite_setup, __cu_suite_teardown, NULL, NULL);
    CUNIT_CI_TEST(test_cycle_active_notify_on_click);
    return CU_CI_RUN_SUITES();
}
