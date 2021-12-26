/*
    Copyright © 2021, The AROS Development Team. All rights reserved.
    $Id$
*/


#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/peropener.h>
#include <proto/userel.h>

#include <CUnit/Basic.h>
#include <CUnit/Automated.h>

#include "peropenervalue.h"

/* The suite initialization function.
  * Returns zero on success, non-zero otherwise.
 */
int init_suite(void)
{
    return 0;
}

/* The suite cleanup function.
  * Returns zero on success, non-zero otherwise.
 */
int clean_suite(void)
{
    return 0;
}

void testADD(void)
{
    const int e1 = 11;
    int r1 = UseRelAdd2(8, 1, 2);
    if (e1 != r1)
    {
        CU_FAIL("11 != UseRelAdd2(8, 1, 2)");
    }
    else
    {
        CU_PASS("");
    }
}

void testGPBSE(void)
{
    const int e4 = 5;
    SetPeropenerLibraryValue(e4);
    int r4 = GetPeropenerLibraryValue();
    if (e4 != r4)
    {
        CU_FAIL("2 != GetPeropenerLibraryValue()");
    }
    else
    {
        CU_PASS("");
    }
}

void testPOVL(void)
{
    const int e5 = 7;
    SetPeropenerLibraryValue(e5);

    CU_ASSERT(e5 == GetPeropenerLibraryValue());
    CU_ASSERT(e5 == peropenervalue); // <-- this one fails
}

int main(void)
{
    CU_pSuite pSuite = NULL;

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

   /* add a suite to the registry */
    pSuite = CU_add_suite("Relbase_Suite", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

   /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "test of calling single.library via userel.library, using relbase", testADD)) ||
        (NULL == CU_add_test(pSuite, "test of calling peropener.library via userel.library, using relbase", testGPBSE)) ||
        (NULL == CU_add_test(pSuite, "test of accessing peropenerbase->value via userel.library, using relbase", testPOVL)))
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic & Automated interfaces */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_basic_set_mode(CU_BRM_SILENT);
    CU_automated_package_name_set("GenmoduleUnitTests");
    CU_set_output_filename("Genmodule-Relbase");
    CU_automated_enable_junit_xml(CU_TRUE);
    CU_automated_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}
