/*
    Copyright (C) 2024, The AROS Development Team.
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

struct MUI_CustomClass *mcc_Button;
LONG touchxin, touchyin, touchxout, touchyout;

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

    return (IPTR)obj;
}

static IPTR mCreateDragImage(struct IClass *cl, Object *obj, struct MUIP_CreateDragImage *msg)
{
    touchxin = msg->touchx;
    touchyin = msg->touchy;
    IPTR ret = DoSuperMethodA(cl, obj, (Msg)msg);
    struct MUI_DragImage *img = (struct MUI_DragImage *)ret;
    touchxout = img->touchx;
    touchyout = img->touchy;
    return ret;
}

BOOPSI_DISPATCHER(IPTR, dispatcher, cl, obj, msg)
{
    switch (msg->MethodID)
    {
        case OM_NEW: return mOM_NEW(cl, obj, (struct opSet *)msg);
        case MUIM_CreateDragImage: return mCreateDragImage(cl, obj, (APTR)msg);
    }
    return DoSuperMethodA(cl, obj, msg);
}
BOOPSI_DISPATCHER_END

CU_SUITE_SETUP()
{
    MUIMasterBase = OpenLibrary((STRPTR)MUIMASTER_NAME, 0);
    if (!MUIMasterBase)
        CUE_SINIT_FAILED;

    mcc_Button = MUI_CreateCustomClass(NULL, MUIC_Text, NULL, sizeof(struct Data), dispatcher);

    return CUE_SUCCESS;
}

CU_SUITE_TEARDOWN()
{
    MUI_DeleteCustomClass(mcc_Button);
    CloseLibrary(MUIMasterBase);
    return CUE_SUCCESS;
}

static void test_dragdrop_touch_is_positive_when_clicked_inside_object()
{
    Object *wnd;
    Object *app;
    Object *str1, *btn2;
#if defined(__AROS__)
    const LONG ycoord = 50;
#else
    const LONG ycoord = 35;
#endif

    btn2 = NewObject(mcc_Button->mcc_Class, NULL,
                MUIA_Text_Contents, "BTN2",
                MUIA_Draggable, TRUE,
                0x01010101, TRUE,
                TAG_DONE);

    app = ApplicationObject,
        SubWindow, wnd = WindowObject,
            MUIA_Window_Activate, TRUE,
            WindowContents, VGroup,
                GroupFrame,
                Child, str1 = StringObject, MUIA_String_Contents, "EMPTY", MUIA_Dropable, TRUE, End,
                Child, btn2,
            End,
        End,
    End;

    if (app)
    {
        ULONG sigs = 0;

        set(wnd,MUIA_Window_Open,TRUE);

        struct Window *w = _window(btn2);

        for (int j = 0; j < 3; j++)
        {
            switch(j)
            {
            case 0:
                CU_ASSERT_EQUAL(0, touchxin);
                CU_ASSERT_EQUAL(0, touchyin);
                CU_ASSERT_EQUAL(0, touchxout);
                CU_ASSERT_EQUAL(0, touchyout);
                break;
            case 1:
                DragFromTo(w, 30, ycoord, 40, ycoord);
                break;
            case 2:
                break;
            }

            sigs = 0;
            do {
                DoMethod(app, MUIM_Application_NewInput, &sigs);
            } while (sigs == 0);
            Delay(2);
        }

        MUI_DisposeObject(app);

        CU_ASSERT(touchxin > 0);
        CU_ASSERT(touchyin > 0);
        CU_ASSERT(touchxout > 0);
        CU_ASSERT(touchyout > 0);

#if defined(__AROS__)
        CU_ASSERT(touchxin == touchxout);
        CU_ASSERT(touchyin == touchyout);
#else
        // MUI 3.9
        CU_ASSERT(touchxin == touchxout - 2);
        CU_ASSERT(touchyin == touchyout - 2);
#endif
    }
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(MUIM_DragDrop_Suite, __cu_suite_setup, __cu_suite_teardown, NULL, NULL);
    CUNIT_CI_TEST(test_dragdrop_touch_is_positive_when_clicked_inside_object);
    return CU_CI_RUN_SUITES();
}
