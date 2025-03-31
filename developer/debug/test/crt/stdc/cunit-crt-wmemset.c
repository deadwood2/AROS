#include <stdio.h>
#include <wchar.h>
#include <CUnit/CUnitCI.h>

// Function to test wmemset
static void test_wmemset_1() {
    wchar_t wcs[] = L"almost every programmer should know wmemset!";
    wchar_t *result = wmemset(wcs,L'-',6);
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT(wcscmp(result, L"------ every programmer should know wmemset!")== 0);
}
// Main function to run the tests
int main(int argc, char** argv) {
    //Define test suite
    CU_CI_DEFINE_SUITE("wmemset_Suite", NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_wmemset_1);
    
    return CU_CI_RUN_SUITES();
}
    
