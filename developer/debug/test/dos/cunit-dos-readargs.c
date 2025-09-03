/*
    Copyright (C) 2021-2025, The AROS Development Team. All rights reserved.
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

/* test of ReadArgs() with a /N parameter and a number.
 */
void testREADARGSNUMBER(void)
{
    IPTR args[ARG_CNT];
    struct RDArgs *rdargs;
    LONG colno = -1;
    
    STRPTR templ = "COL=C/N";
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

        if ((ReadArgs(templ, args, rdargs)))
#if defined(__AROS__)
        {
            if (args[ARG_COL])
                colno = *(LONG *) args[ARG_COL];

            CU_ASSERT(3 == colno);

            FreeArgs(rdargs);
        }
        else
        {
            LONG err = IoErr();
            CU_FAIL("ReadArgs() returned NULL");
            CU_ASSERT_NOT_EQUAL(err, ERROR_REQUIRED_ARG_MISSING);
            CU_ASSERT_NOT_EQUAL(err, ERROR_BAD_TEMPLATE);
            CU_ASSERT_NOT_EQUAL(err, ERROR_LINE_TOO_LONG);
            CU_ASSERT_NOT_EQUAL(err, ERROR_TOO_MANY_ARGS);
            CU_ASSERT_NOT_EQUAL(err, ERROR_KEY_NEEDS_ARG);
            CU_ASSERT_NOT_EQUAL(err, ERROR_NO_FREE_STORE);
            CU_ASSERT_NOT_EQUAL(err, ERROR_BAD_NUMBER);
        }
#else
// AmigaOS 3.1
        {
            CU_FAIL("ReadArgs() returned non-NULL");
            
            FreeArgs(rdargs);
        }
#endif

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
void testREADARGSNUMBERSPACE(void)
{
    IPTR args[ARG_CNT];
    struct RDArgs *rdargs;
    LONG colno = -1;
    
    STRPTR templ = "COL=C/N";
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

        if ((ReadArgs(templ, args, rdargs)))
        {
            if (args[ARG_COL])
                colno = *(LONG *) args[ARG_COL];

            CU_ASSERT(3 == colno);
            
            FreeArgs(rdargs);
        }
        else
        {
            LONG err = IoErr();
            CU_FAIL("ReadArgs() returned NULL");
            CU_ASSERT_NOT_EQUAL(err, ERROR_REQUIRED_ARG_MISSING);
            CU_ASSERT_NOT_EQUAL(err, ERROR_BAD_TEMPLATE);
            CU_ASSERT_NOT_EQUAL(err, ERROR_LINE_TOO_LONG);
            CU_ASSERT_NOT_EQUAL(err, ERROR_TOO_MANY_ARGS);
            CU_ASSERT_NOT_EQUAL(err, ERROR_KEY_NEEDS_ARG);
            CU_ASSERT_NOT_EQUAL(err, ERROR_NO_FREE_STORE);
            CU_ASSERT_NOT_EQUAL(err, ERROR_BAD_NUMBER);
        }

        FreeDosObject(DOS_RDARGS, rdargs);
    }
    else
    {
        CU_FAIL("AllocDosObject() returned NULL");
    }
}

