/*
    Copyright © 2023, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/exec.h>
#define SINGLE_NOLIBINLINE
#define SINGLE_NOLIBDEFINES
#include <proto/single.h>

#include <CUnit/CUnitCI.h>

void test_single_local_reg_linklib(struct Library *SingleBase)
{
    /* This is not possible due to linklib being pre-compiled with use of global variable */
    /* This file is kept as documentation of behavior, do not remove */
    /*
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, SingleBase);
    CU_ASSERT_EQUAL(16, RegAdd4(0, 0, 0, 0));
    CU_ASSERT_EQUAL(20, RegAdd4(0, 4, 0, 0));
    RegSetValue(25);
    */
}
