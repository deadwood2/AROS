/*
    Copyright (C) 2022, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dostags.h>
#include <dos/dos.h>

#include <CUnit/CUnitCI.h>

#include "../test-util.h"

static LONG stack_cliDefaultStack_Original;

CU_SUITE_SETUP()
{
    struct CommandLineInterface *cli = Cli();
    stack_cliDefaultStack_Original = cli->cli_DefaultStack;
    cli->cli_DefaultStack = 50000 / CLI_DEFAULTSTACK_UNIT;

    return CUE_SUCCESS;
}

CU_SUITE_TEARDOWN()
{
    struct CommandLineInterface *cli = Cli();
    cli->cli_DefaultStack = stack_cliDefaultStack_Original;

    return CUE_SUCCESS;
}

static void test_passing_stack_to_program()
{
    char buff[64];
    STRPTR slavepath = CUNIT_ABSOLUTE_PATH "/dos/cunit-dos-systemtags-stack-slave";
    LONG res;
    BPTR file;

    res = SystemTags(slavepath, NP_StackSize, 100000, TAG_DONE);
    CU_ASSERT_EQUAL_FATAL(res, 0);

    file = Open("T:systemtags-stack-slave.txt", MODE_OLDFILE);
    CU_ASSERT_NOT_EQUAL_FATAL(file, BNULL);

    FGets(file, buff, 64);
    Close(file);

    sscanf(buff,"%d", &res);

    CU_ASSERT_EQUAL(res, 100000);
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE("SystemTags_Suite", __cu_suite_setup, __cu_suite_teardown, NULL, NULL);
    CUNIT_CI_TEST(test_passing_stack_to_program);


    return CU_CI_RUN_SUITES();
}
