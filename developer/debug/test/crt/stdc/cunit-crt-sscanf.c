/*
    Copyright © 2022, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <stdlib.h>

#include <CUnit/CUnitCI.h>

#define TESTSTRING1 "1.3 1 string"
#define TESTSTRING2 "NO_NUMBERS_TEXT"
#define TESTSTRING3 "FSOMETHING"
#define TESTSTRING4 "0xAF"              /* Hex integer */
#define TESTSTRING5 "xAF"               /* "Hex integer" without 0 */
#define TESTSTRING6 "AF"                /* "Hex integer" without 0x */

static void test_integer()
{
    int i, cnt;

    i = 123456;
    cnt = sscanf(TESTSTRING2, "%i", &i);
    CU_ASSERT_EQUAL(0, cnt);
    CU_ASSERT_EQUAL(123456, i);

    i = 123456;
    cnt = sscanf(TESTSTRING3, "%i", &i);
    CU_ASSERT_EQUAL(0, cnt);
    CU_ASSERT_EQUAL(123456, i);

    i = 123456;
    cnt = sscanf(TESTSTRING4, "%i", &i);
    CU_ASSERT_EQUAL(1, cnt);
    CU_ASSERT_EQUAL(0xaf, i);

    i = 123456;
    cnt = sscanf(TESTSTRING5, "%i", &i);
    CU_ASSERT_EQUAL(0, cnt);
    CU_ASSERT_EQUAL(123456, i);

    i = 123456;
    cnt = sscanf(TESTSTRING6, "%i", &i);
    CU_ASSERT_EQUAL(0, cnt);
    CU_ASSERT_EQUAL(123456, i);
}

static void test_multi()
{
    char s[10];
    int i, cnt;
    float f;

    cnt = sscanf(TESTSTRING1, "%f %d %s", &f, &i, s);
    CU_ASSERT_EQUAL(3, cnt);
    CU_ASSERT_DOUBLE_EQUAL(1.3f, f, 0.01f);
    CU_ASSERT_EQUAL(1, i)
    CU_ASSERT_STRING_EQUAL("string", s);
}

static void test_float()
{
    int cnt;
    float f;
    cnt = sscanf("0.1", "%f", &f);
    CU_ASSERT_EQUAL(1, cnt);
    CU_ASSERT_DOUBLE_EQUAL(0.1f, f, 0.01f);

    cnt = sscanf(".1", "%f", &f);
    CU_ASSERT_EQUAL(1, cnt);
    CU_ASSERT_DOUBLE_EQUAL(0.1f, f, 0.01f);

    cnt = sscanf("1", "%f", &f);
    CU_ASSERT_EQUAL(1, cnt);
    CU_ASSERT_DOUBLE_EQUAL(1.0f, f, 0.01f);

    cnt = sscanf("-.1", "%f", &f);
    CU_ASSERT_EQUAL(1, cnt);
    CU_ASSERT_DOUBLE_EQUAL(-0.1f, f, 0.01f);

    cnt = sscanf("x", "%f", &f);
    CU_ASSERT_EQUAL(0, cnt);
}

static void test_unsigned()
{
    unsigned char hh;
    unsigned short h;
    unsigned long l;
    unsigned long long ll;
    unsigned int u;
    int cnt;

    cnt = sscanf("10", "%hhu", &hh);
    CU_ASSERT_EQUAL(1, cnt);
    CU_ASSERT_EQUAL(10, hh);

    cnt = sscanf("1010", "%hu", &h);
    CU_ASSERT_EQUAL(1, cnt);
    CU_ASSERT_EQUAL(1010, h);

#if (__WORDSIZE == 64)
    cnt = sscanf("100000000010", "%lu", &l);
    CU_ASSERT_EQUAL(1, cnt);
    CU_ASSERT_EQUAL(100000000010, l);
#else
    cnt = sscanf("10000010", "%lu", &l);
    CU_ASSERT_EQUAL(1, cnt);
    CU_ASSERT_EQUAL(10000010, l);
#endif

    cnt = sscanf("100000000010", "%llu", &ll);
    CU_ASSERT_EQUAL(1, cnt);
    CU_ASSERT_EQUAL(100000000010, ll);

    cnt = sscanf("10000010", "%u", &u);
    CU_ASSERT_EQUAL(1, cnt);
    CU_ASSERT_EQUAL(10000010, u);
}

static void test_signed()
{
    signed char hh;
    signed short h;
    signed long l;
    signed long long ll;
    signed int u;
    int cnt;

    cnt = sscanf("-10", "%hhd", &hh);
    CU_ASSERT_EQUAL(1, cnt);
    CU_ASSERT_EQUAL(-10, hh);

    cnt = sscanf("-1010", "%hd", &h);
    CU_ASSERT_EQUAL(1, cnt);
    CU_ASSERT_EQUAL(-1010, h);

#if (__WORDSIZE == 64)
    cnt = sscanf("-100000000010", "%ld", &l);
    CU_ASSERT_EQUAL(1, cnt);
    CU_ASSERT_EQUAL(-100000000010, l);
#else
    cnt = sscanf("-10000010", "%ld", &l);
    CU_ASSERT_EQUAL(1, cnt);
    CU_ASSERT_EQUAL(-10000010, l);
#endif

    cnt = sscanf("-100000000010", "%lld", &ll);
    CU_ASSERT_EQUAL(1, cnt);
    CU_ASSERT_EQUAL(-100000000010, ll);

    cnt = sscanf("-10000010", "%d", &u);
    CU_ASSERT_EQUAL(1, cnt);
    CU_ASSERT_EQUAL(-10000010, u);
}

static void test_ignore()
{
    char path[100];
    int cnt = sscanf("file:///bookmarks.html", "%*15[^\n/:]:%[^\n]", path);

    CU_ASSERT_EQUAL_FATAL(cnt, 1);
    CU_ASSERT_STRING_EQUAL(path, "///bookmarks.html");

}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(sscanf_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_integer);
    CUNIT_CI_TEST(test_multi);
    CUNIT_CI_TEST(test_float);
    CUNIT_CI_TEST(test_unsigned);
    CUNIT_CI_TEST(test_signed);
    CUNIT_CI_TEST(test_ignore);

    return CU_CI_RUN_SUITES();
}
