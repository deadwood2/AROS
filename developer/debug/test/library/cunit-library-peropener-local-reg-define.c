/*
    Copyright © 2023, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/exec.h>
#define SINGLE_NOLIBINLINE
#include <proto/peropener.h>

#include <CUnit/CUnitCI.h>

void test_peropener_local_reg_define(struct Library *PeropenerBase)
{
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, PeropenerBase);
    CU_ASSERT_EQUAL(5, PeropenerGetValueReg());
    PeropenerSetValueReg(8);
}

void test_peropener_local_redef_reg_define(struct Library *argPeropenerBase)
{
    CU_ASSERT_EQUAL(NULL, PeropenerBase);
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, argPeropenerBase);
#define PeropenerBase argPeropenerBase
    CU_ASSERT_EQUAL(3, PeropenerGetValueReg()); /* New library opened */
    PeropenerSetValueReg(40);
#undef PeropenerBase
}
