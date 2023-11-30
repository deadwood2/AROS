/*
    Copyright © 2023, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/exec.h>
#define SINGLE_NOLIBDEFINES
#include <proto/peropener.h>

#include <CUnit/CUnitCI.h>

void test_peropener_local_reg_inline(struct Library *PeropenerBase)
{
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, PeropenerBase);
    CU_ASSERT_EQUAL(8, PeropenerGetValueReg());
    PeropenerSetValueReg(16);
    PeropenerSetValueReg(33);
}

void test_peropener_local_redef_reg_inline(struct Library *argPeropenerBase)
{
    CU_ASSERT_EQUAL(NULL, PeropenerBase);
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, argPeropenerBase);
#define PeropenerBase argPeropenerBase
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, PeropenerBase);
    CU_ASSERT_EQUAL(40, PeropenerGetValueReg());
    PeropenerSetValueReg(55);
#undef PeropenerBase
}