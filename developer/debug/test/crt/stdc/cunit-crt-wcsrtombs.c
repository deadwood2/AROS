/*
    Copyright © 2025, The AROS Development Team. All rights reserved.
    $Id$
*/
#include <stdio.h>
#include <wchar.h>
#include <CUnit/CUnitCI.h>

static void test_wcsrtombs_1(void) {
    const wchar_t *wide_str = L"Hello, World!";
    char multibyte_str[50];
    size_t converted;

    // Clear the multibyte string buffer
    memset(multibyte_str, 0, sizeof(multibyte_str));

    // Convert wide string to multibyte string
    converted = wcsrtombs(multibyte_str, &wide_str, sizeof(multibyte_str) - 1, NULL);

    // Check if the conversion was successful
    CU_ASSERT(converted != (size_t)-1);
    CU_ASSERT_STRING_EQUAL(multibyte_str, "Hello, World!");
}
static void test_wcsrtombs_2(void) {
    // Test with a null pointer for the wide string
    size_t converted;
    converted = wcsrtombs(NULL, NULL, 0, NULL);
    CU_ASSERT(converted == 0); // Should return 0 when input is NULL
}

// Main function to run the tests
int main(int argc, char** argv) {
    //Define test suite
    CU_CI_DEFINE_SUITE(wcsrtombs_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_wcsrtombs_1);
    CUNIT_CI_TEST(test_wcsrtombs_2);

    return CU_CI_RUN_SUITES();
}
