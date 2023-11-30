/*
    Copyright © 2023, The AROS Development Team. All rights reserved.
    $Id$
*/

#define SINGLE_NOLIBDEFINES
#include <proto/single.h>

#include <CUnit/CUnitCI.h>

void test_single_global_reg_inline(void)
{
    CU_ASSERT_EQUAL(8, RegAdd4(0, 0, 0, 0));
    CU_ASSERT_EQUAL(13, RegAdd4(5, 0, 0, 0));
    RegSetValue(16);
}

#include "cunit-library-any-any.h"

extern struct storage mystore;

void test_single_global_redef_reg_inline(void)
{
    SingleBase = NULL;
    CU_ASSERT_EQUAL(NULL, SingleBase);

#define SingleBase mystore.myBase
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, SingleBase);
    CU_ASSERT_EQUAL(40, RegAdd4(0, 0, 0, 0));
    CU_ASSERT_EQUAL(45, RegAdd4(0, 0, 5, 0));
    RegSetValue(55);
#undef SingleBase

    SingleBase = mystore.myBase;
}