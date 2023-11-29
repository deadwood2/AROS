/*
    Copyright © 2023, The AROS Development Team. All rights reserved.
    $Id$
*/

#define SINGLE_NOLIBINLINE
#define SINGLE_NOLIBDEFINES
#include <proto/single.h>

#include <CUnit/CUnitCI.h>

void test_single_global_reg_linklib(void)
{
    CU_ASSERT_EQUAL(16, RegAdd4(0, 0, 0, 0));
    CU_ASSERT_EQUAL(20, RegAdd4(0, 4, 0, 0));
    RegSetValue(25);
}