/* Reading /F should remove everything after last argument. Test passes on AmigaOS 3.1 */
void test_ReadArgs_AF(void)
{
    IPTR args[2];
    struct RDArgs *rdargs;
    STRPTR progargs = NULL;

    STRPTR templ = "PROG/A,ARGS/F";
    STRPTR param = "foobar barbar \n"; //space + \n is important for this test

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

        if ((ReadArgs(templ, args, rdargs)))
        {
            if (args[1])
                progargs = (STRPTR)args[1];
            CU_ASSERT_EQUAL(strlen(progargs), 6);
            CU_ASSERT_STRING_EQUAL(progargs, "barbar");

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

#define ARG_COL 0
#define ARG_CNT 1

void test_ReadArgs_ERROR_REQUIRED_ARG_MISSING(void)
{
    IPTR args[ARG_CNT];
    struct RDArgs *rdargs;
    STRPTR templ = "FOO/A";
    STRPTR param = ""; // Empty input string triggers missing required arg

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

        memset(args, 0, sizeof(args));

        struct RDArgs *rda = ReadArgs(templ, args, rdargs);
        CU_ASSERT_PTR_NULL(rda);
        if (!rda)
        {
            LONG err = IoErr();
            CU_ASSERT_EQUAL(err, ERROR_REQUIRED_ARG_MISSING);
            CU_ASSERT_NOT_EQUAL(err, ERROR_BAD_TEMPLATE);
            CU_ASSERT_NOT_EQUAL(err, ERROR_LINE_TOO_LONG);
            CU_ASSERT_NOT_EQUAL(err, ERROR_TOO_MANY_ARGS);
            CU_ASSERT_NOT_EQUAL(err, ERROR_KEY_NEEDS_ARG);
            CU_ASSERT_NOT_EQUAL(err, ERROR_NO_FREE_STORE);
            CU_ASSERT_NOT_EQUAL(err, ERROR_BAD_NUMBER);
        }

        FreeDosObject(DOS_RDARGS, rdargs);
    }
    else
    {
        CU_FAIL("AllocDosObject(DOS_RDARGS) failed");
    }
}

void test_ReadArgs_ERROR_BAD_TEMPLATE(void)
{
    IPTR args[ARG_CNT];
    struct RDArgs *rdargs;
    STRPTR templ = "FOO==/K/="; // invalid templ
    STRPTR param = "FOO===3 ";

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

        memset(args, 0, sizeof(args));

        struct RDArgs *rda = ReadArgs(templ, args, rdargs);
        CU_ASSERT_PTR_NULL(rda);
        if (!rda)
        {
            LONG err = IoErr();
#if defined(__AROS__)
            CU_ASSERT_EQUAL(err, ERROR_BAD_TEMPLATE);
            CU_ASSERT_NOT_EQUAL(err, ERROR_LINE_TOO_LONG);
#else
// AmigaOS 3.1
            CU_ASSERT_EQUAL(err, ERROR_LINE_TOO_LONG);
            CU_ASSERT_NOT_EQUAL(err, ERROR_BAD_TEMPLATE);
#endif
            CU_ASSERT_NOT_EQUAL(err, ERROR_REQUIRED_ARG_MISSING);
            CU_ASSERT_NOT_EQUAL(err, ERROR_TOO_MANY_ARGS);
            CU_ASSERT_NOT_EQUAL(err, ERROR_KEY_NEEDS_ARG);
            CU_ASSERT_NOT_EQUAL(err, ERROR_NO_FREE_STORE);
            CU_ASSERT_NOT_EQUAL(err, ERROR_BAD_NUMBER);
        }

        FreeDosObject(DOS_RDARGS, rdargs);
    }
    else
    {
        CU_FAIL("AllocDosObject(DOS_RDARGS) failed");
    }
}

void test_ReadArgs_ERROR_TOO_MANY_ARGS(void)
{
    IPTR args[2]; // Intentionally larger to simulate too many args
    struct RDArgs *rdargs;
    STRPTR templ = "FOO/A";
    STRPTR param = "one two"; // More than one arg input

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

        memset(args, 0, sizeof(args));

        struct RDArgs *rda = ReadArgs(templ, args, rdargs);
        CU_ASSERT_PTR_NULL(rda);
        if (!rda)
        {
            LONG err = IoErr();
            CU_ASSERT_EQUAL(err, ERROR_TOO_MANY_ARGS);
            CU_ASSERT_NOT_EQUAL(err, ERROR_REQUIRED_ARG_MISSING);
            CU_ASSERT_NOT_EQUAL(err, ERROR_BAD_TEMPLATE);
            CU_ASSERT_NOT_EQUAL(err, ERROR_LINE_TOO_LONG);
            CU_ASSERT_NOT_EQUAL(err, ERROR_KEY_NEEDS_ARG);
            CU_ASSERT_NOT_EQUAL(err, ERROR_NO_FREE_STORE);
            CU_ASSERT_NOT_EQUAL(err, ERROR_BAD_NUMBER);
        }

        FreeDosObject(DOS_RDARGS, rdargs);
    }
    else
    {
        CU_FAIL("AllocDosObject(DOS_RDARGS) failed");
    }
}

