#include <stdio.h>
#include <wchar.h>
#include <CUnit/CUnitCI.h>

// Function to test wmemcmp
static void test_wmemcmp_1() {
    wchar_t arr1[] = L"Hello, World!";
    wchar_t arr2[] = L"Upper, Key";
    wchar_t arr3[] = L"Lower, Key";
    wchar_t arr4[] = L"Hello, World!"; // Same as arr1 for comparison

    // Test case 1: Compare two identical wide strings
    int result = wmemcmp(arr1, arr4, wcslen(arr1));
    CU_ASSERT(result == 0); // Strings are equal

    // Test case 2: Compare different wide strings
    result = wmemcmp(arr1, arr3, wcslen(arr1));
    CU_ASSERT(result < 0); // arr1 is less than arr3

    // Test case 3: Compare different wide strings of same length
    result = wmemcmp(arr2, arr3, wcslen(arr2));
    CU_ASSERT(result > 0); // arr2 is more than arr3

    // Test case 4: Compare with a different length
    result = wmemcmp(arr1, arr3, wcslen(arr1) - 1);
    CU_ASSERT(result < 0); // arr1 is less than arr3 when comparing with length - 1
}

// Main function to run the tests
int main(int argc, char** argv) {
    //Define test suite
    CU_CI_DEFINE_SUITE(wmemcmp_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_wmemcmp_1);

    return CU_CI_RUN_SUITES();
}

