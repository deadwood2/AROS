/*
    Copyright © 2023, The AROS Development Team. All rights reserved.
    $Id$
*/

#define SINGLE_NOLIBINLINE
#include <proto/single.h>

#include <CUnit/CUnitCI.h>

void test_single_global_reg_define(void)
{
    CU_ASSERT_EQUAL(5, RegAdd4(0, 0, 0, 0));
    CU_ASSERT_EQUAL(7, RegAdd4(2, 0, 0, 0));
    RegSetValue(8);
}

#include "cunit-library-any-global.h"

extern struct storage mystore;

void test_single_global_redef_reg_define(void)
{
    SingleBase = NULL;
    CU_ASSERT_EQUAL(NULL, SingleBase);

#define SingleBase mystore.myBase
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, SingleBase);
    CU_ASSERT_EQUAL(33, RegAdd4(0, 0, 0, 0));
    CU_ASSERT_EQUAL(35, RegAdd4(2, 0, 0, 0));
    RegSetValue(40);
#undef SingleBase

    SingleBase = mystore.myBase;
}
