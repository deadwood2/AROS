/*
    Copyright (C) 2024, The AROS Development Team. All rights reserved.
    $Id$
*/


#include <proto/exec.h>
#include <proto/dos.h>

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

void RegSetValue(LONG v);
int StackGetValue();

void test_basic(void)
{
    RegSetValue(5);
    CU_ASSERT_EQUAL(StackGetValue(), 5);
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE("Library_Noincludes_Suite", __cu_suite_setup, __cu_suite_teardown, __cu_test_setup, __cu_test_teardown);
    CUNIT_CI_TEST(test_basic);
    return CU_CI_RUN_SUITES();
}

