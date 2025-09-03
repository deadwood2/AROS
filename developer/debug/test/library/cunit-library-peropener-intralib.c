/*
    Copyright © 2024, The AROS Development Team. All rights reserved.
    $Id$
*/


#include <proto/exec.h>
#include <proto/peropener.h>

#include <CUnit/CUnitCI.h>

struct Library *PeropenerBase = NULL;

void test_stack_call_that_goes_internally_through_reg_call()
{
    CU_ASSERT_EQUAL_FATAL(PeropenerBase, NULL);

    PeropenerBase = OpenLibrary("peropener.library", 0L);
    CU_ASSERT_NOT_EQUAL_FATAL(PeropenerBase, NULL);

    PeropenerSetValueStackThroughInternalReg(5);
    CU_ASSERT_EQUAL(PeropenerGetValueReg(), 5);

    CloseLibrary(PeropenerBase);
    PeropenerBase = NULL;
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(Library_Peropener_IntraLib_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_stack_call_that_goes_internally_through_reg_call);
    return CU_CI_RUN_SUITES();
}
