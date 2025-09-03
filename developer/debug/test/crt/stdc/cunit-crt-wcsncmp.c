#include <wchar.h>

#include <CUnit/CUnitCI.h>

static void test_wcsncmp_1() {
    // Test case 1: Basic comparison
    wchar_t str1[] = L"Hello";
    wchar_t str2[] = L"Hello";
    int result1 = wcsncmp(str1, str2, 5);
    CU_ASSERT_EQUAL(result1, 0); // Strings are equal
}

static void test_wcsncmp_2() {

    // Test case 2: Different strings
    wchar_t str3[] = L"Hello";
    wchar_t str4[] = L"World";
    int result2 = wcsncmp(str3, str4, 5);
    CU_ASSERT(result2 < 0); // "Hello" is less than "World"
}

static void test_wcsncmp_3() {
    // Test case 3: Different lengths
    wchar_t str5[] = L"Hello";
    wchar_t str6[] = L"Hello, World!";
    int result3 = wcsncmp(str5, str6, 5);
    CU_ASSERT_EQUAL(result3, 0); // First 5 characters are equal
}

static void test_wcsncmp_4() {
    // Test case 4: Compare with different number of characters
    wchar_t str7[] = L"Hello";
    wchar_t str8[] = L"Hel";
    int result4 = wcsncmp(str7, str8, 3);
    CU_ASSERT_EQUAL(result4, 0); // First 3 characters are equal
}

static void test_wcsncmp_5() {
    // Test case 5: Compare with null terminator
    wchar_t str9[] = L"Hello\0World";
    wchar_t str10[] = L"Hello";
    int result5 = wcsncmp(str9, str10, 6);
    CU_ASSERT_EQUAL(result5, 0); // First 6 characters are equal (including null)
}

static void test_wcsncmp_6() {
    // Test case 6: Different lengths reversed
    wchar_t str11[] = L"Hello, World";
    wchar_t str12[] = L"Hello";
    int result6 = wcsncmp(str11, str12, 6);
    CU_ASSERT(result6 > 0); // First 6 characters are not equal
}

int main(int argc, char** argv) {
    //Define test suite
    CU_CI_DEFINE_SUITE(wcsncmp_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_wcsncmp_1);
    CUNIT_CI_TEST(test_wcsncmp_2);
    CUNIT_CI_TEST(test_wcsncmp_3);
    CUNIT_CI_TEST(test_wcsncmp_4);
    CUNIT_CI_TEST(test_wcsncmp_5);
    CUNIT_CI_TEST(test_wcsncmp_6);

    return CU_CI_RUN_SUITES();
}
