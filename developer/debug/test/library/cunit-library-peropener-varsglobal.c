/*
    Copyright © 2023, The AROS Development Team. All rights reserved.
    $Id$
*/


#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/peropener.h>

#include <CUnit/CUnitCI.h>

struct Library *PeropenerBase = NULL;

void test_peropener_global_vars_peropener()
{
    struct Library *base1, *base2;

    base1 = OpenLibrary((STRPTR)"peropener.library",0);
    base2 = OpenLibrary((STRPTR)"peropener.library",0);

    CU_ASSERT_NOT_EQUAL_FATAL(base1, base2);

    /* Check value for base2 */
    PeropenerBase = base2;
    CU_ASSERT_EQUAL(8, PeropenerGetGlobalPeropenerValueReg());

    /* Check value for base1 */
    PeropenerBase = base1;
    CU_ASSERT_EQUAL(8, PeropenerGetGlobalPeropenerValueReg());

    /* Set value for base1 */
    PeropenerBase = base1;
    PeropenerSetGlobalPeropenerValueReg(1);

    /* Set value for base2 */
    PeropenerBase = base2;
    PeropenerSetGlobalPeropenerValueReg(2);

    /* Check value for base2 */
    CU_ASSERT_EQUAL(2, PeropenerGetGlobalPeropenerValueReg());

    /* Check value for base1 */
    PeropenerBase = base1;
    CU_ASSERT_EQUAL(1, PeropenerGetGlobalPeropenerValueReg());

    if (base1 != NULL)
        CloseLibrary(base1);
    if (base2 != NULL)
        CloseLibrary(base2);
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(Library_Peropener_VarsGlobal_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_peropener_global_vars_peropener);
    return CU_CI_RUN_SUITES();
}
