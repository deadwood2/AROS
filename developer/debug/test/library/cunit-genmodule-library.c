/*
    Copyright © 2021, The AROS Development Team. All rights reserved.
    $Id$
*/


#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/single.h>

#include <CUnit/Basic.h>
#include <CUnit/Automated.h>

struct Library *SingleBase;

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

void testOPEN(void)
{
    SingleBase = OpenLibrary((STRPTR)"single.library",0);

    if (SingleBase)
    {
        CU_PASS("");
    }
    else
    {
        CU_FAIL("NULL != OpenLibrary( \"single.library\", 0 ))");
    }
}

void testBASE(void)
{
    CU_SKIP_IF(SingleBase == NULL);
    if(SingleBase != NULL)
    {
        CU_ASSERT(NT_LIBRARY == SingleBase->lib_Node.ln_Type);
        CU_ASSERT(0 != SingleBase->lib_NegSize);
        CU_ASSERT(0 != SingleBase->lib_PosSize);
        CU_ASSERT(0 != SingleBase->lib_OpenCnt);
    }
}

void testGPBSE(void)
{
    ULONG a=1,b=2,c=0,d=0;
    CU_SKIP_IF(SingleBase == NULL);
    if(SingleBase != NULL)
    {
        RegSetValue(5);

        const LONG e1 = 15;
        LONG r1 = RegAdd4(1, 2, 3, 4);

        if (e1 != r1)
            CU_FAIL("15 != RegAdd4(1, 2, 3, 4)");

        const int e5 = 26;
        int r5 = StackAdd4OrMore(6, 1, 2, 3, 4, 5, 6);
        if (e5 != r5)
            CU_FAIL("26 != StackAdd4OrMore(6, 1, 2, 3, 4, 5, 6)");

        CU_PASS("");
    }
}

void testCLOSE(void)
{
    CU_SKIP_IF(SingleBase == NULL);
    if(SingleBase != NULL)
    {
        CloseLibrary((struct Library *)SingleBase);
        CU_PASS("");
    }
}

int main(void)
{
    CU_pSuite pSuite = NULL;

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

   /* add a suite to the registry */
    pSuite = CU_add_suite("Library_Suite", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

   /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "test of OpenLibrary() on genmodule generated library", testOPEN)) ||
        (NULL == CU_add_test(pSuite, "test of opened library base", testBASE)) ||
        (NULL == CU_add_test(pSuite, "test of calling reg-call functions of opened library", testGPBSE)) ||
        (NULL == CU_add_test(pSuite, "test of CloseLibrary() on genmodule generated library", testCLOSE)))
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic & Automated interfaces */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_basic_set_mode(CU_BRM_SILENT);
    CU_automated_package_name_set("GenmoduleUnitTests");
    CU_set_output_filename("Genmodule-Library");
    CU_automated_enable_junit_xml(CU_TRUE);
    CU_automated_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}
