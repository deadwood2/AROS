/*
    Copyright © 2024, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <wchar.h>

#include <CUnit/CUnitCI.h>


static void test_wcsncpy_3()
{
    const wchar_t *src = L"ABC";
    wchar_t dst[6] = { L'Z', L'Z', L'Z', L'Z', L'Z', L'Z' };

    wcsncpy(dst, src, 3);

    CU_ASSERT_EQUAL(dst[0], (wchar_t)'A');
    CU_ASSERT_EQUAL(dst[1], (wchar_t)'B');
    CU_ASSERT_EQUAL(dst[2], (wchar_t)'C');
    CU_ASSERT_EQUAL(dst[3], (wchar_t)'Z');
    CU_ASSERT_EQUAL(dst[4], (wchar_t)'Z');
    CU_ASSERT_EQUAL(dst[5], (wchar_t)'Z');
}

static void test_wcsncpy_4()
{
    const wchar_t *src = L"ABC";
    wchar_t dst[6] = { L'Z', L'Z', L'Z', L'Z', L'Z', L'Z' };

    wcsncpy(dst, src, 4);

    CU_ASSERT_EQUAL(dst[0], (wchar_t)'A');
    CU_ASSERT_EQUAL(dst[1], (wchar_t)'B');
    CU_ASSERT_EQUAL(dst[2], (wchar_t)'C');
    CU_ASSERT_EQUAL(dst[3], (wchar_t)'\0');
    CU_ASSERT_EQUAL(dst[4], (wchar_t)'Z');
    CU_ASSERT_EQUAL(dst[5], (wchar_t)'Z');
}

static void test_wcsncpy_0()
{
    const wchar_t *src = L"ABC";
    wchar_t dst[6] = { L'Z', L'Z', L'Z', L'Z', L'Z', L'Z' };

    wcsncpy(dst, src, 0);

    CU_ASSERT_EQUAL(dst[0], (wchar_t)'Z');
    CU_ASSERT_EQUAL(dst[1], (wchar_t)'Z');
    CU_ASSERT_EQUAL(dst[2], (wchar_t)'Z');
    CU_ASSERT_EQUAL(dst[3], (wchar_t)'Z');
    CU_ASSERT_EQUAL(dst[4], (wchar_t)'Z');
    CU_ASSERT_EQUAL(dst[5], (wchar_t)'Z');
}

static void test_wcsncpy_5()
{
    const wchar_t *src = L"ABC";
    wchar_t dst[6] = { L'Z', L'Z', L'Z', L'Z', L'Z', L'Z' };

    wcsncpy(dst, src, 5);

    CU_ASSERT_EQUAL(dst[0], (wchar_t)'A');
    CU_ASSERT_EQUAL(dst[1], (wchar_t)'B');
    CU_ASSERT_EQUAL(dst[2], (wchar_t)'C');
    CU_ASSERT_EQUAL(dst[3], (wchar_t)'\0');
    CU_ASSERT_EQUAL(dst[4], (wchar_t)'\0');
    CU_ASSERT_EQUAL(dst[5], (wchar_t)'Z');
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(wcsncpy_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_wcsncpy_3);
    CUNIT_CI_TEST(test_wcsncpy_4);
    CUNIT_CI_TEST(test_wcsncpy_0);
    CUNIT_CI_TEST(test_wcsncpy_5);

    return CU_CI_RUN_SUITES();
}

