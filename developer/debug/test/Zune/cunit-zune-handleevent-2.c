/*
    Copyright (C) 2022, The AROS Development Team.
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
    struct MUI_EventHandlerNode ehnode;
};

struct MUI_CustomClass *mcc_Button;

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
    struct Data *data = NULL;
    struct TagItem *tags, *tag;
    struct TagItem tagssuper[] =
    {
        { MUIA_Font, MUIV_Font_Button } ,
        { MUIA_Text_PreParse, (IPTR)"\33c" } ,
        { MUIA_InputMode    , MUIV_InputMode_RelVerify },
        { MUIA_Background   , MUII_ButtonBack },
        { MUIA_CycleChain,    1 },
        { MUIA_Frame, MUIV_Frame_Button },
        { TAG_MORE, 0}
    };
    tagssuper[6].ti_Data = (IPTR)msg->ops_AttrList;
    msg->ops_AttrList = tagssuper;

    obj = (Object *)DoSuperMethodA(cl, obj, (Msg)msg);

    data = INST_DATA(cl, obj);
    data->ehnode.ehn_Flags = 0;

    for (tags = msg->ops_AttrList; (tag = NextTagItem(&tags));)
    {
        switch (tag->ti_Tag)
        {
        case 0x01010101:
            data->ehnode.ehn_Flags = MUI_EHF_GUIMODE;
            break;
        }
    }

    return (IPTR)obj;
}

static IPTR mSetup(struct IClass *cl, Object *obj, Msg msg)
{
    struct Data *data = INST_DATA(cl, obj);

    if (!DoSuperMethodA(cl, obj, msg))
    {
        return FALSE;
    }

    data->ehnode.ehn_Object = obj;
    data->ehnode.ehn_Class  = cl;
    data->ehnode.ehn_Events = IDCMP_MOUSEBUTTONS;
    data->ehnode.ehn_Priority = -20; /* Priority below built-in classes */
    DoMethod(_win(obj), MUIM_Window_AddEventHandler, &data->ehnode);

    return TRUE;
}

static IPTR mCleanup(struct IClass *cl, Object *obj, Msg msg)
{
    struct Data *data = INST_DATA(cl, obj);

    DoMethod(_win(obj), MUIM_Window_RemEventHandler, &data->ehnode);
    return DoSuperMethodA(cl, obj, msg);
}

static IPTR mHandleEvent(struct IClass *cl, Object *obj, struct  MUIP_HandleEvent *msg)
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
        case MUIM_HandleEvent:  return mHandleEvent(cl, obj, (APTR)msg);
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

    mcc_Button = MUI_CreateCustomClass(NULL, MUIC_Text, NULL, sizeof(struct Data), dispatcher);

    pressedhook.h_Entry = HookEntry;
    pressedhook.h_SubEntry = (HOOKFUNC)pressedfunction;

    return CUE_SUCCESS;
}

CU_SUITE_TEARDOWN()
{
    MUI_DeleteCustomClass(mcc_Button);
    CloseLibrary(MUIMasterBase);
    return CUE_SUCCESS;
}

static void globalReset()
{
    global_Pressed      =   0;
    global_EventDown    =   0;
    global_EventUp      =   0;
}

/* While invisible objects will be receive events, they should only be handled
   by currently visibile ones. This test creates two buttons on two tabs. At
   any point in time only one button is visible and it should be the one that
   is "clicked" */

