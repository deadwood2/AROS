#include <wchar.h>

#include <CUnit/CUnitCI.h>

static void test_wcscat_1() {
    // Test case 1: Basic concatenation
    wchar_t dest1[20] = L"Hello, ";
    wchar_t src1[] = L"World!";
    wchar_t *result1 = wcscat(dest1, src1);
    CU_ASSERT_PTR_EQUAL(result1, dest1); // wcscat returns the destination
    CU_ASSERT_EQUAL(wcscmp(dest1, L"Hello, World!"), 0); // Check the result
}

static void test_wcscat_2() {
    // Test case 2: Concatenation with empty source
    wchar_t dest2[20] = L"Hello, ";
    wchar_t src2[] = L"";
    wchar_t *result2 = wcscat(dest2, src2);
    CU_ASSERT_PTR_EQUAL(result2, dest2); // wcscat returns the destination
    CU_ASSERT_EQUAL(wcscmp(dest2, L"Hello, "), 0); // Check the result
}

static void test_wcscat_3() {
    // Test case 3: Concatenation with empty destination
    wchar_t dest3[20] = L"";
    wchar_t src3[] = L"World!";
    wchar_t *result3 = wcscat(dest3, src3);
    CU_ASSERT_PTR_EQUAL(result3, dest3); // wcscat returns the destination
    CU_ASSERT_EQUAL(wcscmp(dest3, L"World!"), 0); // Check the result
}

int main(int argc, char** argv) {
    //Define test suite
    CU_CI_DEFINE_SUITE(wcscat_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_wcscat_1);
    CUNIT_CI_TEST(test_wcscat_2);
    CUNIT_CI_TEST(test_wcscat_3);

    return CU_CI_RUN_SUITES();
}
