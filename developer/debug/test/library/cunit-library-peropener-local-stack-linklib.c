/*
    Copyright © 2023, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/exec.h>
#include <proto/peropener.h>

#include <CUnit/CUnitCI.h>

void test_peropener_local_stack_linklib(struct Library *PeropenerBase)
{
    /* This is not possible due to linklib being pre-compiled with use of global variable */
    /* This file is kept as documentation of behavior, do not remove */
    /*
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, PeropenerBase);
    CU_ASSERT_EQUAL(3, PeropenerGetValueStack()); // New library opened
    PeropenerSetValueStack(33);
    */
}
