/*
    Copyright © 2023, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/exec.h>
#define SINGLE_NOLIBINLINE
#include <proto/single.h>

#include <CUnit/CUnitCI.h>

void test_single_local_reg_define(struct Library *SingleBase)
{
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, SingleBase);
    CU_ASSERT_EQUAL(5, RegAdd4(0, 0, 0, 0));
    CU_ASSERT_EQUAL(7, RegAdd4(2, 0, 0, 0));
    RegSetValue(8);
}

void test_single_local_redef_reg_define(struct Library *argSingleBase)
{
    CU_ASSERT_EQUAL(NULL, SingleBase);
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, argSingleBase);
#define SingleBase argSingleBase
    CU_ASSERT_EQUAL(33, RegAdd4(0, 0, 0, 0));
    CU_ASSERT_EQUAL(35, RegAdd4(2, 0, 0, 0));
    RegSetValue(40);
#undef SingleBase
}
