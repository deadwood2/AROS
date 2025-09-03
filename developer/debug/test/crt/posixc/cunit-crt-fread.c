/*
    Copyright © 2022, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <stdlib.h>
#include <stdio.h>

#include <CUnit/CUnitCI.h>

const char *filename01 = "T:test_fread_and_feof.txt";
FILE *file01 = NULL;

CU_SUITE_SETUP()
{
    FILE *file;
    file = fopen(filename01, "w");
    if (file == NULL)
        return CUE_SINIT_FAILED;

    fprintf(file, "The answer is 42.");
    fclose(file);

    return CUE_SUCCESS;
}

CU_SUITE_TEARDOWN()
{
    if (file01) fclose(file01);
    remove(filename01);

    return CUE_SUCCESS;
}

static void test_fread_and_feof()
{
    char buf[32];

    file01 = fopen(filename01, "r");
    CU_ASSERT_FATAL(file01 != NULL);

    CU_ASSERT_EQUAL_FATAL(3, fread(buf, 1, 3, file01));
    CU_ASSERT_EQUAL_FATAL(0, feof(file01));

    CU_ASSERT_EQUAL_FATAL(0, fread(buf, 1, 0, file01));
    CU_ASSERT_EQUAL_FATAL(0, feof(file01));

    CU_ASSERT_EQUAL_FATAL(14, fread(buf, 1, 32, file01));
    CU_ASSERT_EQUAL_FATAL(1, feof(file01));
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(fread_Suite, __cu_suite_setup, __cu_suite_teardown, NULL, NULL);
    CUNIT_CI_TEST(test_fread_and_feof);
    return CU_CI_RUN_SUITES();
}