static void test_handleevent_objects_samelocation()
{
    Object *wnd;
    Object *app;
    Object *firstButton, *secondButton;
    CONST_STRPTR Tabs[] = { "First", "Second", NULL };

#if defined(__AROS__)
const LONG xcoord[] = { 67, 50, 67, 67, 50};
const LONG ycoord[] = { 70, 70, 30, 70, 70};
#else
const LONG xcoord[] = { 55, 30, 70, 55, 30};
const LONG ycoord[] = { 40, 40, 20, 40, 40};
#endif

    firstButton = NewObject(mcc_Button->mcc_Class, NULL,
                MUIA_Text_Contents, "First",
                TAG_DONE);


    app = ApplicationObject,
        SubWindow, wnd = WindowObject,
            MUIA_Window_Activate, TRUE,
            WindowContents, RegisterGroup((IPTR)Tabs),
                Child, (IPTR)VGroup,
                    Child, (IPTR)HGroup,
                        GroupFrame,
                        Child, (IPTR)(RectangleObject, End),
                        Child, (IPTR)(firstButton),
                    End,
                End,
                Child, (IPTR)VGroup,
                    Child, (IPTR)HGroup,
                        GroupFrame,
                        Child, (IPTR)(RectangleObject, End),
                        Child, (IPTR)(secondButton = MUI_MakeObject(MUIO_Button,(IPTR)("Second"))),
                    End,
                End,
            End,
        End,
    End;

    if (app)
    {
        ULONG sigs = 0;

        DoMethod
        (
            firstButton, MUIM_Notify, MUIA_Pressed, FALSE,
            (IPTR)firstButton, 3, MUIM_CallHook, &pressedhook, 7
        );

        DoMethod
        (
            secondButton, MUIM_Notify, MUIA_Pressed, FALSE,
            (IPTR)firstButton, 3, MUIM_CallHook, &pressedhook, 4
        );

        set(wnd,MUIA_Window_Open,TRUE);

        struct Window *w = _window(firstButton);

        for (int j = 0; j < 5; j++) {

            EVENT_LOOP

            switch(j)
            {
            case 0:
                CU_ASSERT(global_Pressed == 7);
                break;
            case 1:
                CU_ASSERT(global_Pressed == 0);
                break;
            case 2:
                CU_ASSERT(global_Pressed == 0);
                break;
            case 3:
                CU_ASSERT(global_Pressed == 4);
                break;
            case 4:
                CU_ASSERT(global_Pressed == 4);
                break;
            }
        }

        MUI_DisposeObject(app);
    }
}

static void test_handleevent_hidden_button_is_not_pressed()
{
    Object *wnd;
    Object *app;
    Object *btn1, *btn2;
    const LONG xcoord[] = { 80, 0, 0, 80, 0};
#if defined(__AROS__)
    const LONG ycoord[] = { 35, 35, 0, 35, 35};
#else
    const LONG ycoord[] = { 20, 20, 0, 20, 20};
#endif

    app = ApplicationObject,
        SubWindow, wnd = WindowObject,
            MUIA_Window_Activate, TRUE,
            WindowContents, HGroup,
                GroupFrame,
                Child, btn1 = MUI_MakeObject(MUIO_Button,(IPTR)"BTN1"),
                Child, btn2 = MUI_MakeObject(MUIO_Button,(IPTR)"BTN2"),
            End,
        End,
    End;

    if (app)
    {
        ULONG sigs = 0;

        DoMethod
        (
            btn1, MUIM_Notify, MUIA_Pressed, FALSE,
            (IPTR)btn1, 3, MUIM_CallHook, &pressedhook, 5
        );

        DoMethod
        (
            btn2, MUIM_Notify, MUIA_Pressed, FALSE,
            (IPTR)btn2, 3, MUIM_CallHook, &pressedhook, 8
        );

        set(wnd,MUIA_Window_Open,TRUE);

        struct Window *w = _window(btn2);

        for (int j = 0; j < 5; j++) {

            EVENT_LOOP

            switch(j)
            {
            case 0:
                CU_ASSERT(global_Pressed == 8);
                break;
            case 1:
                CU_ASSERT(global_Pressed == 0);
                set(btn2, MUIA_ShowMe, FALSE);
                break;
            case 2:
                CU_ASSERT(global_Pressed == 0);
                break;
            case 3:
                CU_ASSERT(global_Pressed == 5);
                break;
            case 4:
                CU_ASSERT(global_Pressed == 0);
                break;
            }
        }

        MUI_DisposeObject(app);
    }
}

