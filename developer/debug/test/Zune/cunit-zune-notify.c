/*
    Copyright (C) 2023, The AROS Development Team.
    All rights reserved.
*/

#define MUI_OBSOLETE

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

#define MUIA_My_Custom (TAG_USER | 0x00556677)

struct MyData
{
    LONG val;
};

struct MUI_CustomClass *mcc_Notify;
struct MUI_CustomClass *mcc_Area;

IPTR mOM_NEW(struct IClass *cl, Object *obj, struct opSet *msg)
{
    obj = (Object *)DoSuperMethodA(cl, obj, (Msg)msg);

    return (IPTR)obj;
}

IPTR mOM_SET(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct MyData *data = INST_DATA(cl, obj);
    struct TagItem *tags = msg->ops_AttrList;
    struct TagItem *tag;

    while ((tag = NextTagItem(&tags)) != NULL)
    {
        switch (tag->ti_Tag)
        {
        case MUIA_My_Custom:
            data->val = (LONG)tag->ti_Data;
            break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

static IPTR mOM_GET(struct IClass *cl, Object *obj, struct opGet *msg)
{
#define STORE *(msg->opg_Storage)

    struct MyData *data = INST_DATA(cl, obj);

    switch (msg->opg_AttrID)
    {
    case MUIA_My_Custom:
        STORE = (IPTR) data->val;
        return TRUE;
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

BOOPSI_DISPATCHER(IPTR, dispatcher, cl, obj, msg)
{
    switch (msg->MethodID)
    {
        case OM_NEW: return mOM_NEW(cl, obj, (struct opSet *)msg);
        case OM_SET: return mOM_SET(cl, obj, (struct opSet *)msg);
        case OM_GET: return mOM_GET(cl, obj, (struct opGet *)msg);
    }
    return DoSuperMethodA(cl, obj, msg);
}
BOOPSI_DISPATCHER_END

static struct Hook notifyhook;

LONG global_NotifyHookVal = 0;

static void notifyfunction(struct Hook *hook, Object *obj, APTR msg)
{
    LONG val = *(LONG *)msg;
    global_NotifyHookVal += val;
}

CU_SUITE_SETUP()
{
    MUIMasterBase = OpenLibrary((STRPTR)MUIMASTER_NAME, 0);
    if (!MUIMasterBase)
        CUE_SINIT_FAILED;

    mcc_Notify  = MUI_CreateCustomClass(NULL, MUIC_Notify, NULL, sizeof(struct MyData), dispatcher);
    mcc_Area    = MUI_CreateCustomClass(NULL, MUIC_Area, NULL, sizeof(struct MyData), dispatcher);

    notifyhook.h_Entry = HookEntry;
    notifyhook.h_SubEntry = (HOOKFUNC)notifyfunction;

    return CUE_SUCCESS;
}

CU_SUITE_TEARDOWN()
{
    MUI_DeleteCustomClass(mcc_Notify);
    MUI_DeleteCustomClass(mcc_Area);
    CloseLibrary(MUIMasterBase);
    return CUE_SUCCESS;
}

static void globalReset()
{
    global_NotifyHookVal  =   0;
}

static void test_basic_functionality()
{
    Object *wnd;
    Object *app;
    Object *lvl1, *lvl2;

    app = ApplicationObject,
        SubWindow, wnd = WindowObject,
            MUIA_Window_Activate, TRUE,
            WindowContents, HGroup,
                GroupFrame,
                Child, lvl1 = StringObject,
                    MUIA_String_Integer,    1,
                End,
                Child, lvl2 = StringObject,
                    MUIA_String_Integer,    2,
                End,
            End,
        End,
    End;

    if (app)
    {
        IPTR tmp;

        DoMethod
        (
            lvl1, MUIM_Notify, MUIA_String_Integer, MUIV_EveryTime,
            (IPTR)lvl1, 3, MUIM_CallHook, &notifyhook, 7
        );

        DoMethod
        (
            lvl1, MUIM_Notify, MUIA_String_Integer, MUIV_EveryTime,
            (IPTR)lvl2, 3, MUIM_NoNotifySet, MUIA_String_Integer, MUIV_TriggerValue
        );

        set(wnd,MUIA_Window_Open,TRUE);
        globalReset();


        get(lvl1, MUIA_String_Integer, &tmp);
        CU_ASSERT_EQUAL(1, tmp);
        get(lvl2, MUIA_String_Integer, &tmp);
        CU_ASSERT_EQUAL(2, tmp);


        set(lvl1, MUIA_String_Integer, 4);


        tmp = 0; get(lvl1, MUIA_String_Integer, &tmp);
        CU_ASSERT_EQUAL(4, tmp);
        tmp = 0; get(lvl2, MUIA_String_Integer, &tmp);
        CU_ASSERT_EQUAL(4, tmp);
        CU_ASSERT_EQUAL(7, global_NotifyHookVal);

        set(wnd, MUIA_Window_Open, FALSE);
        MUI_DisposeObject(app);
    }
    else
    {
        CU_ASSERT(0);
    }
}

static void test_cascade_of_notifies()
{
    Object *wnd;
    Object *app;
    Object *lvl1, *lvl2a, *lvl2b, *lvl3a, *lvl3b;

    app = ApplicationObject,
        SubWindow, wnd = WindowObject,
            MUIA_Window_Activate, TRUE,
            WindowContents, HGroup,
                GroupFrame,
                Child, lvl1 = StringObject,
                    MUIA_String_Integer,    1,
                End,
                Child, lvl2a = StringObject,
                    MUIA_String_Integer,    2,
                End,
                Child, lvl2b = StringObject,
                    MUIA_String_Integer,    3,
                End,
                Child, lvl3a = StringObject,
                    MUIA_String_Integer,    4,
                End,
                Child, lvl3b = StringObject,
                    MUIA_String_Integer,    5,
                End,
            End,
        End,
    End;

    if (app)
    {
        IPTR tmp;

        DoMethod
        (
            lvl1, MUIM_Notify, MUIA_String_Integer, MUIV_EveryTime,
            (IPTR)lvl2a, 3, MUIM_NoNotifySet, MUIA_String_Integer, MUIV_TriggerValue
        );

        DoMethod
        (
            lvl2a, MUIM_Notify, MUIA_String_Integer, MUIV_EveryTime,
            (IPTR)lvl3a, 3, MUIM_Set, MUIA_String_Integer, MUIV_TriggerValue
        );

        DoMethod
        (
            lvl1, MUIM_Notify, MUIA_String_Integer, MUIV_EveryTime,
            (IPTR)lvl2b, 3, MUIM_Set, MUIA_String_Integer, MUIV_TriggerValue
        );

        DoMethod
        (
            lvl2b, MUIM_Notify, MUIA_String_Integer, MUIV_EveryTime,
            (IPTR)lvl3b, 3, MUIM_Set, MUIA_String_Integer, MUIV_TriggerValue
        );

        set(wnd,MUIA_Window_Open,TRUE);
        globalReset();


        get(lvl1, MUIA_String_Integer, &tmp);
        CU_ASSERT_EQUAL(1, tmp);
        get(lvl2a, MUIA_String_Integer, &tmp);
        CU_ASSERT_EQUAL(2, tmp);
        get(lvl2b, MUIA_String_Integer, &tmp);
        CU_ASSERT_EQUAL(3, tmp);
        get(lvl3a, MUIA_String_Integer, &tmp);
        CU_ASSERT_EQUAL(4, tmp);
        get(lvl3b, MUIA_String_Integer, &tmp);
        CU_ASSERT_EQUAL(5, tmp);


        /* Check mid-level notification works */
        set(lvl2a, MUIA_String_Integer, 20);
        tmp = 0; get(lvl2a, MUIA_String_Integer, &tmp);
        CU_ASSERT_EQUAL(20, tmp);
        tmp = 0; get(lvl3a, MUIA_String_Integer, &tmp);
        CU_ASSERT_EQUAL(20, tmp);


        /* Check top-level notification */
        set(lvl1, MUIA_String_Integer, 10);
        tmp = 0; get(lvl1, MUIA_String_Integer, &tmp);
        CU_ASSERT_EQUAL(10, tmp);
        tmp = 0; get(lvl2a, MUIA_String_Integer, &tmp);
        CU_ASSERT_EQUAL(10, tmp);
        tmp = 0; get(lvl2b, MUIA_String_Integer, &tmp);
        CU_ASSERT_EQUAL(10, tmp);
        tmp = 0; get(lvl3a, MUIA_String_Integer, &tmp);
        CU_ASSERT_EQUAL(20, tmp);
        tmp = 0; get(lvl3b, MUIA_String_Integer, &tmp);
        CU_ASSERT_EQUAL(10, tmp);

        set(wnd, MUIA_Window_Open, FALSE);
        MUI_DisposeObject(app);
    }
    else
    {
        CU_ASSERT(0);
    }
}

static void test_string_integer_does_not_notify_on_same_value_when_everytime()
{
    Object *wnd;
    Object *app;
    Object *lvl1;

    app = ApplicationObject,
        SubWindow, wnd = WindowObject,
            MUIA_Window_Activate, TRUE,
            WindowContents, HGroup,
                GroupFrame,
                Child, lvl1 = StringObject,
                    MUIA_String_Integer,    1,
                End,
            End,
        End,
    End;

    if (app)
    {
        IPTR tmp;

        DoMethod
        (
            lvl1, MUIM_Notify, MUIA_String_Integer, MUIV_EveryTime,
            (IPTR)lvl1, 3, MUIM_CallHook, &notifyhook, MUIV_TriggerValue
        );

        set(wnd,MUIA_Window_Open,TRUE);
        globalReset();

        get(lvl1, MUIA_String_Integer, &tmp);
        CU_ASSERT_EQUAL(1, tmp);

        /* Set value several times */
        set(lvl1, MUIA_String_Integer, 10);
        CU_ASSERT_EQUAL(10, global_NotifyHookVal);

        set(lvl1, MUIA_String_Integer, 15);
        CU_ASSERT_EQUAL(25, global_NotifyHookVal);

        set(lvl1, MUIA_String_Integer, 15);
        CU_ASSERT_EQUAL(25, global_NotifyHookVal);

        set(lvl1, MUIA_String_Integer, 20);
        CU_ASSERT_EQUAL(45, global_NotifyHookVal);

        set(wnd, MUIA_Window_Open, FALSE);
        MUI_DisposeObject(app);
    }
    else
    {
        CU_ASSERT(0);
    }
}

static void test_string_integer_does_not_notify_on_same_value_when_defined_value()
{
    Object *wnd;
    Object *app;
    Object *lvl1;

    app = ApplicationObject,
        SubWindow, wnd = WindowObject,
            MUIA_Window_Activate, TRUE,
            WindowContents, HGroup,
                GroupFrame,
                Child, lvl1 = StringObject,
                    MUIA_String_Integer,    1,
                End,
            End,
        End,
    End;

    if (app)
    {
        IPTR tmp;

        DoMethod
        (
            lvl1, MUIM_Notify, MUIA_String_Integer, 15,
            (IPTR)lvl1, 3, MUIM_CallHook, &notifyhook, 8
        );

        set(wnd,MUIA_Window_Open,TRUE);
        globalReset();

        get(lvl1, MUIA_String_Integer, &tmp);
        CU_ASSERT_EQUAL(1, tmp);

        /* Set value several times */
        set(lvl1, MUIA_String_Integer, 10);
        CU_ASSERT_EQUAL(0, global_NotifyHookVal);

        set(lvl1, MUIA_String_Integer, 15);
        CU_ASSERT_EQUAL(8, global_NotifyHookVal);

        set(lvl1, MUIA_String_Integer, 15);
        CU_ASSERT_EQUAL(8, global_NotifyHookVal);

        set(lvl1, MUIA_String_Integer, 20);
        CU_ASSERT_EQUAL(8, global_NotifyHookVal);

        set(lvl1, MUIA_String_Integer, 15);
        CU_ASSERT_EQUAL(16, global_NotifyHookVal);

        set(wnd, MUIA_Window_Open, FALSE);
        MUI_DisposeObject(app);
    }
    else
    {
        CU_ASSERT(0);
    }
}

static void test_string_contents_does_not_notify_on_same_value()
{
    Object *wnd;
    Object *app;
    Object *lvl1;

    app = ApplicationObject,
        SubWindow, wnd = WindowObject,
            MUIA_Window_Activate, TRUE,
            WindowContents, HGroup,
                GroupFrame,
                Child, lvl1 = StringObject,
                    MUIA_String_Contents,   "RED",
                End,
            End,
        End,
    End;

    if (app)
    {
        IPTR tmp;
        TEXT s1[] = "RED";
        STRPTR s2 = (STRPTR)AllocMem(4, MEMF_CLEAR);
        s2[0] = 'R'; s2[1] = 'E'; s2[2] = 'D';

        DoMethod
        (
            lvl1, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
            (IPTR)lvl1, 3, MUIM_CallHook, &notifyhook, 8
        );

        set(wnd,MUIA_Window_Open,TRUE);


        /* Set same literal */
        globalReset();
        set(lvl1, MUIA_String_Contents, (IPTR)"RED");
        CU_ASSERT_EQUAL(0, global_NotifyHookVal);

        /* Set same value, different variable */
        globalReset();
        set(lvl1, MUIA_String_Contents, (IPTR)s1);
        CU_ASSERT_EQUAL(0, global_NotifyHookVal);

        /* Set same variable */
        globalReset();
        set(lvl1, MUIA_String_Contents, (IPTR)s1);
        CU_ASSERT_EQUAL(0, global_NotifyHookVal);

        /* Set same value, different variable */
        globalReset();
        set(lvl1, MUIA_String_Contents, (IPTR)s2);
        CU_ASSERT_EQUAL(0, global_NotifyHookVal);

        /* Different value */
        globalReset();
        set(lvl1, MUIA_String_Contents, (IPTR)"BLUE");
        CU_ASSERT_EQUAL(8, global_NotifyHookVal);

        set(wnd, MUIA_Window_Open, FALSE);
        MUI_DisposeObject(app);
        FreeMem(s2, 4);
    }
    else
    {
        CU_ASSERT(0);
    }
}

static void test_custom_class_notifies_on_same_value_set()
{
    Object *wnd;
    Object *app;
    Object *myNotify, *myArea;

    myNotify    = NewObject(mcc_Notify->mcc_Class, NULL, TAG_DONE);
    myArea      = NewObject(mcc_Area->mcc_Class, NULL, TAG_DONE);

    app = ApplicationObject,
        SubWindow, wnd = WindowObject,
            MUIA_Window_Activate, TRUE,
            WindowContents, HGroup,
                GroupFrame,
                Child, myNotify,
                Child, myArea,
            End,
        End,
    End;

    if (app)
    {
        IPTR tmp;

        DoMethod
        (
            myNotify, MUIM_Notify, MUIA_My_Custom, MUIV_EveryTime,
            (IPTR)myNotify, 3, MUIM_CallHook, &notifyhook, MUIV_TriggerValue
        );

        DoMethod
        (
            myArea, MUIM_Notify, MUIA_My_Custom, MUIV_EveryTime,
            (IPTR)myArea, 3, MUIM_CallHook, &notifyhook, MUIV_TriggerValue
        );

        set(wnd,MUIA_Window_Open,TRUE);

        /* Set value several times */
        globalReset();
        set(myNotify, MUIA_My_Custom, 10);
        CU_ASSERT_EQUAL(10, global_NotifyHookVal);

        set(myNotify, MUIA_My_Custom, 15);
        CU_ASSERT_EQUAL(25, global_NotifyHookVal);

        tmp = 0; get(myNotify, MUIA_My_Custom, &tmp);
        CU_ASSERT_EQUAL(15, tmp);

        set(myNotify, MUIA_My_Custom, 15);
        CU_ASSERT_EQUAL(40, global_NotifyHookVal);

        globalReset();

        /* Set value several times */
        globalReset();
        set(myArea, MUIA_My_Custom, 10);
        CU_ASSERT_EQUAL(10, global_NotifyHookVal);

        set(myArea, MUIA_My_Custom, 15);
        CU_ASSERT_EQUAL(25, global_NotifyHookVal);

        tmp = 0; get(myArea, MUIA_My_Custom, &tmp);
        CU_ASSERT_EQUAL(15, tmp);

        set(myArea, MUIA_My_Custom, 15);
        CU_ASSERT_EQUAL(40, global_NotifyHookVal);

        set(wnd, MUIA_Window_Open, FALSE);
        MUI_DisposeObject(app);
    }
    else
    {
        CU_ASSERT(0);
    }
}

static void test_cycle_active_does_not_notify_on_same_value_when_everytime()
{
    Object *wnd;
    Object *app;
    Object *cyc;
    CONST_STRPTR cyclestrings[] = { "Zero", "One", "Two", "Three", NULL };

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

        DoMethod
        (
            cyc, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
            (IPTR)cyc, 3, MUIM_CallHook, &notifyhook, MUIV_TriggerValue
        );

        set(wnd,MUIA_Window_Open,TRUE);
        globalReset();

        get(cyc, MUIA_Cycle_Active, &tmp);
        CU_ASSERT_EQUAL(0, tmp);

        /* Set value several times */
        set(cyc, MUIA_Cycle_Active, 1);
        CU_ASSERT_EQUAL(1, global_NotifyHookVal);

        set(cyc, MUIA_Cycle_Active, 2);
        CU_ASSERT_EQUAL(3, global_NotifyHookVal);

        set(cyc, MUIA_Cycle_Active, 2);
        CU_ASSERT_EQUAL(3, global_NotifyHookVal);

        set(cyc, MUIA_Cycle_Active, 3);
        CU_ASSERT_EQUAL(6, global_NotifyHookVal);

        set(wnd, MUIA_Window_Open, FALSE);
        MUI_DisposeObject(app);
    }
    else
    {
        CU_ASSERT(0);
    }
}

static void test_selected_does_not_notify_on_same_value_when_everytime()
{
    Object *wnd;
    Object *app;
    Object *cm;

    app = ApplicationObject,
        SubWindow, wnd = WindowObject,
            MUIA_Window_Activate, TRUE,
            WindowContents, HGroup,
                GroupFrame,
                Child, cm = CheckMark(FALSE),
                    Child, Label1("Checkmark"),
            End,
        End,
    End;

    if (app)
    {
        IPTR tmp;

        DoMethod
        (
            cm, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            (IPTR)cm, 3, MUIM_CallHook, &notifyhook, 15
        );

        set(wnd,MUIA_Window_Open,TRUE);
        globalReset();

        get(cm, MUIA_Selected, &tmp);
        CU_ASSERT_EQUAL(0, tmp);

        /* Set value several times */
        set(cm, MUIA_Selected, TRUE);
        CU_ASSERT_EQUAL(15, global_NotifyHookVal);

        set(cm, MUIA_Selected, TRUE);
        CU_ASSERT_EQUAL(15, global_NotifyHookVal);

        set(cm, MUIA_Selected, FALSE);
        CU_ASSERT_EQUAL(30, global_NotifyHookVal);

        set(cm, MUIA_Selected, FALSE);
        CU_ASSERT_EQUAL(30, global_NotifyHookVal);

        set(cm, MUIA_Selected, TRUE);
        CU_ASSERT_EQUAL(45, global_NotifyHookVal);

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
    CU_CI_DEFINE_SUITE(MUIM_Notify_Suite, __cu_suite_setup, __cu_suite_teardown, NULL, NULL);
    CUNIT_CI_TEST(test_basic_functionality);
    CUNIT_CI_TEST(test_cascade_of_notifies);
    CUNIT_CI_TEST(test_string_integer_does_not_notify_on_same_value_when_defined_value);
    CUNIT_CI_TEST(test_string_integer_does_not_notify_on_same_value_when_everytime);
    CUNIT_CI_TEST(test_string_contents_does_not_notify_on_same_value);
    CUNIT_CI_TEST(test_custom_class_notifies_on_same_value_set);
    CUNIT_CI_TEST(test_cycle_active_does_not_notify_on_same_value_when_everytime);
    CUNIT_CI_TEST(test_selected_does_not_notify_on_same_value_when_everytime);
    return CU_CI_RUN_SUITES();
}
