/*
    Copyright © 2023, The AROS Development Team. All rights reserved.
    $Id$
*/

#define SINGLE_NOLIBINLINE
#include <proto/peropener.h>

#include <CUnit/CUnitCI.h>


void test_peropener_relative_reg_define(void)
{
    CU_ASSERT_EQUAL(5, PeropenerGetValueReg());
    PeropenerSetValueReg(8);
}

#include "cunit-library-any-any.h"

extern struct storage mystore;

static char *localgetoffsets()
{
    return (char *)&mystore;
}

void test_peropener_relative_redef_reg_define(void)
{
#define __aros_getoffsettable() (localgetoffsets())
    CU_ASSERT_EQUAL(33, PeropenerGetValueReg());
    PeropenerSetValueReg(40);
#undef __aros_getoffsettable
}
