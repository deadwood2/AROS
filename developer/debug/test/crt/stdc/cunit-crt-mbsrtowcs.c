/*
    Copyright © 2025, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <stdio.h>
#include <wchar.h>
#include <CUnit/CUnitCI.h>

static void test_mbsrtowcs_1(void) {

    const char *mbs = "Hello, World!";
    wchar_t wcs[50];
    size_t converted;

    // Convert multibyte string to wide character string
    converted = mbsrtowcs(wcs, &mbs, sizeof(wcs)/sizeof(wchar_t), NULL);

    // Check if the conversion was successful
    CU_ASSERT(converted != (size_t)-1);
    CU_ASSERT(wcs[converted] == L'\0'); // Ensure null termination

    // Check the content of the wide character string
    wchar_t expected[] = L"Hello, World!";
    CU_ASSERT(wcscmp(wcs, expected) == 0);
}

// Main function to run the tests
int main(int argc, char** argv) {
    //Define test suite
    CU_CI_DEFINE_SUITE(mbsrtowcs_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_mbsrtowcs_1);

    return CU_CI_RUN_SUITES();
}
