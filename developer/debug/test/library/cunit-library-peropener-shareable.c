/*
    Copyright © 2023, The AROS Development Team. All rights reserved.
    $Id$
*/


#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/peropener.h>
#include <proto/userel.h>

#include "peropenervalue.h"

#include <CUnit/CUnitCI.h>

struct Library *PeropenerBase = NULL;
struct Library *UserelBase = NULL;

CU_SUITE_SETUP()
{
    return CUE_SUCCESS;
}

CU_SUITE_TEARDOWN()
{
    return CUE_SUCCESS;
}

CU_TEST_SETUP()
{
}

CU_TEST_TEARDOWN()
{
}

void test_peropener_not_shareable()
{
    /* Shows that when shareable mode is not enabled, binary and userel.library use separe peropener bases */
    CU_ASSERT_EQUAL_FATAL(NULL, PeropenerBase);
    CU_ASSERT_EQUAL_FATAL(NULL, UserelBase);

    PeropenerBase = OpenLibrary("peropener.library", 0L);
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, PeropenerBase);

    UserelBase = OpenLibrary("userel.library", 0L);
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, UserelBase);

    SetPeropenerLibraryValue(5);
    CU_ASSERT_EQUAL(5, GetPeropenerLibraryValue());
    CU_ASSERT_EQUAL(3, PeropenerGetValueStack());
    CU_ASSERT_EQUAL(3, peropenervalue);

    PeropenerSetValueStack(8);
    CU_ASSERT_EQUAL(5, GetPeropenerLibraryValue());
    CU_ASSERT_EQUAL(8, PeropenerGetValueStack());
    CU_ASSERT_EQUAL(8, peropenervalue);

    peropenervalue = 13;
    CU_ASSERT_EQUAL(5, GetPeropenerLibraryValue());
    CU_ASSERT_EQUAL(13, PeropenerGetValueStack());
    CU_ASSERT_EQUAL(13, peropenervalue);

    CloseLibrary(UserelBase);
    UserelBase = NULL;

    CloseLibrary(PeropenerBase);
    PeropenerBase = NULL;
}

void test_peropener_shareable()
{
    /* Shows that when shareable mode is enabled, binary and userel.library use same peropener base */
    CU_ASSERT_EQUAL_FATAL(NULL, PeropenerBase);
    CU_ASSERT_EQUAL_FATAL(NULL, UserelBase);

    PeropenerBase = OpenLibrary("peropener.library", 0L);
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, PeropenerBase);

    UserelBase = OpenLibrary("userel.library", 0L);
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, UserelBase);

    SetPeropenerLibraryValue(5);
    CU_ASSERT_EQUAL(5, GetPeropenerLibraryValue());
    CU_ASSERT_EQUAL(5, PeropenerGetValueStack());
    CU_ASSERT_EQUAL(5, peropenervalue);

    PeropenerSetValueStack(8);
    CU_ASSERT_EQUAL(8, GetPeropenerLibraryValue());
    CU_ASSERT_EQUAL(8, PeropenerGetValueStack());
    CU_ASSERT_EQUAL(8, peropenervalue);

    peropenervalue = 13;
    CU_ASSERT_EQUAL(13, GetPeropenerLibraryValue());
    CU_ASSERT_EQUAL(13, PeropenerGetValueStack());
    CU_ASSERT_EQUAL(13, peropenervalue);

    CloseLibrary(UserelBase);
    UserelBase = NULL;

    CloseLibrary(PeropenerBase);
    PeropenerBase = NULL;
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE("Library_Peropener_Shareable_Suite", __cu_suite_setup, __cu_suite_teardown, __cu_test_setup, __cu_test_teardown);
    CUNIT_CI_TEST(test_peropener_not_shareable);
    CUNIT_CI_TEST(test_peropener_shareable);
    return CU_CI_RUN_SUITES();
}
