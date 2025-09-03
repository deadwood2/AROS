/*
    Copyright © 2025, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <stdio.h>
#include <wchar.h>
#include <CUnit/CUnitCI.h>

static void test_mbrtowc_1(void) {

    const char *mbs = "A"; // Multibyte character
    wchar_t wc;
    size_t result;

    // Convert multibyte character to wide character
    result = mbrtowc(&wc, mbs, MB_CUR_MAX, NULL);

    // Check if the conversion was successful
    CU_ASSERT(result != (size_t)-1);
    CU_ASSERT(wc == L'A'); // Check if the wide character is correct
}

// Main function to run the tests
int main(int argc, char** argv) {
    //Define test suite
    CU_CI_DEFINE_SUITE(mbrtowc_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_mbrtowc_1);

    return CU_CI_RUN_SUITES();
}
