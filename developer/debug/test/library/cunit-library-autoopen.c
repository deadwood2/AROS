/*
    Copyright © 2021-2023, The AROS Development Team. All rights reserved.
    $Id$
*/


#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/single.h>

#include <CUnit/CUnitCI.h>

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

void test_add(void)
{
    RegSetValue(5);

    LONG r1 = RegAdd4(1, 2, 3, 4);

    CU_ASSERT_EQUAL(15, r1);

    int r5 = StackAdd4OrMore(6, 1, 2, 3, 4, 5, 6);

    CU_ASSERT_EQUAL(26, r5);
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE("Library_AutoOpen_Suite", __cu_suite_setup, __cu_suite_teardown, __cu_test_setup, __cu_test_teardown);
    CUNIT_CI_TEST(test_add);
    return CU_CI_RUN_SUITES();
}
