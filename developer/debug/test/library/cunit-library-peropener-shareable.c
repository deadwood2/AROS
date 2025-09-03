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
struct Library *UseRelBase = NULL;

void test_peropener_not_shareable()
{
    /* Shows that when shareable mode is not enabled, binary and userel.library use separe peropener bases */
    CU_ASSERT_EQUAL_FATAL(NULL, PeropenerBase);
    CU_ASSERT_EQUAL_FATAL(NULL, UseRelBase);

    PeropenerBase = OpenLibrary("peropener.library", 0L);
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, PeropenerBase);

    UseRelBase = OpenLibrary("userel.library", 0L);
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, UseRelBase);

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

    CloseLibrary(UseRelBase);
    UseRelBase = NULL;

    CloseLibrary(PeropenerBase);
    PeropenerBase = NULL;
}

void test_peropener_shareable()
{
    /* Shows that when shareable mode is enabled, binary and userel.library use same peropener base */
    CU_ASSERT_EQUAL_FATAL(NULL, PeropenerBase);
    CU_ASSERT_EQUAL_FATAL(NULL, UseRelBase);

    PeropenerBase = OpenLibrary("peropener.library", 0L);
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, PeropenerBase);

    Peropener_AllowShareable(PeropenerBase);

    UseRelBase = OpenLibrary("userel.library", 0L);
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, UseRelBase);

    Peropener_DisallowShareable(PeropenerBase);

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

    CloseLibrary(UseRelBase);
    UseRelBase = NULL;

    CloseLibrary(PeropenerBase);
    PeropenerBase = NULL;
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(Library_Peropener_Shareable_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_peropener_not_shareable);
    CUNIT_CI_TEST(test_peropener_shareable);
    return CU_CI_RUN_SUITES();
}
