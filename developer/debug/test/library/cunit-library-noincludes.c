/*
    Copyright (C) 2024, The AROS Development Team. All rights reserved.
    $Id$
*/


#include <proto/exec.h>
#include <proto/dos.h>

#include <CUnit/CUnitCI.h>

void RegSetValue(LONG v);
int StackGetValue();

void test_basic(void)
{
    RegSetValue(5);
    CU_ASSERT_EQUAL(StackGetValue(), 5);
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(Library_Noincludes_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_basic);
    return CU_CI_RUN_SUITES();
}

