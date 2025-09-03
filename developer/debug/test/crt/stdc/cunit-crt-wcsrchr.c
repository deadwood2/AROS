#include <stdio.h>
#include <wchar.h>
#include <CUnit/CUnitCI.h>

// Function to test wcsrchr
static void test_wcsrchr_1() {
    wchar_t str[] = L"Hello, World!";
    
    // Test case 1: Find last occurrence of character 'o'
    wchar_t *result = wcsrchr(str, L'o');
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT(wcscmp(result, L"orld!") == 0);
}
static void test_wcsrchr_2() {
    wchar_t str[] = L"Hello, World!";
    // Test case 2: Find last occurrence of character 'W'
    wchar_t *result = wcsrchr(str, L'W');
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT(wcscmp(result, L"World!") == 0);
}
static void test_wcsrchr_3() {
    wchar_t str[] = L"Hello, World!";
    // Test case 3: Find character 'x' (not in string)
    wchar_t *result = wcsrchr(str, L'x');
    CU_ASSERT_PTR_NULL(result);
}
static void test_wcsrchr_4() {
    wchar_t str[] = L"Hello, World!";
    // Test case 4: Find last occurrence of character '!'
    wchar_t *result = wcsrchr(str, L'!');
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT(wcscmp(result, L"!") == 0);
}

// Main function to run the tests
int main(int argc, char** argv) {
    //Define test suite
    CU_CI_DEFINE_SUITE(wcsrchr_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_wcsrchr_1);
    CUNIT_CI_TEST(test_wcsrchr_2);
    CUNIT_CI_TEST(test_wcsrchr_3);
    CUNIT_CI_TEST(test_wcsrchr_4);

    return CU_CI_RUN_SUITES();
}