/* Notes:
    MUI_EHF_GUIMODE is only starting from MUI 3.9
    MUI 3.8 FAIL
    MUI 3.9 PASS
*/
static void test_handleevent_hidden_object_ehf_guimode_does_not_receive_events()
{
    Object *wnd;
    Object *app;
    Object *btn1, *btn2;
    const LONG xcoord[] = { 80, 0, 0, 80, 0};
#if defined(__AROS__)
    const LONG ycoord[] = { 35, 35, 0, 35, 35};
#else
    const LONG ycoord[] = { 20, 20, 0, 20, 20};
#endif

    btn2 = NewObject(mcc_Button->mcc_Class, NULL,
                MUIA_Text_Contents, "BTN2",
                0x01010101, TRUE,
                TAG_DONE);

    app = ApplicationObject,
        SubWindow, wnd = WindowObject,
            MUIA_Window_Activate, TRUE,
            WindowContents, HGroup,
                GroupFrame,
                Child, btn1 = MUI_MakeObject(MUIO_Button,(IPTR)"BTN1"),
                Child, btn2,
            End,
        End,
    End;

    if (app)
    {
        ULONG sigs = 0;

        set(wnd,MUIA_Window_Open,TRUE);

        struct Window *w = _window(btn2);

        for (int j = 0; j < 5; j++) {

            EVENT_LOOP

            switch(j)
            {
            case 0:
                CU_ASSERT(global_EventDown  == 0);
                CU_ASSERT(global_EventUp    == 1);
                break;
            case 1:
                CU_ASSERT(global_EventDown  == 1);
                CU_ASSERT(global_EventUp    == 1);
                set(btn2, MUIA_ShowMe, FALSE);
                break;
            case 2:
                CU_ASSERT(global_EventDown  == 0);
                CU_ASSERT(global_EventUp    == 0);
                break;
            case 3:
                CU_ASSERT(global_EventDown  == 0);
                CU_ASSERT(global_EventUp    == 0);
                break;
            case 4:
                CU_ASSERT(global_EventDown  == 0);
                CU_ASSERT(global_EventUp    == 0);
                break;
            }
        }

        MUI_DisposeObject(app);
    }
}

/* An event handler with priority lower than built-in classes will
   receive mouse button selectup events even if processes by other objects */
static void test_handleevent_area_does_not_eat_selectup_event()
{
    Object *wnd;
    Object *app;
    Object *aButton, *anOrdButton;
    const LONG xcoord[] = { 30, 65, 0, 30, 65};
#if defined(__AROS__)
    const LONG ycoord[] = { 35, 35, 0, 35, 35};
#else
    const LONG ycoord[] = { 20, 20, 0, 20, 20};
#endif


    aButton = NewObject(mcc_Button->mcc_Class, NULL,
                MUIA_Text_Contents, "BTN2",
                TAG_DONE);


    app = ApplicationObject,
        SubWindow, wnd = WindowObject,
            MUIA_Window_Activate, TRUE,
            WindowContents, HGroup,
                GroupFrame,
                Child, RectangleObject,
                    MUIA_FixWidthTxt, "BTN0",
                    MUIA_FixHeightTxt, "BTN0",
                    MUIA_Rectangle_HBar, TRUE,
                End,
                Child, anOrdButton = MUI_MakeObject(MUIO_Button,(IPTR)"BTN1"),
                Child, aButton,
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

        struct Window *w = _window(aButton);

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
                CU_ASSERT(global_EventDown  == 0);
                CU_ASSERT(global_EventUp    == 1);
                CU_ASSERT(global_Pressed    == 7);
                set(aButton, MUIA_ShowMe, FALSE);
                break;
            case 2:
                CU_ASSERT(global_EventDown  == 1);
                CU_ASSERT(global_EventUp    == 1);
                CU_ASSERT(global_Pressed    == 0);
                break;
            case 3:
                CU_ASSERT(global_EventDown  == 1);
                CU_ASSERT(global_EventUp    == 1);
                CU_ASSERT(global_Pressed    == 0);
                break;
            case 4:
                CU_ASSERT(global_EventDown  == 0);
                CU_ASSERT(global_EventUp    == 1);
                CU_ASSERT(global_Pressed    == 7);
                break;
            }
        }

        MUI_DisposeObject(app);
    }
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(MUIM_HandleEvent_2_Suite, __cu_suite_setup, __cu_suite_teardown, NULL, NULL);
    CUNIT_CI_TEST(test_handleevent_area_does_not_eat_selectup_event);
    CUNIT_CI_TEST(test_handleevent_hidden_button_is_not_pressed);
    CUNIT_CI_TEST(test_handleevent_objects_samelocation);
    CUNIT_CI_TEST(test_handleevent_hidden_object_ehf_guimode_does_not_receive_events);
    return CU_CI_RUN_SUITES();
}
