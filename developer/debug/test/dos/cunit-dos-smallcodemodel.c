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
    CU_CI_DEFINE_SUITE(SmallCodeModel_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_read_struct_field_small_code_model);

    return CU_CI_RUN_SUITES();
}
