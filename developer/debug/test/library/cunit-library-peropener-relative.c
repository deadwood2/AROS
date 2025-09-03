/*
    Copyright © 2023, The AROS Development Team. All rights reserved.
    $Id$
*/


#include <proto/exec.h>
#include <proto/dos.h>
#include <aros/symbolsets.h>
#include <proto/peropener.h>

#include <CUnit/CUnitCI.h>

#include "cunit-library-any-any.h"

/*
    See arch/x86_64-all/ABI_SPECIFICATION for details
*/

struct Library *PeropenerBase = NULL;

struct storage mystore;

/* Rellib support */
const ULONG __aros_rellib_base_PeropenerBase = 0;
SETRELLIBOFFSET(PeropenerBase, struct storage, myBase);
char *__aros_getoffsettable(void)
{
    return (char *)&mystore;
}
/* + -D__PEROPENER_RELLIBBASE__ */
/* Rellib support end */

void test_peropener_relative_reg_define();
void test_peropener_relative_reg_inline();
void test_peropener_relative_reg_linklib();
void test_peropener_relative_stack_linklib();
void test_peropener_relative_redef_reg_define();
void test_peropener_relative_redef_reg_inline();

void test_reg_calls()
{
    CU_ASSERT_EQUAL_FATAL(NULL, PeropenerBase);

    mystore.myBase = OpenLibrary("peropener.library", 0L);
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, mystore.myBase);
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, __aros_getbase_PeropenerBase());

    CU_ASSERT_EQUAL(3, PeropenerGetValueReg())
    PeropenerSetValueReg(5);

    test_peropener_relative_reg_define();

    test_peropener_relative_reg_inline();

    test_peropener_relative_reg_linklib();

    CloseLibrary(mystore.myBase);
    mystore.myBase = NULL;
}

void test_stack_calls_redef_reg_calls()
{
    CU_ASSERT_EQUAL_FATAL(NULL, PeropenerBase);

    mystore.myBase = OpenLibrary("peropener.library", 0L);
    CU_ASSERT_NOT_EQUAL_FATAL(NULL, mystore.myBase);

    test_peropener_relative_stack_linklib();

    test_peropener_relative_redef_reg_define();

    test_peropener_relative_redef_reg_inline();

    CloseLibrary(mystore.myBase);
    mystore.myBase = NULL;
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(Library_Peropener_Relative_Suite, NULL, NULL, NULL, NULL);
    /* Order of calls matter as they manipulate relative variable */
    CUNIT_CI_TEST(test_reg_calls);
    CUNIT_CI_TEST(test_stack_calls_redef_reg_calls);
    return CU_CI_RUN_SUITES();
}
