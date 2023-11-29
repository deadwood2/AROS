/*
    Copyright © 2023, The AROS Development Team. All rights reserved.
    $Id$
*/


#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/single.h>

#include <CUnit/CUnitCI.h>

#include "cunit-library-single-global.h"

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
    SingleBase = OpenLibrary("single.library", 0L);
    if (!SingleBase)
        return CUE_SINIT_FAILED;

    mystore.mySingleBase = SingleBase;

    RegSetValue(5);

    return CUE_SUCCESS;
}

CU_SUITE_TEARDOWN()
{
    CloseLibrary(SingleBase);
    return CUE_SUCCESS;
}

CU_TEST_SETUP()
{
}

CU_TEST_TEARDOWN()
{
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE("Library_Single_Global_Suite", __cu_suite_setup, __cu_suite_teardown, __cu_test_setup, __cu_test_teardown);
    /* Order of calls matter as they manipulate global variable */
    CUNIT_CI_TEST(test_single_global_reg_define);
    CUNIT_CI_TEST(test_single_global_reg_inline);
    CUNIT_CI_TEST(test_single_global_reg_linklib);
    CUNIT_CI_TEST(test_single_global_stack_linklib);
    CUNIT_CI_TEST(test_single_global_redef_reg_define);
    CUNIT_CI_TEST(test_single_global_redef_reg_inline);
    return CU_CI_RUN_SUITES();
}
