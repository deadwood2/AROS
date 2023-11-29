/*
    Copyright © 2023, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/exec.h>
#define SINGLE_NOLIBDEFINES
#include <proto/single.h>

#include <CUnit/CUnitCI.h>

void test_single_local_reg_inline(void)
{
    struct Library *SingleBase = OpenLibrary("single.library", 0L);
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, SingleBase);
    CU_ASSERT_EQUAL(8, RegAdd4(0, 0, 0, 0));
    CU_ASSERT_EQUAL(13, RegAdd4(5, 0, 0, 0));
    RegSetValue(16);
    RegSetValue(33);
    CloseLibrary(SingleBase);
}

void test_single_local_redef_reg_inline(void)
{
    struct Library *mySingleBase = OpenLibrary("single.library", 0L);
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, mySingleBase);
#define SingleBase mySingleBase
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, SingleBase);
    CU_ASSERT_EQUAL(40, RegAdd4(0, 0, 0, 0));
    CU_ASSERT_EQUAL(45, RegAdd4(0, 0, 5, 0));
    RegSetValue(55);
#undef SingleBase
    CloseLibrary(mySingleBase);
}