#include <stdio.h>
#include <wchar.h>
#include <CUnit/CUnitCI.h>

// Function to test wcsstr
static void test_wcsstr_1() {
    // Test case 1: Find a substring that exists
    wchar_t str[] = L"Hello, World!";
    wchar_t substr1[] = L"World"; // Substring to find
    wchar_t *result = wcsstr(str, substr1);
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT(wcscmp(result, L"World!") == 0);
}
static void test_wcsstr_2() {
    // Test case 2: Find another substring that exists
    wchar_t str[] = L"Hello, World!";
    wchar_t substr2[] = L"Hello";  // Another substring to find
    wchar_t *result = wcsstr(str, substr2);
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT(wcscmp(result, L"Hello, World!") == 0);
}
static void test_wcsstr_3() {
    // Test case 3: Find a substring that does not exist
    wchar_t str[] = L"Hello, World!";
    wchar_t substr3[] = L"test";    // Substring not in the string
    wchar_t *result = wcsstr(str, substr3);
    CU_ASSERT_PTR_NULL(result);
}
static void test_wcsstr_4() {
    // Test case 4: Find an empty substring
    wchar_t str[] = L"Hello, World!";
    wchar_t substr4[] = L"";        // Empty substring
    wchar_t *result = wcsstr(str, substr4);
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT(wcscmp(result, str) == 0);
}

// Main function to run the tests
int main(int argc, char** argv) {
    //Define test suite
    CU_CI_DEFINE_SUITE("wcsstr_Suite", NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_wcsstr_1);
    CUNIT_CI_TEST(test_wcsstr_2);
    CUNIT_CI_TEST(test_wcsstr_3);
    CUNIT_CI_TEST(test_wcsstr_4);

    return CU_CI_RUN_SUITES();
}
