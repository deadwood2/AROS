/*
    Copyright © 2021-2022, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dos.h>
#include <string.h>

#include "../test-util.h"

#if defined(__AROS__)
#include <CUnit/CUnitCI.h>
#endif

enum
{
    ARG_COL,
    ARG_CNT
};

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

/* test of ReadArgs() with a /N parameter and a number.
 */
void test_readargs_number(void)
{
    IPTR args[ARG_CNT];
    struct RDArgs *rdargs;
    LONG colno = -1;
    
    STRPTR template = "COL=C/N";
    STRPTR param = "COL=3";
    
    if ((rdargs = AllocDosObject(DOS_RDARGS, NULL)))
    {
        rdargs->RDA_Source.CS_Buffer = param;
        rdargs->RDA_Source.CS_Length = strlen(param);
        rdargs->RDA_Source.CS_CurChr = 0;
        rdargs->RDA_DAList = 0;
        rdargs->RDA_Buffer = NULL;
        rdargs->RDA_BufSiz = 0;
        rdargs->RDA_ExtHelp = NULL;
        rdargs->RDA_Flags = 0;

        memset(args, 0, sizeof args);

        if ((ReadArgs(template, args, rdargs)))
        {
            CU_FAIL("ReadArgs() returned non-NULL");
            
            FreeArgs(rdargs);
        }

        FreeDosObject(DOS_RDARGS, rdargs);
    }
    else
    {
        CU_FAIL("AllocDosObject() returned NULL");
    }
}


/* test of ReadArgs() with a /N parameter and a number
 * followed by a space.
 */
void test_readargs_number_space(void)
{
    IPTR args[ARG_CNT];
    struct RDArgs *rdargs;
    LONG colno = -1;
    
    STRPTR template = "COL=C/N";
    STRPTR param = "COL=3 ";
    
    if ((rdargs = AllocDosObject(DOS_RDARGS, NULL)))
    {
        rdargs->RDA_Source.CS_Buffer = param;
        rdargs->RDA_Source.CS_Length = strlen(param);
        rdargs->RDA_Source.CS_CurChr = 0;
        rdargs->RDA_DAList = 0;
        rdargs->RDA_Buffer = NULL;
        rdargs->RDA_BufSiz = 0;
        rdargs->RDA_ExtHelp = NULL;
        rdargs->RDA_Flags = 0;

        memset(args, 0, sizeof args);

        if ((ReadArgs(template, args, rdargs)))
        {
            if (args[ARG_COL])
                colno = *(LONG *) args[ARG_COL];

            CU_ASSERT(3 == colno);
            
            FreeArgs(rdargs);
        }
        else
        {
            CU_FAIL("ReadArgs() returned NULL");
        }

        FreeDosObject(DOS_RDARGS, rdargs);
    }
    else
    {
        CU_FAIL("AllocDosObject() returned NULL");
    }
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE("ReadArgs_Suite", __cu_suite_setup, __cu_suite_teardown, __cu_test_setup, __cu_test_teardown);
    CUNIT_CI_TEST(test_readargs_number);
    CUNIT_CI_TEST(test_readargs_number_space);
    return CU_CI_RUN_SUITES();
}
