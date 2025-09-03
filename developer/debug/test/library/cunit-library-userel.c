/*
    Copyright © 2021-2023, The AROS Development Team. All rights reserved.
    $Id$
*/


#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/peropener.h>
#include <proto/userel.h>

#include <CUnit/CUnitCI.h>

void test_add(void)
{
    CU_ASSERT_EQUAL(11, UseRelAdd2(8, 1, 2));
}

void test_gpbse(void)
{
    const int e5 = 7;
    SetPeropenerLibraryValue(e5);

    CU_ASSERT(e5 == GetPeropenerLibraryValue());
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(Library_UseRel_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_add);
    CUNIT_CI_TEST(test_gpbse);
    return CU_CI_RUN_SUITES();
}
