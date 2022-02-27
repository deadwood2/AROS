/*
    Copyright (C) 2022, The AROS Development Team. All rights reserved.
*/

#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#define CUNIT_ABSOLUTE_PATH "SYS:Development/Debug/Tests/cunit"

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

static inline void Click(struct Window *w, LONG x, LONG y)
{
    struct IntuiMessage *imsg = NULL;

    imsg = _AllocIntuiMessage(w);
    imsg->Class = IDCMP_MOUSEBUTTONS;
    imsg->Code = SELECTDOWN;
    imsg->MouseX = (WORD)x;
    imsg->MouseY = (WORD)y;
    _SendIntuiMessage(w, imsg);

    imsg = _AllocIntuiMessage(w);
    imsg->Class = IDCMP_MOUSEBUTTONS;
    imsg->Code = SELECTUP;
    imsg->MouseX = (WORD)x;
    imsg->MouseY = (WORD)y;
    _SendIntuiMessage(w, imsg);

}

#if !defined(__AROS__)

#define REG(r, x)   x __asm(#r)
#define SAVEDS      __saveds
#define STDARGS     __stdargs
#define INTERRUPT   __interrupt
#define IPTR        ULONG

#define BOOPSI_DISPATCHER(type, func, cl, obj, msg) \
static ULONG SAVEDS func(REG(a0, struct IClass *cl), \
        REG(a2, Object *obj), REG(a1, Msg msg))

#define BOOPSI_DISPATCHER_END

#ifndef MUIM_Window_Setup
#define MUIM_Window_Setup         0x8042c34c /* Custom Class */ /* V18 */
#define MUIM_Window_Cleanup       0x8042ab26 /* Custom Class */ /* V18 */
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

#define CU_ASSERT_EQUAL(expected, actual) \
    CU_ASSERT(expected == actual)

#define CU_ASSERT(expr)                                 \
    if (!(expr))                                        \
    {                                                   \
        CONST_STRPTR f = __FILE__;                      \
        ULONG _tags[] = { (ULONG)f, __LINE__};          \
        VPrintf("Assertion failed %s:%ld\n", _tags);    \
    }

#define CU_CI_DEFINE_SUITE(...) \
    __cu_suite_setup();         \

#define CUNIT_CI_TEST(func)                 \
    __cu_test_setup();                      \
    {                                       \
        CONST_STRPTR f = #func;             \
        ULONG _tags[] = { (ULONG) f};       \
        VPrintf("Running: %s\n", _tags);    \
    }                                       \
    func();                                 \
    __cu_test_teardown();                   \

#define CU_CI_RUN_SUITES()      \
    __cu_suite_teardown();      \

#endif

#endif
