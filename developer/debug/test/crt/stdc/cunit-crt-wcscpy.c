#include <wchar.h>

#include <CUnit/CUnitCI.h>

static void test_wcscpy_1() {
    // Test case 1: Basic copy
    wchar_t dest1[20];
    wchar_t src1[] = L"Hello, World!";
    wchar_t *result1 = wcscpy(dest1, src1);
    CU_ASSERT_PTR_EQUAL(result1, dest1); // wcscpy returns the destination
    CU_ASSERT_EQUAL(wcscmp(dest1, src1), 0); // Check the result
}

static void test_wcscpy_2() {
    // Test case 2: Copying an empty string
    wchar_t dest2[20];
    wchar_t src2[] = L"";
    wchar_t *result2 = wcscpy(dest2, src2);
    CU_ASSERT_PTR_EQUAL(result2, dest2); // wcscpy returns the destination
    CU_ASSERT_EQUAL(wcscmp(dest2, src2), 0); // Check the result
}

static void test_wcscpy_3() {
    // Test case 3: Copying to an already initialized destination
    wchar_t dest3[20] = L"Initial";
    wchar_t src3[] = L"New String";
    wchar_t *result3 = wcscpy(dest3, src3);
    CU_ASSERT_PTR_EQUAL(result3, dest3); // wcscpy returns the destination
    CU_ASSERT_EQUAL(wcscmp(dest3, src3), 0); // Check the result
}

int main(int argc, char** argv) {
    //Define test suite
    CU_CI_DEFINE_SUITE(wcscpy_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_wcscpy_1);
    CUNIT_CI_TEST(test_wcscpy_2);
    CUNIT_CI_TEST(test_wcscpy_3);

    return CU_CI_RUN_SUITES();
}
