#include <stdio.h>
#include <wchar.h>
#include <CUnit/CUnitCI.h>

// Function to test wmemchr
static void test_wmemchr_1() {
    wchar_t arr[] = L"Hello, World!";
    
    // Test case 1: Find a character that exists
    wchar_t *result = wmemchr(arr, L'o', wcslen(arr));
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT(wcscmp(result, L"o, World!") == 0);

    // Test case 2: Find a character that exists at the beginning
    result = wmemchr(arr, L'H', wcslen(arr));
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT(wcscmp(result, L"Hello, World!") == 0);

    // Test case 3: Find a character that does not exist
    result = wmemchr(arr, L'x', wcslen(arr));
    CU_ASSERT_PTR_NULL(result);

    // Test case 4: Find a character at the end
    result = wmemchr(arr, L'!', wcslen(arr));
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT(wcscmp(result, L"!") == 0);
}

// Main function to run the tests
int main(int argc, char** argv) {
    //Define test suite
    CU_CI_DEFINE_SUITE(wmemchr_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_wmemchr_1);

    return CU_CI_RUN_SUITES();
}
