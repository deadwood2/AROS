/*
    Copyright (C) 2022, The AROS Development Team. All rights reserved.
*/
#include <exec/types.h>

#include <CUnit/CUnitCI.h>

struct A
{
    int b;
} OBJ;

static void _do(struct A *p)
{
    p->b = 0xfeed;
}

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

/* Note: If small code support is removed, this test will crash rather than fail
         The crash will be caused by using only 32-bits of variable adress (as small
         code does) when a variable is in > 2GB range
*/
static void test_read_struct_field_small_code_model()
{
    _do(&OBJ);
    CU_ASSERT_EQUAL(OBJ.b, 0xfeed);
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE("SmallCodeModel_Suite", __cu_suite_setup, __cu_suite_teardown, __cu_test_setup, __cu_test_teardown);
    CUNIT_CI_TEST(test_read_struct_field_small_code_model);

    return CU_CI_RUN_SUITES();
}
