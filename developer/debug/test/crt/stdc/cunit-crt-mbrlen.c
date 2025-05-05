/*
    Copyright © 2025, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <stdio.h>
#include <wchar.h>
#include <CUnit/CUnitCI.h>

static void test_mbrlen_1(void) {

    const char *mbs1 = "A"; // Multibyte character

    // Test length of single-byte character
    size_t len1 = mbrlen(mbs1, MB_CUR_MAX, NULL);
    CU_ASSERT(len1 == 1); // Length should be 1
}

// Main function to run the tests
int main(int argc, char** argv) {
    //Define test suite
    CU_CI_DEFINE_SUITE("mbrlen_Suite", NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_mbrlen_1);

    return CU_CI_RUN_SUITES();
}
