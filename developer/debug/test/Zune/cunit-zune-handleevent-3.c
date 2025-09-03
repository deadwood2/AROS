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

struct Data
{
    LONG dummy;
};

struct MUI_CustomClass *mcc_Rectangle;

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

LONG global_Pressed     =  0;
LONG global_EventDown   =  0;
LONG global_EventUp     =  0;


IPTR mOM_NEW(struct IClass *cl, Object *obj, struct opSet *msg)
{
    obj = (Object *)DoSuperMethodA(cl, obj, (Msg)msg);

    return (IPTR)obj;
}

static IPTR mSetup(struct IClass *cl, Object *obj, Msg msg)
{
    struct Data *data = INST_DATA(cl, obj);

    if (!DoSuperMethodA(cl, obj, msg))
        return FALSE;

    MUI_RequestIDCMP(obj, IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE | IDCMP_RAWKEY);

    return TRUE;
}

static IPTR mCleanup(struct IClass *cl, Object *obj, Msg msg)
{
    struct Data *data = INST_DATA(cl, obj);

    MUI_RejectIDCMP(obj, IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE | IDCMP_RAWKEY);

    return DoSuperMethodA(cl, obj, msg);
}

static IPTR mHandleInput(struct IClass *cl, Object *obj, struct  MUIP_HandleInput *msg)
{
    if (msg->imsg->Class == IDCMP_MOUSEBUTTONS)
    {
        if (msg->imsg->Code == SELECTDOWN)
            global_EventDown++;

        if (msg->imsg->Code == SELECTUP)
            global_EventUp++;
    }

    return 0;
}


BOOPSI_DISPATCHER(IPTR, dispatcher, cl, obj, msg)
{
    switch (msg->MethodID)
    {
        case OM_NEW: return mOM_NEW(cl, obj, (struct opSet *)msg);
        case MUIM_Setup: return mSetup(cl, obj, (APTR)msg);
        case MUIM_Cleanup: return mCleanup(cl, obj, (APTR)msg);
        case MUIM_HandleInput:  return mHandleInput(cl, obj, (APTR)msg);
    }
    return DoSuperMethodA(cl, obj, msg);
}
BOOPSI_DISPATCHER_END

static struct Hook pressedhook;

static void pressedfunction(struct Hook *hook, Object *obj, APTR msg)
{
    LONG val = *(LONG *)msg;
    global_Pressed += val;
}

CU_SUITE_SETUP()
{
    MUIMasterBase = OpenLibrary((STRPTR)MUIMASTER_NAME, 0);
    if (!MUIMasterBase)
        CUE_SINIT_FAILED;

    mcc_Rectangle = MUI_CreateCustomClass(NULL, MUIC_Rectangle, NULL, sizeof(struct Data), dispatcher);

    pressedhook.h_Entry = HookEntry;
    pressedhook.h_SubEntry = (HOOKFUNC)pressedfunction;

    return CUE_SUCCESS;
}

CU_SUITE_TEARDOWN()
{
    MUI_DeleteCustomClass(mcc_Rectangle);
    CloseLibrary(MUIMasterBase);
    return CUE_SUCCESS;
}

static void globalReset()
{
    global_Pressed      =   0;
    global_EventDown    =   0;
    global_EventUp      =   0;
}

/* MUIM_HandleInpu will receive mouse button select up events even if processes by other objects */
static void test_handleevent_area_does_not_eat_selectup_event()
{
    Object *wnd;
    Object *app;
    Object *aRectangle, *anOrdButton;
    const LONG xcoord[] = { 0, 30, 65, 105, 130};
#if defined(__AROS__)
    const LONG ycoord[] = { 35, 35, 35, 35, 35};
#else
    const LONG ycoord[] = { 20, 20, 20, 20, 20};
#endif


    aRectangle = NewObject(mcc_Rectangle->mcc_Class, NULL,
                MUIA_FixWidthTxt, "REC2",
                MUIA_FixHeightTxt, "REC2",
                MUIA_Rectangle_HBar, TRUE,
                TAG_DONE);


    app = ApplicationObject,
        SubWindow, wnd = WindowObject,
            MUIA_Window_Activate, TRUE,
            WindowContents, HGroup,
                GroupFrame,
                Child, RectangleObject,
                    MUIA_FixWidthTxt, "REC0",
                    MUIA_FixHeightTxt, "REC0",
                    MUIA_Rectangle_HBar, TRUE,
                End,
                Child, anOrdButton = MUI_MakeObject(MUIO_Button,(IPTR)"BTN1"),
                Child, aRectangle,
            End,
        End,
    End;

    if (app)
    {
        ULONG sigs = 0;

        DoMethod
        (
            anOrdButton, MUIM_Notify, MUIA_Pressed, FALSE,
            (IPTR)anOrdButton, 3, MUIM_CallHook, &pressedhook, 7
        );

        set(wnd,MUIA_Window_Open,TRUE);

        struct Window *w = _window(aRectangle);

        for (int j = 0; j < 5; j++) {

            EVENT_LOOP

            switch(j)
            {
            case 0:
                CU_ASSERT(global_EventDown  == 1);
                CU_ASSERT(global_EventUp    == 1);
                CU_ASSERT(global_Pressed    == 0);
                break;
            case 1:
                CU_ASSERT(global_EventDown  == 1);
                CU_ASSERT(global_EventUp    == 1);
                CU_ASSERT(global_Pressed    == 0);
                break;
            case 2:
                CU_ASSERT(global_EventDown  == 0);
                CU_ASSERT(global_EventUp    == 1);
                CU_ASSERT(global_Pressed    == 7);
                break;
            case 3:
                CU_ASSERT(global_EventDown  == 1);
                CU_ASSERT(global_EventUp    == 1);
                CU_ASSERT(global_Pressed    == 0);
                break;
            case 4:
                CU_ASSERT(global_EventDown  == 1);
                CU_ASSERT(global_EventUp    == 1);
                CU_ASSERT(global_Pressed    == 0);
                break;
            }
        }

        MUI_DisposeObject(app);
    }
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(MUIM_HandleEvent_3_Suite, __cu_suite_setup, __cu_suite_teardown, NULL, NULL);
    CUNIT_CI_TEST(test_handleevent_area_does_not_eat_selectup_event);
    return CU_CI_RUN_SUITES();
}
