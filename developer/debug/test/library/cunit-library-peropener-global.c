/*
    Copyright © 2023, The AROS Development Team. All rights reserved.
    $Id$
*/


#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/peropener.h>

#include <CUnit/CUnitCI.h>

#include "cunit-library-any-any.h"

/*
    See arch/x86_64-all/ABI_SPECIFICATION for details
*/

struct Library *PeropenerBase = NULL;

struct storage mystore;

void test_peropener_global_reg_define();
void test_peropener_global_reg_inline();
void test_peropener_global_reg_linklib();
void test_peropener_global_stack_linklib();
void test_peropener_global_redef_reg_define();
void test_peropener_global_redef_reg_inline();

void test_reg_calls()
{
    CU_ASSERT_EQUAL_FATAL(NULL, PeropenerBase);

    PeropenerBase = OpenLibrary("peropener.library", 0L);
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, PeropenerBase);

    CU_ASSERT_EQUAL(3, PeropenerGetValueReg())
    PeropenerSetValueReg(5);

    test_peropener_global_reg_define();

    test_peropener_global_reg_inline();

    test_peropener_global_reg_linklib();

    CloseLibrary(PeropenerBase);
    PeropenerBase = NULL;
}

void test_stack_calls_redef_reg_calls()
{
    CU_ASSERT_EQUAL_FATAL(NULL, PeropenerBase);

    PeropenerBase = OpenLibrary("peropener.library", 0L);
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, PeropenerBase);

    test_peropener_global_stack_linklib();

    mystore.myBase = PeropenerBase;

    test_peropener_global_redef_reg_define();

    test_peropener_global_redef_reg_inline();

    CloseLibrary(PeropenerBase);
    PeropenerBase = NULL;
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(Library_Peropener_Global_Suite, NULL, NULL, NULL, NULL);
    /* Order of calls matter as they manipulate global variable */
    CUNIT_CI_TEST(test_reg_calls);
    CUNIT_CI_TEST(test_stack_calls_redef_reg_calls);
    return CU_CI_RUN_SUITES();
}
