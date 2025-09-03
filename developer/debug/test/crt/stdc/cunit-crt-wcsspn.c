#include <stdio.h>
#include <wchar.h>
#include <CUnit/CUnitCI.h>

// Function to test wcsspn
static void test_wcsspn_1() {
    wchar_t str[] = L"129th";
    wchar_t accept[] = L"1234567890"; // Digits to match

    // Test case 1: Count digits from the beginning that are in 'accept'
    size_t result = wcsspn(str, accept);
    CU_ASSERT(result == 3); // "129th" has 3 digits
}
static void test_wcsspn_2() {
    // Test case 2: Count with no matching characters at the beginning
    wchar_t str[] = L"Hello, World!";
    wchar_t accept2[] = L"xyz"; // No characters in the string
    size_t result = wcsspn(str, accept2);
    CU_ASSERT(result == 0); // Expected count is 0
}
static void test_wcsspn_3() {
    // Test case 3: Count with some matching characters at the beginning
    wchar_t str[] = L"Hello, World!";
    wchar_t accept3[] = L"He"; // Only 'H' and 'e' are accepted
    size_t result = wcsspn(str, accept3);
    CU_ASSERT(result == 2); // "He" has 2 characters
}
static void test_wcsspn_4() {
    // Test case 4: Count with empty accept string
    wchar_t str[] = L"Hello, World!";
    wchar_t accept4[] = L""; // Empty accept string
    size_t result = wcsspn(str, accept4);
    CU_ASSERT(result == 0); // Expected count is 0
}

// Main function to run the tests
int main(int argc, char** argv) {
    //Define test suite
    CU_CI_DEFINE_SUITE(wcsspn_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_wcsspn_1);
    CUNIT_CI_TEST(test_wcsspn_2);
    CUNIT_CI_TEST(test_wcsspn_3);
    CUNIT_CI_TEST(test_wcsspn_4);

    return CU_CI_RUN_SUITES();
}
