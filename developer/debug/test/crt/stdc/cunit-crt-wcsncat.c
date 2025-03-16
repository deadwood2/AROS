#include <stdio.h>
#include <wchar.h>
#include <CUnit/CUnitCI.h>

// Function to test wcsncat
static void test_wcsncat_1() {
    wchar_t dest[50] = L"Hello, ";
    wchar_t src[] = L"World!";

    // Test case 1: Normal concatenation
    wchar_t *result = wcsncat(dest, src, 6); // Concatenate "World!"
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT(wcscmp(dest, L"Hello, World!") == 0);

    // Reset dest for the next test
    wcscpy(dest, L"Hello, ");
}
static void test_wcsncat_2() {
    wchar_t dest[50] = L"Hello, ";
    wchar_t src[] = L"World!";

    // Test case 2: Concatenation with limit less than source length
    wchar_t *result = wcsncat(dest, src, 3); // Concatenate "Wor"
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT(wcscmp(dest, L"Hello, Wor") == 0);

    // Reset dest for the next test
    wcscpy(dest, L"Hello, ");
}
static void test_wcsncat_3() {
    wchar_t dest[50] = L"Hello, ";
    wchar_t src[] = L"World!";

    // Test case 3: Concatenation with limit equal to source length
    wchar_t *result = wcsncat(dest, src, 6); // Concatenate "World!"
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT(wcscmp(dest, L"Hello, World!") == 0);

    // Reset dest for the next test
    wcscpy(dest, L"Hello, ");
}
static void test_wcsncat_4() {
    wchar_t dest[50] = L"Hello, ";
    wchar_t src[] = L"World!";

    // Test case 4: Concatenation with limit of 0
    wchar_t *result = wcsncat(dest, src, 0); // Concatenate nothing
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT(wcscmp(dest, L"Hello, ") == 0);
}

// Main function to run the tests
int main(int argc, char** argv) {
    //Define test suite
    CU_CI_DEFINE_SUITE("wcsncat_Suite", NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_wcsncat_1);
    CUNIT_CI_TEST(test_wcsncat_2);
    CUNIT_CI_TEST(test_wcsncat_3);
    CUNIT_CI_TEST(test_wcsncat_4);

    return CU_CI_RUN_SUITES();
}
