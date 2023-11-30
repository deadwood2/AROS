/*
    Copyright © 2023, The AROS Development Team. All rights reserved.
    $Id$
*/


#include <proto/peropener.h>

#include <CUnit/CUnitCI.h>

void test_peropener_global_stack_linklib(void)
{
    CU_ASSERT_EQUAL(3, PeropenerGetValueStack()); /* New library opened */
    PeropenerSetValueStack(33);
}
