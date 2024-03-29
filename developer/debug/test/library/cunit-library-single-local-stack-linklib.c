/*
    Copyright � 2023, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/exec.h>
#include <proto/single.h>

#include <CUnit/CUnitCI.h>

void test_single_local_stack_linklib(struct Library *SingleBase)
{
    /* This is not possible due to linklib being pre-compiled with use of global variable */
    /* This file is kept as documentation of behavior, do not remove */
    /*
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, SingleBase);
    CU_ASSERT_EQUAL(25, StackAdd4(0, 0, 0, 0));
    CU_ASSERT_EQUAL(27, StackAdd4(2, 0, 0, 0));
    StackSetValue(33);
    */
}
