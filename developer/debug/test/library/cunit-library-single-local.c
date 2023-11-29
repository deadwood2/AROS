/*
    Copyright © 2023, The AROS Development Team. All rights reserved.
    $Id$
*/


#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/single.h>

#include <CUnit/CUnitCI.h>

/*
    See arch/x86_64-all/ABI_SPECIFICATION for details
*/

struct Library *SingleBase = NULL;

void test_single_local_reg_define();
void test_single_local_reg_inline();
void test_single_local_reg_linklib();
void test_single_local_stack_linklib();
void test_single_local_redef_reg_define();
void test_single_local_redef_reg_inline();

CU_SUITE_SETUP()
{
    struct Library *SingleBase = OpenLibrary("single.library", 0L);
    if (!SingleBase)
        return CUE_SINIT_FAILED;

    RegSetValue(5);

    CloseLibrary(SingleBase);

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

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE("Library_Single_Local_Suite", __cu_suite_setup, __cu_suite_teardown, __cu_test_setup, __cu_test_teardown);
    /* Order of calls matter as they manipulate global variable */
    CUNIT_CI_TEST(test_single_local_reg_define);
    CUNIT_CI_TEST(test_single_local_reg_inline);
    /* CUNIT_CI_TEST(test_single_local_reg_linklib); This is not possible due to linklib being pre-compiled with use of global variable */
    /* CUNIT_CI_TEST(test_single_local_stack_linklib); This is not possible due to linklib being pre-compiled with use of global variable */
    CUNIT_CI_TEST(test_single_local_redef_reg_define);
    CUNIT_CI_TEST(test_single_local_redef_reg_inline);
    return CU_CI_RUN_SUITES();
}
