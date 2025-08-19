/*
    Copyright (C) 2022, The AROS Development Team. All rights reserved.
*/

#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#define CUNIT_ABSOLUTE_PATH "SYS:Development/Debug/Tests/cunit"

#if defined(ENABLE_CLICK)
static inline struct IntuiMessage * _AllocIntuiMessage(struct Window *w)
{
    struct IntuiMessage *imsg = AllocMem(sizeof(struct IntuiMessage), MEMF_PUBLIC | MEMF_CLEAR);
    imsg->ExecMessage.mn_Node.ln_Type = NT_MESSAGE;
    imsg->ExecMessage.mn_Length       = sizeof(struct IntuiMessage);
    imsg->IDCMPWindow = w;
    return imsg;
}

static inline void _SendIntuiMessage(struct Window *w, struct IntuiMessage *imsg)
{
    PutMsg(w->UserPort, &imsg->ExecMessage);
}

static inline void MouseMoveTo(struct Window *w, LONG x, LONG y, BOOL selpressed)
{
    struct IntuiMessage *imsg = NULL;

    imsg = _AllocIntuiMessage(w);
    imsg->Class = IDCMP_MOUSEMOVE;
    imsg->Code = IECODE_NOBUTTON;
    imsg->MouseX = (WORD)x;
    imsg->MouseY = (WORD)y;
    imsg->IAddress = w;
    if (selpressed) imsg->Qualifier |= IEQUALIFIER_LEFTBUTTON;
    _SendIntuiMessage(w, imsg);
}

static inline void MouseLButtonPress(struct Window *w, LONG x, LONG y)
{
    struct IntuiMessage *imsg = NULL;

    imsg = _AllocIntuiMessage(w);
    imsg->Class = IDCMP_MOUSEBUTTONS;
    imsg->Code = SELECTDOWN;
    imsg->MouseX = (WORD)x;
    imsg->MouseY = (WORD)y;
    imsg->IAddress = w;
    _SendIntuiMessage(w, imsg);
}

static inline void MouseLButtonRelease(struct Window *w, LONG x, LONG y)
{
    struct IntuiMessage *imsg = NULL;

    imsg = _AllocIntuiMessage(w);
    imsg->Class = IDCMP_MOUSEBUTTONS;
    imsg->Code = SELECTUP;
    imsg->MouseX = (WORD)x;
    imsg->MouseY = (WORD)y;
    imsg->IAddress = w;
    _SendIntuiMessage(w, imsg);
}

static inline void Click(struct Window *w, LONG x, LONG y)
{
    MouseMoveTo(w, x, y, FALSE);

    MouseLButtonPress(w, x, y);

    MouseLButtonRelease(w, x, y);
}

static inline void DragFromTo(struct Window *w, LONG fromx, LONG fromy, LONG tox, LONG toy)
{
    MouseMoveTo(w, fromx, fromy, FALSE);

    MouseLButtonPress(w, fromx, fromy);

    MouseMoveTo(w, tox, toy, TRUE);

    MouseLButtonRelease(w, tox, toy);

}
#endif

#if !defined(__AROS__)

#include <string.h>

#define REG(r, x)   x __asm(#r)
#define SAVEDS      __saveds
#define STDARGS     __stdargs
#define INTERRUPT   __interrupt
#define IPTR        ULONG
#define BNULL       (0)

#define BOOPSI_DISPATCHER(type, func, cl, obj, msg) \
static ULONG SAVEDS func(REG(a0, struct IClass *cl), \
        REG(a2, Object *obj), REG(a1, Msg msg))

#define BOOPSI_DISPATCHER_END

#ifndef MUIM_Window_Setup
#define MUIM_Window_Setup         0x8042c34c /* Custom Class */ /* V18 */
#define MUIM_Window_Cleanup       0x8042ab26 /* Custom Class */ /* V18 */
#endif

#ifndef MUI_EHF_GUIMODE
#define MUI_EHF_GUIMODE (1 << 1)
#endif

#define XGET(object, attribute)                 \
({                                              \
    IPTR __storage = 0;                         \
    GetAttr((attribute), (object), &__storage); \
    __storage;                                  \
})

#define CU_SUITE_SETUP      static int __cu_suite_setup
#define CU_SUITE_TEARDOWN   static int __cu_suite_teardown
#define CU_TEST_SETUP       static void __cu_test_setup
#define CU_TEST_TEARDOWN    static void __cu_test_teardown

#define CUE_SUCCESS         0
#define CUE_SINIT_FAILED    22

#define CU_ASSERT_STRING_EQUAL(expected, actual) \
    CU_ASSERT(strcmp(expected, actual) == 0)

#define CU_ASSERT_EQUAL(expected, actual) \
    CU_ASSERT(expected == actual)

#define CU_ASSERT_NOT_EQUAL(expected, actual) \
    CU_ASSERT(expected != actual)

#define CU_ASSERT_NOT_EQUAL_FATAL(expected, actual) \
    CU_ASSERT_FATAL(expected != actual)

#define CU_ASSERT_PTR_NOT_NULL_FATAL(actual) \
    CU_ASSERT_FATAL(NULL != actual)

#define CU_ASSERT(expr)                                 \
    if (!(expr))                                        \
    {                                                   \
        CONST_STRPTR f = __FILE__;                      \
        ULONG _tags[] = { (ULONG)f, __LINE__};          \
        VPrintf("Assertion failed %s:%ld\n", _tags);    \
    }

#define CU_ASSERT_FATAL(expr)                           \
    if (!(expr))                                        \
    {                                                   \
        CONST_STRPTR f = __FILE__;                      \
        ULONG _tags[] = { (ULONG)f, __LINE__};          \
        VPrintf("Fatal assertion failed %s:%ld\n", _tags);  \
        return;                                         \
    }

#define CU_FAIL(msg)                                        \
    {                                                       \
        CONST_STRPTR f = __FILE__;                          \
        CONST_STRPTR m = #msg;                              \
        ULONG _tags[] = { (ULONG)m, (ULONG)f, __LINE__};    \
        VPrintf("Test failed %s %s:%ld\n", _tags);          \
    }

#define CU_CI_DEFINE_SUITE(name, ssetup, stearndown, tsetup, tteardown) \
    int  (*_p_suite_setup)()    = ssetup;       \
    int  (*_p_suite_teardown)() = stearndown;   \
    void (*_p_test_setup)()     = tsetup;       \
    void (*_p_test_teardown)()  = tteardown;    \
    if (_p_suite_setup) _p_suite_setup();       \

#define CUNIT_CI_TEST(func)                     \
    if (_p_test_setup) _p_test_setup();         \
    {                                           \
        CONST_STRPTR f = #func;                 \
        ULONG _tags[] = { (ULONG) f};           \
        VPrintf("Running: %s\n", _tags);        \
    }                                           \
    func();                                     \
    if (_p_test_teardown) _p_test_teardown();   \

#define CU_CI_RUN_SUITES()      \
    ({int _ret = 0; if (_p_suite_teardown) _p_suite_teardown(); _ret;}) \

#endif

#define BASIC_EVENT_LOOP \
    ULONG sigs; \
    DoMethod(wnd, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, (IPTR) app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit); \
    while (DoMethod(app, MUIM_Application_NewInput, (IPTR) &sigs) != MUIV_Application_ReturnID_Quit); \


#endif
