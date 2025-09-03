#include <stdlib.h>
#include <stdio.h>

#include <proto/mathieeedoubbas.h>

#include <CUnit/CUnitCI.h>


static void test_ieeedpdiv()
{
    double result;
    result = IEEEDPDiv(2.5, 1.5);
    CU_ASSERT(result - 1.67 > -0.05);
    CU_ASSERT(result - 1.67 < 0.05);
}

static void test_ieeedpfix()
{
    LONG result;
    result = IEEEDPFix(0);
    CU_ASSERT_EQUAL(result, 0);

    result = IEEEDPFix(2.5);
    CU_ASSERT_EQUAL(result, 2);

    result = IEEEDPFix(-2.5);
    CU_ASSERT_EQUAL(result, -2);
}

static void test_ieeedpflt()
{
    double result;
    result = IEEEDPFlt(0);
    CU_ASSERT_EQUAL(result, 0.0);

    result = IEEEDPFlt(2);
    CU_ASSERT_EQUAL(result, 2.0);

    result = IEEEDPFlt(-2);
    CU_ASSERT_EQUAL(result, -2.0);
}

static void test_ieeedpmul()
{
    double result;
    result = IEEEDPMul(1.5, 1.67);
    CU_ASSERT(result - 2.5 > -0.05);
    CU_ASSERT(result - 2.5 < 0.05);
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(mathieeedoubbas_Suite, __cu_suite_setup, __cu_suite_teardown, NULL, NULL);
    CUNIT_CI_TEST(test_ieeedpdiv);
    CUNIT_CI_TEST(test_ieeedpfix);
    CUNIT_CI_TEST(test_ieeedpflt);
    CUNIT_CI_TEST(test_ieeedpmul);
    return CU_CI_RUN_SUITES();
}
