#include <stdio.h>
#include <wchar.h>
#include <CUnit/CUnitCI.h>

// Function to test wcspbrk
static void test_wcspbrk_1() {
    // Test case 1: Find first occurrence of any vowel
    wchar_t str[] = L"Hello, World!";
    wchar_t accept[] = L"aeiou"; // Vowels to search for
    wchar_t *result = wcspbrk(str, accept);
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT(wcscmp(result, L"ello, World!") == 0);
}
static void test_wcspbrk_2() {
    // Test case 2: Find first occurrence of a character not in the string
    wchar_t str[] = L"Hello, World!";
    wchar_t accept2[] = L"xyz"; // Characters not in the string
    wchar_t *result = wcspbrk(str, accept2);
    CU_ASSERT_PTR_NULL(result);
}
static void test_wcspbrk_3() {
    // Test case 3: Find first occurrence of a character at the beginning
    wchar_t str[] = L"Hello, World!";
    wchar_t accept3[] = L"H"; // Character at the beginning
    wchar_t *result = wcspbrk(str, accept3);
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT(wcscmp(result, L"Hello, World!") == 0);
}
static void test_wcspbrk_4() {
    // Test case 4: Find first occurrence of a character at the end
    wchar_t str[] = L"Hello, World!";
    wchar_t accept4[] = L"!"; // Character at the end
    wchar_t *result = wcspbrk(str, accept4);
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT(wcscmp(result, L"!") == 0);
}

// Main function to run the tests
int main(int argc, char** argv) {
    //Define test suite
    CU_CI_DEFINE_SUITE(wcspbrk_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_wcspbrk_1);
    CUNIT_CI_TEST(test_wcspbrk_2);
    CUNIT_CI_TEST(test_wcspbrk_3);
    CUNIT_CI_TEST(test_wcspbrk_4);

    return CU_CI_RUN_SUITES();
}
