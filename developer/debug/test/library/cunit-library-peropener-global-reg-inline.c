/*
    Copyright © 2023, The AROS Development Team. All rights reserved.
    $Id$
*/

#define SINGLE_NOLIBDEFINES
#include <proto/peropener.h>

#include <CUnit/CUnitCI.h>

void test_peropener_global_reg_inline(void)
{
    CU_ASSERT_EQUAL(8, PeropenerGetValueReg());
    PeropenerSetValueReg(16);
}

#include "cunit-library-any-any.h"

extern struct storage mystore;

void test_peropener_global_redef_reg_inline(void)
{
    PeropenerBase = NULL;
    CU_ASSERT_EQUAL(NULL, PeropenerBase);

#define PeropenerBase mystore.myBase
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, PeropenerBase);
    CU_ASSERT_EQUAL(40, PeropenerGetValueReg());
    PeropenerSetValueReg(55);
#undef PeropenerBase

    PeropenerBase = mystore.myBase;
}