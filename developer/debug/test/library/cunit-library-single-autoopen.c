/*
    Copyright © 2021-2023, The AROS Development Team. All rights reserved.
    $Id$
*/


#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/single.h>

#include <CUnit/CUnitCI.h>

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
    CU_CI_DEFINE_SUITE(Library_Single_AutoOpen_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_add);
    return CU_CI_RUN_SUITES();
}
