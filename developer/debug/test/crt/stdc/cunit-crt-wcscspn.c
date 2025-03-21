#include <stdio.h>
#include <wchar.h>
#include <CUnit/CUnitCI.h>

// Function to test wcscspn
static void test_wcscspn_1() {
    wchar_t str[] = L"Hello, World!";
    wchar_t reject1[] = L"aeiou";   // Vowels to search for
    wchar_t reject2[] = L"H";       // Character to match
    wchar_t reject3[] = L"xyz";     // Characters not in the string
    wchar_t reject4[] = L"";        // Empty reject string

    // Test case 1: Count characters until a vowel is found
    size_t result = wcscspn(str, reject1);
    CU_ASSERT(result == 1); // 'H' is the first character not in "aeiou"

    // Test case 2: Count characters until a character in 'reject' is found
    result = wcscspn(str, reject2);
    CU_ASSERT(result == 0); // "H" will reject all the wide string

    // Test case 3: Count with no matching characters
    result = wcscspn(str, reject3);
    CU_ASSERT(result == wcslen(str)); // All characters are counted

    // Test case 4: Count with empty reject string
    result = wcscspn(str, reject4);
    CU_ASSERT(result == wcslen(str)); // All characters are counted
}

// Main function to run the tests
int main(int argc, char** argv) {
    //Define test suite
    CU_CI_DEFINE_SUITE("wcscspn_Suite", NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_wcscspn_1);

    return CU_CI_RUN_SUITES();
}