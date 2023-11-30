/*
    Copyright © 2023, The AROS Development Team. All rights reserved.
    $Id$
*/


#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/peropener.h>

#include <CUnit/CUnitCI.h>

/*
    See arch/x86_64-all/ABI_SPECIFICATION for details
*/

struct Library *PeropenerBase = NULL;

void test_peropener_local_reg_define(struct Library *);
void test_peropener_local_reg_inline(struct Library *);
void test_peropener_local_reg_linklib(struct Library *);
void test_peropener_local_stack_linklib(struct Library *);
void test_peropener_local_redef_reg_define(struct Library *);
void test_peropener_local_redef_reg_inline(struct Library *);

CU_SUITE_SETUP()
{
    return CUE_SUCCESS;
}

CU_SUITE_TEARDOWN()
{
    return CUE_SUCCESS;
}

CU_TEST_SETUP()
{
}

CU_TEST_TEARDOWN()
{
}

void test_reg_calls()
{
    CU_ASSERT_EQUAL_FATAL(NULL, PeropenerBase);

    struct Library * localPeropenerBase = OpenLibrary("peropener.library", 0L);
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, localPeropenerBase);

#define PeropenerBase localPeropenerBase
    CU_ASSERT_EQUAL(3, PeropenerGetValueReg())
    PeropenerSetValueReg(5);
#undef PeropenerBase

    test_peropener_local_reg_define(localPeropenerBase);

    test_peropener_local_reg_inline(localPeropenerBase);

    test_peropener_local_reg_linklib(localPeropenerBase);

    CloseLibrary(localPeropenerBase);
}

void test_stack_calls_redef_reg_calls()
{
    CU_ASSERT_EQUAL_FATAL(NULL, PeropenerBase);

    struct Library *localPeropenerBase = OpenLibrary("peropener.library", 0L);
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, localPeropenerBase);

    test_peropener_local_stack_linklib(localPeropenerBase);

    test_peropener_local_redef_reg_define(localPeropenerBase);

    test_peropener_local_redef_reg_inline(localPeropenerBase);

    CloseLibrary(localPeropenerBase);
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE("Library_Peropener_Local_Suite", __cu_suite_setup, __cu_suite_teardown, __cu_test_setup, __cu_test_teardown);
    /* Order of calls matter as they manipulate global variable */
    CUNIT_CI_TEST(test_reg_calls);
    CUNIT_CI_TEST(test_stack_calls_redef_reg_calls);
    return CU_CI_RUN_SUITES();
}