void test_ReadArgs_ERROR_KEY_NEEDS_ARG(void)
{
    IPTR args[2]; // size of 2 required under AmigaOS or will crash
    struct RDArgs *rdargs;
    STRPTR templ = "FOO/A,BAR="; // malformed templ (key needs arg but not optional)
    STRPTR param = "foo";

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

        memset(args, 0, sizeof(args));

        struct RDArgs *rda = ReadArgs(templ, args, rdargs);
        CU_ASSERT_PTR_NULL(rda);
        if (!rda)
        {
            LONG err = IoErr();
#if defined(__AROS__)
            CU_ASSERT_EQUAL(err, ERROR_KEY_NEEDS_ARG);
            CU_ASSERT_NOT_EQUAL(err, ERROR_TOO_MANY_ARGS);
#else
// AmigaOS 3.1
            CU_ASSERT_EQUAL(err, ERROR_TOO_MANY_ARGS);
            CU_ASSERT_NOT_EQUAL(err, ERROR_KEY_NEEDS_ARG);
#endif
            CU_ASSERT_NOT_EQUAL(err, ERROR_REQUIRED_ARG_MISSING);
            CU_ASSERT_NOT_EQUAL(err, ERROR_BAD_TEMPLATE);
            CU_ASSERT_NOT_EQUAL(err, ERROR_LINE_TOO_LONG);
            CU_ASSERT_NOT_EQUAL(err, ERROR_NO_FREE_STORE);
            CU_ASSERT_NOT_EQUAL(err, ERROR_BAD_NUMBER);
        }

        FreeDosObject(DOS_RDARGS, rdargs);
    }
    else
    {
        CU_FAIL("AllocDosObject(DOS_RDARGS) failed");
    }
}

void test_ReadArgs_ERROR_NO_FREE_STORE(void)
{
    IPTR args[ARG_CNT];
    char tmpbuf[4];
    struct RDArgs *rdargs;
    STRPTR templ = "COL=C/N";
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
        rdargs->RDA_Flags = RDAF_NOALLOC;

        struct RDArgs *rda = ReadArgs(templ, args, rdargs);
        CU_ASSERT_PTR_NULL(rda);
        if (!rda)
        {
            LONG err = IoErr();
            CU_ASSERT_EQUAL(err, ERROR_NO_FREE_STORE);
            CU_ASSERT_NOT_EQUAL(err, ERROR_REQUIRED_ARG_MISSING);
            CU_ASSERT_NOT_EQUAL(err, ERROR_BAD_TEMPLATE);
            CU_ASSERT_NOT_EQUAL(err, ERROR_LINE_TOO_LONG);
            CU_ASSERT_NOT_EQUAL(err, ERROR_TOO_MANY_ARGS);
            CU_ASSERT_NOT_EQUAL(err, ERROR_KEY_NEEDS_ARG);
            CU_ASSERT_NOT_EQUAL(err, ERROR_BAD_NUMBER);
        }

        FreeDosObject(DOS_RDARGS, rdargs);
    }
    else
    {
        CU_FAIL("AllocDosObject(DOS_RDARGS) failed");
    }
}

void test_ReadArgs_ERROR_BAD_NUMBER(void)
{
    IPTR args[ARG_CNT];
    struct RDArgs *rdargs;
    STRPTR templ = "NUM/N";
    STRPTR param = "NUM=notanumber";

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

        memset(args, 0, sizeof(args));

        struct RDArgs *rda = ReadArgs(templ, args, rdargs);
        CU_ASSERT_PTR_NULL(rda);
        if (!rda)
        {
            LONG err = IoErr();
            CU_ASSERT_EQUAL(err, ERROR_BAD_NUMBER);
            CU_ASSERT_NOT_EQUAL(err, ERROR_REQUIRED_ARG_MISSING);
            CU_ASSERT_NOT_EQUAL(err, ERROR_BAD_TEMPLATE);
            CU_ASSERT_NOT_EQUAL(err, ERROR_LINE_TOO_LONG);
            CU_ASSERT_NOT_EQUAL(err, ERROR_TOO_MANY_ARGS);
            CU_ASSERT_NOT_EQUAL(err, ERROR_KEY_NEEDS_ARG);
            CU_ASSERT_NOT_EQUAL(err, ERROR_NO_FREE_STORE);
        }

        FreeDosObject(DOS_RDARGS, rdargs);
    }
    else
    {
        CU_FAIL("AllocDosObject(DOS_RDARGS) failed");
    }
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(ReadArgs_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(testREADARGSNUMBER);
    CUNIT_CI_TEST(testREADARGSNUMBERSPACE);
    CUNIT_CI_TEST(test_ReadArgs_ERROR_REQUIRED_ARG_MISSING);
    CUNIT_CI_TEST(test_ReadArgs_ERROR_BAD_TEMPLATE);
    CUNIT_CI_TEST(test_ReadArgs_ERROR_TOO_MANY_ARGS);
    CUNIT_CI_TEST(test_ReadArgs_ERROR_KEY_NEEDS_ARG);
    CUNIT_CI_TEST(test_ReadArgs_ERROR_NO_FREE_STORE);
    CUNIT_CI_TEST(test_ReadArgs_ERROR_BAD_NUMBER);
    CUNIT_CI_TEST(test_ReadArgs_AF);
    return CU_CI_RUN_SUITES();
}
