/*
    Copyright (C) 2024, The AROS Development Team.
    All rights reserved.
*/

#include <proto/exec.h>

#include <CUnit/CUnitCI.h>

int sub()
{
    throw 20;
}

static void test_exception_caught()
{
    int result = 10;

    try
    {
        sub();
    }
    catch(int e)
    {
        result = e;
    }

    CU_ASSERT_EQUAL(result, 20);
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE("CPlusPlus_Exception_Suite", __cu_suite_setup, __cu_suite_teardown, __cu_test_setup, __cu_test_teardown);
    CUNIT_CI_TEST(test_exception_caught);
    return CU_CI_RUN_SUITES();
}
