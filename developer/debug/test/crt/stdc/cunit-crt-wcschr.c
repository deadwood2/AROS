#include <wchar.h>

#include <CUnit/CUnitCI.h>

static void test_wcschr_1() {
    // Test case 1: Character found in the string
    wchar_t str1[] = L"Hello, World!";
    wchar_t *result1 = wcschr(str1, L'o');
    CU_ASSERT_PTR_EQUAL(result1, &str1[4]); // 'o' is at index 4
}
static void test_wcschr_2() {
    // Test case 2: Character not found in the string
    wchar_t str2[] = L"Hello, World!";
    wchar_t *result2 = wcschr(str2, L'x');
    CU_ASSERT_PTR_EQUAL(result2, NULL); // 'x' is not in the string
}
static void test_wcschr_3() {
    // Test case 3: First character is the target
    wchar_t str3[] = L"Hello, World!";
    wchar_t *result3 = wcschr(str3, L'H');
    CU_ASSERT_PTR_EQUAL(result3, &str3[0]); // 'H' is at index 0
}
static void test_wcschr_4() {
    // Test case 4: Last character is the target
    wchar_t str4[] = L"Hello, World!";
    wchar_t *result4 = wcschr(str4, L'!');
    CU_ASSERT_PTR_EQUAL(result4, &str4[12]); // '!' is at index 12
}
static void test_wcschr_5() {
    // Test case 5: Empty string
    wchar_t str5[] = L"";
    wchar_t *result5 = wcschr(str5, L'H');
    CU_ASSERT_PTR_EQUAL(result5, NULL); // No characters in the string
}
static void test_wcschr_6() {
    // Test case 6: Searching for null terminator
    wchar_t str6[] = L"Hello";
    wchar_t *result6 = wcschr(str6, L'\0');
    CU_ASSERT_PTR_EQUAL(result6, &str6[5]); // Null terminator is at index 5
}
static void test_wcschr_7() {
    // Test case 7: Character appears multiple times
    wchar_t str7[] = L"Hello, World!";
    wchar_t *result7 = wcschr(str7, L'o');
    CU_ASSERT_PTR_EQUAL(result7, &str7[4]); // First 'o' is at index 4
}

int main(int argc, char** argv) {
    //Define test suite
    CU_CI_DEFINE_SUITE(wcschr_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_wcschr_1);
    CUNIT_CI_TEST(test_wcschr_2);
    CUNIT_CI_TEST(test_wcschr_3);
    CUNIT_CI_TEST(test_wcschr_4);
    CUNIT_CI_TEST(test_wcschr_5);
    CUNIT_CI_TEST(test_wcschr_6);
    CUNIT_CI_TEST(test_wcschr_7);

    return CU_CI_RUN_SUITES();
}
