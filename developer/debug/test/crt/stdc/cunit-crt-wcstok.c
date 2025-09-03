#include <stdio.h>
#include <wchar.h>
#include <CUnit/CUnitCI.h>

// Function to test wcstok
static void test_wcstok_1() {
    wchar_t str[] = L"Hello, World! This is a test.";
    wchar_t *token;
    wchar_t *rest = str; // Pointer to keep track of the remaining string

    // Test case 1: Tokenize the string using space as a delimiter
    token = wcstok(rest, L" ", &rest);
    CU_ASSERT_PTR_NOT_NULL(token);
    CU_ASSERT(wcscmp(token, L"Hello,") == 0);

    // Test case 2: Get the next token "World!"
    token = wcstok(NULL, L" ", &rest);
    CU_ASSERT_PTR_NOT_NULL(token);
    CU_ASSERT(wcscmp(token, L"World!") == 0);

    // Test case 3: Get the next token "This"
    token = wcstok(NULL, L" ", &rest);
    CU_ASSERT_PTR_NOT_NULL(token);
    CU_ASSERT(wcscmp(token, L"This") == 0);
   
    // Test case 4: Get the next token "is"
    token = wcstok(NULL, L" ", &rest);
    CU_ASSERT_PTR_NOT_NULL(token);
    CU_ASSERT(wcscmp(token, L"is") == 0);

    // Test case 5: Get the next token "a"
    token = wcstok(NULL, L" ", &rest);
    CU_ASSERT_PTR_NOT_NULL(token);
    CU_ASSERT(wcscmp(token, L"a") == 0);

    // Test case 6: Get the next token "test"
    token = wcstok(NULL, L" ", &rest);
    CU_ASSERT_PTR_NOT_NULL(token);
    CU_ASSERT(wcscmp(token, L"test.") == 0);

    // Test case 7: No more tokens
    token = wcstok(NULL, L" ", &rest);
    CU_ASSERT_PTR_NULL(token);
}

// Main function to run the tests
int main(int argc, char** argv) {
    //Define test suite
    CU_CI_DEFINE_SUITE(wcstok_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_wcstok_1);

    return CU_CI_RUN_SUITES();
}