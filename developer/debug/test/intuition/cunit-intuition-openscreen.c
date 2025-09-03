/*
    Copyright © 2025, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/intuition.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/diskfont.h>
#include <clib/alib_protos.h>

#include "../test-util.h"

#if defined(__AROS__)
#include <CUnit/CUnitCI.h>
#endif

CU_SUITE_SETUP()
{
#ifdef __AROS__
    struct TextAttr ta = {"arial.font", 11, FSF_TAGGED, FPF_DISKFONT | FPF_PROPORTIONAL | FPF_DESIGNED};
    if (!OpenDiskFont(&ta))
        CUE_SINIT_FAILED;
#endif

    return CUE_SUCCESS;
}

static void test_openscreen_safont()
{
    /* Make sure these fonts are different then defaults (arial 13 and topaz 8)*/
#ifdef __AROS__
    struct TextAttr ta = {"arial.font", 11, FSF_TAGGED, FPF_DISKFONT | FPF_PROPORTIONAL | FPF_DESIGNED};
#else
    struct TextAttr ta = {"topaz.font", 10, FS_NORMAL, FPF_ROMFONT};
#endif

    struct Screen *scr = OpenScreenTags(NULL, SA_Font, (IPTR)&ta, TAG_END);
    CU_ASSERT_PTR_NOT_NULL_FATAL(scr);

    CU_ASSERT_STRING_EQUAL(ta.ta_Name, scr->Font->ta_Name);
    CU_ASSERT_EQUAL(ta.ta_YSize, scr->Font->ta_YSize);
    CU_ASSERT_EQUAL(ta.ta_Style, scr->Font->ta_Style);
    CU_ASSERT_EQUAL(ta.ta_Flags, scr->Font->ta_Flags);

    CloseScreen(scr);
}

static void test_openscreen_likeworkbench()
{
    /* Default values */
#ifdef __AROS__
    struct TextAttr defta = {"arial.font", 13, FSF_TAGGED, FPF_DISKFONT | FPF_PROPORTIONAL | FPF_DESIGNED};
#else
    struct TextAttr defta = {"topaz.font", 8, FS_NORMAL, 0 };
#endif

    struct Screen *wb = LockPubScreen("Workbench");
    CU_ASSERT_PTR_NOT_NULL_FATAL(wb);

    struct Screen *scr = OpenScreenTags(NULL, SA_LikeWorkbench, TRUE, TAG_END);
    CU_ASSERT_PTR_NOT_NULL_FATAL(scr);

    CU_ASSERT_STRING_EQUAL(wb->Font->ta_Name, scr->Font->ta_Name);
    CU_ASSERT_EQUAL(wb->Font->ta_YSize, scr->Font->ta_YSize);
    CU_ASSERT_EQUAL(wb->Font->ta_Style, scr->Font->ta_Style);
    CU_ASSERT_EQUAL(wb->Font->ta_Flags, scr->Font->ta_Flags);

    CU_ASSERT_STRING_EQUAL(defta.ta_Name, scr->Font->ta_Name);
    CU_ASSERT_EQUAL(defta.ta_YSize, scr->Font->ta_YSize);
    CU_ASSERT_EQUAL(defta.ta_Style, scr->Font->ta_Style);
    CU_ASSERT_EQUAL(defta.ta_Flags, scr->Font->ta_Flags);

    CloseScreen(scr);

    UnlockPubScreen(NULL, wb);
}

static void test_openscreen_likeworkbench_safont_1()
{
    /* Make sure these fonts are different then defaults (arial 13 and topaz 8)*/
#ifdef __AROS__
    struct TextAttr ta = {"arial.font", 11, FSF_TAGGED, FPF_DISKFONT | FPF_PROPORTIONAL | FPF_DESIGNED};
#else
    struct TextAttr ta = {"topaz.font", 10, FS_NORMAL, FPF_ROMFONT};
#endif

    struct Screen *scr = OpenScreenTags(NULL, SA_LikeWorkbench, TRUE, SA_Font, (IPTR)&ta, TAG_END);
    CU_ASSERT_PTR_NOT_NULL_FATAL(scr);

    CU_ASSERT_STRING_EQUAL(ta.ta_Name, scr->Font->ta_Name);
    CU_ASSERT_EQUAL(ta.ta_YSize, scr->Font->ta_YSize);
    CU_ASSERT_EQUAL(ta.ta_Style, scr->Font->ta_Style);
    CU_ASSERT_EQUAL(ta.ta_Flags, scr->Font->ta_Flags);

    CloseScreen(scr);
}

/* This test succeeds under OS 3.1 and fails under AROS even though documentation says SA_SysFont should override SA_Font */
/*static void test_openscreen_likeworkbench_safont_2()
{
    // Make sure these fonts are different then defaults (arial 13 and topaz 8)
#ifdef __AROS__
    struct TextAttr ta = {"arial.font", 11, FSF_TAGGED, FPF_DISKFONT | FPF_PROPORTIONAL | FPF_DESIGNED};
#else
    struct TextAttr ta = {"topaz.font", 10, FS_NORMAL, FPF_ROMFONT};
#endif

    struct Screen *scr = OpenScreenTags(NULL, SA_LikeWorkbench, TRUE, SA_SysFont, 0, SA_Font, (IPTR)&ta, TAG_END);
    CU_ASSERT_PTR_NOT_NULL_FATAL(scr);

    CU_ASSERT_STRING_EQUAL(ta.ta_Name, scr->Font->ta_Name);
    CU_ASSERT_EQUAL(ta.ta_YSize, scr->Font->ta_YSize);
    CU_ASSERT_EQUAL(ta.ta_Style, scr->Font->ta_Style);
    CU_ASSERT_EQUAL(ta.ta_Flags, scr->Font->ta_Flags);

    CloseScreen(scr);
}*/


int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(OpenScreen_Suite, __cu_suite_setup, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_openscreen_safont);
    CUNIT_CI_TEST(test_openscreen_likeworkbench);
    CUNIT_CI_TEST(test_openscreen_likeworkbench_safont_1);
    // CUNIT_CI_TEST(test_openscreen_likeworkbench_safont_2);

    return CU_CI_RUN_SUITES();
}
