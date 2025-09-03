#include <wchar.h>

#include <CUnit/CUnitCI.h>

static void test_wcslen_1() {
    // Test case 1: Basic length
    wchar_t str1[] = L"Hello, World!";
    size_t len1 = wcslen(str1);
    CU_ASSERT_EQUAL(len1, 13); // Length of "Hello, World!" is 13
}

static void test_wcslen_2() {
    // Test case 2: Empty string
    wchar_t str2[] = L"";
    size_t len2 = wcslen(str2);
    CU_ASSERT_EQUAL(len2, 0); // Length of an empty string is 0
}

static void test_wcslen_3() {
    // Test case 3: String with spaces
    wchar_t str3[] = L"   ";
    size_t len3 = wcslen(str3);
    CU_ASSERT_EQUAL(len3, 3); // Length of "   " is 3
}

static void test_wcslen_4() {
    // Test case 4: String with special characters
    wchar_t str4[] = L"Hello, `!";
    size_t len4 = wcslen(str4);
    CU_ASSERT_EQUAL(len4, 9); // Length of "Hello, `!" is 9
}

static void test_wcslen_5() {
    // Test case 5: String with null terminator
    wchar_t str5[] = L"Hello\0World"; // Length should only count until the first null
    size_t len5 = wcslen(str5);
    CU_ASSERT_EQUAL(len5, 5); // Length of "Hello" is 5
}

int main(int argc, char** argv) {
    //Define test suite
    CU_CI_DEFINE_SUITE(wcslen_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_wcslen_1);
    CUNIT_CI_TEST(test_wcslen_2);
    CUNIT_CI_TEST(test_wcslen_3);
    CUNIT_CI_TEST(test_wcslen_4);
    CUNIT_CI_TEST(test_wcslen_5);

    return CU_CI_RUN_SUITES();
}
