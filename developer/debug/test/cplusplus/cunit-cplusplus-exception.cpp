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
    CU_CI_DEFINE_SUITE(CPlusPlus_Exception_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_exception_caught);
    return CU_CI_RUN_SUITES();
}
