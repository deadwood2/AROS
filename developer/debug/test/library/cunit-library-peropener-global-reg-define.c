/*
    Copyright © 2023, The AROS Development Team. All rights reserved.
    $Id$
*/

#define SINGLE_NOLIBINLINE
#include <proto/peropener.h>

#include <CUnit/CUnitCI.h>

void test_peropener_global_reg_define(void)
{
    CU_ASSERT_EQUAL(5, PeropenerGetValueReg());
    PeropenerSetValueReg(8);
}

#include "cunit-library-any-global.h"

extern struct storage mystore;

void test_peropener_global_redef_reg_define(void)
{
    PeropenerBase = NULL;
    CU_ASSERT_EQUAL(NULL, PeropenerBase);

#define PeropenerBase mystore.myBase
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, PeropenerBase);
    CU_ASSERT_EQUAL(33, PeropenerGetValueReg());
    PeropenerSetValueReg(40);
#undef PeropenerBase

    PeropenerBase = mystore.myBase;
}
