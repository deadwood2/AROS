/*
    Copyright © 2022, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <stdlib.h>

#include <CUnit/CUnitCI.h>

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

static void test_sprintf_float_double()
{
    char buffer[50];
    float xf = 3.14;
    double xd = 6.28;

    sprintf(buffer, "%6.3f %6.3f", xf, xd);
    CU_ASSERT_EQUAL(0, strcmp(buffer, " 3.140  6.280"));

    xd = 2;
    sprintf(buffer, "%a", xd);
    CU_ASSERT_EQUAL(0, strcmp(buffer, "0x1p+1"));

    xd = 256;
    sprintf(buffer, "%a", xd);
    CU_ASSERT_EQUAL(0, strcmp(buffer, "0x1p+8"));

    xd = 0.015625; //= 2^-6
    sprintf(buffer, "%a", xd);
    CU_ASSERT_EQUAL(0, strcmp(buffer, "0x1p-6"));

    xd = 0.857421875;
    sprintf(buffer, "%a", xd);
    CU_ASSERT_EQUAL(0, strcmp(buffer, "0x1.b7p-1"));

    xd = 0x1p-1074; //Smallest double (unnormalized)
    sprintf(buffer, "%a", xd);
    CU_ASSERT_EQUAL(0, strcmp(buffer, "0x0.0000000000001p-1022"));

    xd = 3.1415926;
    sprintf(buffer, "%A", xd);
    CU_ASSERT_EQUAL(0, strcmp(buffer, "0X1.921FB4D12D84AP+1"));

    xd = 0.1;
    sprintf(buffer, "%a", xd);
    CU_ASSERT_EQUAL(0, strcmp(buffer, "0x1.999999999999ap-4"));

    xd = 0x3.3333333333334p-5;
    sprintf(buffer, "%a", xd);
    CU_ASSERT_EQUAL(0, strcmp(buffer, "0x1.999999999999ap-4"));

    xd = 0xcc.ccccccccccdp-11;
    sprintf(buffer, "%a", xd);
    CU_ASSERT_EQUAL(0, strcmp(buffer, "0x1.999999999999ap-4"));
}

#define TESTSTRING1 "test"
#define TESTSTRING2 "123456789"
#define TESTSTRING3 "0123456789"
#define TESTSTRING4 "a long test string"

#define BUFSIZE 10

static void test_snprintf()
{
    char buf[BUFSIZE + 1] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

    /* first check strings shorter than buffer */
    CU_ASSERT_EQUAL(strlen(TESTSTRING1), snprintf(buf, BUFSIZE, "%s", TESTSTRING1));
    CU_ASSERT_EQUAL(0, buf[strlen(TESTSTRING1)]);
    CU_ASSERT_EQUAL((char) 0xff, buf[strlen(TESTSTRING1) + 1]);

    /* now strings with length equal to buffer size - 1 */
    CU_ASSERT_EQUAL(strlen(TESTSTRING2), snprintf(buf, BUFSIZE, "%s", TESTSTRING2));
    CU_ASSERT_EQUAL(0, buf[strlen(TESTSTRING2)]);
    CU_ASSERT_EQUAL((char) 0xff, buf[BUFSIZE]);

    /* now strings with length equal to buffer size (no zero byte written) */
    CU_ASSERT_EQUAL(strlen(TESTSTRING3), snprintf(buf, BUFSIZE, "%s", TESTSTRING3));
    CU_ASSERT_NOT_EQUAL(TESTSTRING3[strlen(TESTSTRING3)-1], buf[BUFSIZE-1]);
    CU_ASSERT_EQUAL(0, buf[BUFSIZE-1]);
    CU_ASSERT_EQUAL((char) 0xff, buf[BUFSIZE]);

    /* now strings longer than buffer size */
    CU_ASSERT_EQUAL(strlen(TESTSTRING4), snprintf(buf, BUFSIZE, "%s", TESTSTRING4));
    CU_ASSERT_NOT_EQUAL(TESTSTRING4[BUFSIZE-1], buf[BUFSIZE-1]);
    CU_ASSERT_EQUAL(0, buf[BUFSIZE-1]);
    CU_ASSERT_EQUAL((char) 0xff, buf[BUFSIZE]);
}

int __sprintf_StdlibBase_stack_varargs_cnt = 18;

static void test_sprintf_long_varargs()
{
    char buffer[50];
    sprintf(buffer, "%d %d", 1, 2);
    CU_ASSERT_EQUAL(0, strcmp(buffer, "1 2"));

    sprintf(buffer, "%d %d %d %d", 1, 2, 3, 4);
    CU_ASSERT_EQUAL(0, strcmp(buffer, "1 2 3 4"));

    sprintf(buffer,"%d %d %d %d %d %d %d %d %d", 1, 2, 3, 4, 5, 6, 7, 8, 9);
    CU_ASSERT_EQUAL(0, strcmp(buffer, "1 2 3 4 5 6 7 8 9"));

    sprintf(buffer, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20);
    CU_ASSERT_EQUAL(0, strcmp(buffer, "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20"));

    sprintf(buffer, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22);
    CU_ASSERT_EQUAL(0, strcmp(buffer, "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22"));
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE("sxprintf_Suite", __cu_suite_setup, __cu_suite_teardown, __cu_test_setup, __cu_test_teardown);
    CUNIT_CI_TEST(test_sprintf_float_double);
    CUNIT_CI_TEST(test_snprintf);
    CUNIT_CI_TEST(test_sprintf_long_varargs);

    return CU_CI_RUN_SUITES();
}
