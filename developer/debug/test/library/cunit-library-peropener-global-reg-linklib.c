/*
    Copyright © 2023, The AROS Development Team. All rights reserved.
    $Id$
*/

#define SINGLE_NOLIBINLINE
#define SINGLE_NOLIBDEFINES
#include <proto/peropener.h>

#include <CUnit/CUnitCI.h>

void test_peropener_global_reg_linklib(void)
{
    CU_ASSERT_EQUAL(16, PeropenerGetValueReg());
    PeropenerSetValueReg(25);
}
