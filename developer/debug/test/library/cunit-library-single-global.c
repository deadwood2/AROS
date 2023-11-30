/*
    Copyright © 2023, The AROS Development Team. All rights reserved.
    $Id$
*/


#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/single.h>

#include <CUnit/CUnitCI.h>

#include "cunit-library-single-global.h"

/*
    See arch/x86_64-all/ABI_SPECIFICATION for details
*/

struct Library *SingleBase = NULL;

struct storage mystore;

void test_single_global_reg_define();
void test_single_global_reg_inline();
void test_single_global_reg_linklib();
void test_single_global_stack_linklib();
void test_single_global_redef_reg_define();
void test_single_global_redef_reg_inline();

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

void test_reg_calls()
{
    CU_ASSERT_EQUAL_FATAL(NULL, SingleBase);

    SingleBase = OpenLibrary("single.library", 0L);
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, SingleBase);

    RegSetValue(5);

    test_single_global_reg_define();

    test_single_global_reg_inline();

    test_single_global_reg_linklib();

    CloseLibrary(SingleBase);
    SingleBase = NULL;
}

void test_stack_calls_redef_reg_calls()
{
    CU_ASSERT_EQUAL_FATAL(NULL, SingleBase);

    SingleBase = OpenLibrary("single.library", 0L);
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, SingleBase);

    test_single_global_stack_linklib();

    mystore.mySingleBase = SingleBase;

    test_single_global_redef_reg_define();

    test_single_global_redef_reg_inline();

    CloseLibrary(SingleBase);
    SingleBase = NULL;
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE("Library_Single_Global_Suite", __cu_suite_setup, __cu_suite_teardown, __cu_test_setup, __cu_test_teardown);
    /* Order of calls matter as they manipulate global variable */
    CUNIT_CI_TEST(test_reg_calls);
    CUNIT_CI_TEST(test_stack_calls_redef_reg_calls);
    return CU_CI_RUN_SUITES();
}
