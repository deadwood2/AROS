/*
    Copyright © 2023, The AROS Development Team. All rights reserved.
    $Id$
*/

#define SINGLE_NOLIBDEFINES
#include <proto/peropener.h>

#include <CUnit/CUnitCI.h>

void test_peropener_relative_reg_inline(void)
{
    /* Inline calls are disabled in case of relative library base */
    /* This file is kept as documentation of behavior, do not remove */
    /*
    CU_ASSERT_EQUAL(8, PeropenerGetValueReg());
    PeropenerSetValueReg(16);
    */
}

#include "cunit-library-any-any.h"

extern struct storage mystore;

static char *localgetoffsets()
{
    return (char *)&mystore;
}

void test_peropener_relative_redef_reg_inline(void)
{
    /* Inline calls are disabled in case of relative library base */
    /* This file is kept as documentation of behavior, do not remove */
    /*
#define __aros_getoffsettable() (localgetoffsets())
    CU_ASSERT_EQUAL(40, PeropenerGetValueReg());
    PeropenerSetValueReg(55);
#undef __aros_getoffsettable
    */
}
