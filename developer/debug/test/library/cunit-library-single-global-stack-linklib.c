/*
    Copyright © 2023, The AROS Development Team. All rights reserved.
    $Id$
*/


#include <proto/single.h>

#include <CUnit/CUnitCI.h>

void test_single_global_stack_linklib(void)
{
    CU_ASSERT_EQUAL(25, StackAdd4(0, 0, 0, 0));
    CU_ASSERT_EQUAL(27, StackAdd4(2, 0, 0, 0));
    StackSetValue(33);
}
