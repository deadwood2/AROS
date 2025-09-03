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

struct Library *MUIMasterBase = NULL;

struct Data
{
    struct MUI_EventHandlerNode ehnode;
};

int numMessages;

struct MUI_CustomClass *mcc_Rectangle;
struct MUI_CustomClass *mcc_Window;

static IPTR mSetup(struct IClass *cl, Object *obj, Msg msg, Object *win)
{
    struct Data *data = INST_DATA(cl, obj);

    if (!DoSuperMethodA(cl, obj, msg))
    {
        return FALSE;
    }

    data->ehnode.ehn_Object = obj;
    data->ehnode.ehn_Class  = cl;
    data->ehnode.ehn_Events = IDCMP_INTUITICKS;
    data->ehnode.ehn_Priority = 10;
    DoMethod(win, MUIM_Window_AddEventHandler, &data->ehnode);

    return TRUE;
}

static IPTR mCleanup(struct IClass *cl, Object *obj, Msg msg, Object *win)
{
    struct Data *data = INST_DATA(cl, obj);

    DoMethod(win, MUIM_Window_RemEventHandler, &data->ehnode);
    return DoSuperMethodA(cl, obj, msg);
}

static IPTR mHandleEvent(struct IClass *cl, Object *obj, struct  MUIP_HandleEvent *msg)
{
    if (msg->imsg->Class == IDCMP_INTUITICKS)
        numMessages++;
}

BOOPSI_DISPATCHER(IPTR, dispatcher, cl, obj, msg)
{
    switch (msg->MethodID)
    {
        case MUIM_Setup: return mSetup(cl, obj, (APTR)msg, _win(obj));
        case MUIM_Cleanup: return mCleanup(cl, obj, (APTR)msg, _win(obj));
        case MUIM_Window_Setup: return mSetup(cl, obj, (APTR)msg, obj);
        case MUIM_Window_Cleanup: return mCleanup(cl, obj, (APTR)msg, obj);
        case MUIM_HandleEvent:  return mHandleEvent(cl, obj, (APTR)msg);
    }
    return DoSuperMethodA(cl, obj, msg);
}
BOOPSI_DISPATCHER_END

CU_SUITE_SETUP()
{
    MUIMasterBase = OpenLibrary((STRPTR)MUIMASTER_NAME, 0);
    if (!MUIMasterBase)
        CUE_SINIT_FAILED;

    mcc_Rectangle   = MUI_CreateCustomClass(NULL, MUIC_Rectangle, NULL, sizeof(struct Data), dispatcher);
    mcc_Window      = MUI_CreateCustomClass(NULL, MUIC_Window, NULL, sizeof(struct Data), dispatcher);

    return CUE_SUCCESS;
}

CU_SUITE_TEARDOWN()
{
    MUI_DeleteCustomClass(mcc_Rectangle);
    MUI_DeleteCustomClass(mcc_Window);
    CloseLibrary(MUIMasterBase);
    return CUE_SUCCESS;
}

static void test_handleevent_rectangle()
{
    ULONG sigs;
    Object *app = ApplicationObject, End;
    Object *win = WindowObject, MUIA_Window_RootObject, NewObject(mcc_Rectangle->mcc_Class, NULL, TAG_END), End;
    DoMethod(app, OM_ADDMEMBER, win);

    numMessages = 0;

    set(win, MUIA_Window_Open, TRUE);

    for (int j = 0; j < 10; j++) {
        sigs = 0;
        do {
            DoMethod(app, MUIM_Application_NewInput, &sigs);
        } while (sigs == 0);
        Delay(1);
    }

    CU_ASSERT(numMessages > 0);

    MUI_DisposeObject(app);
}

static void test_handleevent_rectangle_on_not_visible_tab()
{
    ULONG sigs;
    CONST_STRPTR tabs[] = { "First", "Second", NULL };
    Object *app = ApplicationObject, End;
    Object *win = WindowObject,
        MUIA_Window_RootObject, RegisterGroup((IPTR)tabs),
            Child, (IPTR)VGroup,
                Child, (IPTR)(RectangleObject, End),
            End,
            Child, (IPTR)VGroup,
                Child, (IPTR)(NewObject(mcc_Rectangle->mcc_Class, NULL, TAG_END)),
            End,
        End,
    End;

    DoMethod(app, OM_ADDMEMBER, win);

    numMessages = 0;

    set(win, MUIA_Window_Open, TRUE);

    for (int j = 0; j < 10; j++) {
        sigs = 0;
        do {
            DoMethod(app, MUIM_Application_NewInput, &sigs);
        } while (sigs == 0);
        Delay(1);
    }

    CU_ASSERT(numMessages > 0);

    MUI_DisposeObject(app);
}

static void test_handleevent_window()
{
    ULONG sigs;
    Object *app = ApplicationObject, End;
    Object *win = NewObject(mcc_Window->mcc_Class, NULL, MUIA_Window_RootObject, RectangleObject, End, TAG_END);
    DoMethod(app, OM_ADDMEMBER, win);

    numMessages = 0;

    set(win, MUIA_Window_Open, TRUE);

    for (int j = 0; j < 10; j++) {
        sigs = 0;
        do {
            DoMethod(app, MUIM_Application_NewInput, &sigs);
        } while (sigs == 0);
        Delay(1);
    }

    CU_ASSERT(numMessages > 0);

    MUI_DisposeObject(app);
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(MUIM_HandleEvent_Suite, __cu_suite_setup, __cu_suite_teardown, NULL, NULL);
    CUNIT_CI_TEST(test_handleevent_rectangle);
    CUNIT_CI_TEST(test_handleevent_window);
    CUNIT_CI_TEST(test_handleevent_rectangle_on_not_visible_tab);
    return CU_CI_RUN_SUITES();
}
