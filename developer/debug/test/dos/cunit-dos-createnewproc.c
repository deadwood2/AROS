/*
    Copyright (C) 2022, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dos.h>
#include <dos/dostags.h>

#include <CUnit/CUnitCI.h>

#ifndef __AROS__
#define IPTR ULONG
#define CLI_DEFAULTSTACK_UNIT sizeof(IPTR)
typedef struct RAWARG_s { } *RAWARG;
#endif

static BYTE sig;
static struct Task *task;

static LONG stack_prStackSize;
static LONG stack_cliDefaultStack;
static LONG stack_tcSPUpper_tcSPLower;

static LONG stack_prStackSize_Initial;
static LONG stack_cliDefaultStack_Initial;
static LONG stack_tcSPUpper_tcSPLower_Initial;

static LONG stack_cliDefaultStack_Original;

static void save_stack_values()
{
    struct CommandLineInterface *cli;
    struct Process *me;

    me = (struct Process *)FindTask(NULL);
    cli = Cli();

    stack_tcSPUpper_tcSPLower   = (LONG)(((UBYTE *)me->pr_Task.tc_SPUpper) - ((UBYTE *)me->pr_Task.tc_SPLower));
    stack_prStackSize           = me->pr_StackSize;
    stack_cliDefaultStack       = cli ? cli->cli_DefaultStack * CLI_DEFAULTSTACK_UNIT: -1;

}

static void clear_stack_values()
{
    stack_prStackSize           = 0;
    stack_cliDefaultStack       = 0;
    stack_tcSPUpper_tcSPLower   = 0;
}

static void SubProcess(void)
{
    struct CommandLineInterface *cli;
    struct Process *me;

    save_stack_values();

    Signal(task, 1<<sig);
}

CU_SUITE_SETUP()
{
    struct CommandLineInterface *cli = Cli();
    stack_cliDefaultStack_Original = cli->cli_DefaultStack;
    cli->cli_DefaultStack = 50000 / CLI_DEFAULTSTACK_UNIT;

    sig = AllocSignal(-1);
    task = FindTask(NULL);

    save_stack_values();
    stack_tcSPUpper_tcSPLower_Initial   = stack_tcSPUpper_tcSPLower;
    stack_prStackSize_Initial           = stack_prStackSize;
    stack_cliDefaultStack_Initial       = stack_cliDefaultStack;

    return CUE_SUCCESS;
}

CU_SUITE_TEARDOWN()
{

    struct CommandLineInterface *cli = Cli();
    cli->cli_DefaultStack = stack_cliDefaultStack_Original;

    FreeSignal(sig);

    return CUE_SUCCESS;
}

CU_TEST_SETUP()
{
    clear_stack_values();
}

static void test_main_process()
{
    save_stack_values();
    CU_ASSERT_EQUAL(stack_tcSPUpper_tcSPLower,  stack_tcSPUpper_tcSPLower_Initial);
    CU_ASSERT_EQUAL(stack_prStackSize,          stack_prStackSize_Initial);
    CU_ASSERT_EQUAL(stack_cliDefaultStack,      stack_cliDefaultStack_Initial);
}

static void test_nostack_nocli()
{
    CreateNewProcTags(NP_Entry, (IPTR) SubProcess, NP_Output, Output(), NP_CloseOutput, FALSE, TAG_DONE);
    Wait(1<<sig);

    CU_ASSERT_EQUAL(stack_tcSPUpper_tcSPLower,  stack_cliDefaultStack_Initial);
    CU_ASSERT_EQUAL(stack_prStackSize,          stack_cliDefaultStack_Initial);
    CU_ASSERT_EQUAL(stack_cliDefaultStack,      -1);
    /* Note: AROS inherits stack from cli_DeafultStack, AmigaOS sets defaults stack size */
}

static void test_nostack_cli()
{
    CreateNewProcTags(NP_Entry, (IPTR) SubProcess, NP_Output, Output(), NP_CloseOutput, FALSE, NP_Cli, TRUE, TAG_DONE);
    Wait(1<<sig);

    CU_ASSERT_EQUAL(stack_tcSPUpper_tcSPLower,  stack_cliDefaultStack_Initial);
    CU_ASSERT_EQUAL(stack_prStackSize,          stack_cliDefaultStack_Initial);
    CU_ASSERT_EQUAL(stack_cliDefaultStack,      stack_cliDefaultStack_Initial);
    /* Note: AROS inherits stack from cli_DeafultStack, AmigaOS sets defaults stack size */
}

static void test_stack_nocli()
{
    CreateNewProcTags(NP_Entry, (IPTR) SubProcess, NP_Output, Output(), NP_CloseOutput, FALSE, NP_StackSize, 64000, TAG_DONE);
    Wait(1<<sig);

    CU_ASSERT_EQUAL(stack_tcSPUpper_tcSPLower,  64000);
    CU_ASSERT_EQUAL(stack_prStackSize,          64000);
    CU_ASSERT_EQUAL(stack_cliDefaultStack,      -1);
}

static void test_stack_cli()
{
    CreateNewProcTags(NP_Entry, (IPTR) SubProcess, NP_Output, Output(), NP_CloseOutput, FALSE, NP_StackSize, 96000, NP_Cli, TRUE, TAG_DONE);
    Wait(1<<sig);

    CU_ASSERT_EQUAL(stack_tcSPUpper_tcSPLower,  96000);
    CU_ASSERT_EQUAL(stack_prStackSize,          96000);
    CU_ASSERT_EQUAL(stack_cliDefaultStack,      96000);
}

static void test_combined_1()
{
    CreateNewProcTags(NP_Entry, (IPTR) SubProcess, NP_Output, Output(), NP_CloseOutput, FALSE, NP_StackSize, 96000, NP_Cli, TRUE, TAG_DONE);
    Wait(1<<sig);

    CU_ASSERT_EQUAL(stack_tcSPUpper_tcSPLower,  96000);
    CU_ASSERT_EQUAL(stack_prStackSize,          96000);
    CU_ASSERT_EQUAL(stack_cliDefaultStack,      96000);

    clear_stack_values();

    CreateNewProcTags(NP_Entry, (IPTR) SubProcess, NP_Output, Output(), NP_CloseOutput, FALSE, TAG_DONE);
    Wait(1<<sig);

    CU_ASSERT_EQUAL(stack_tcSPUpper_tcSPLower,  stack_cliDefaultStack_Initial);
    CU_ASSERT_EQUAL(stack_prStackSize,          stack_cliDefaultStack_Initial);
    CU_ASSERT_EQUAL(stack_cliDefaultStack,      -1);

    clear_stack_values();

    save_stack_values();
    CU_ASSERT_EQUAL(stack_tcSPUpper_tcSPLower,  stack_tcSPUpper_tcSPLower_Initial);
    CU_ASSERT_EQUAL(stack_prStackSize,          stack_prStackSize_Initial);
    CU_ASSERT_EQUAL(stack_cliDefaultStack,      stack_cliDefaultStack_Initial);
}


int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(CreateNewProc_Suite, __cu_suite_setup, __cu_suite_teardown, __cu_test_setup, NULL);
    CUNIT_CI_TEST(test_main_process);
    CUNIT_CI_TEST(test_nostack_nocli);
    CUNIT_CI_TEST(test_nostack_cli);
    CUNIT_CI_TEST(test_stack_nocli);
    CUNIT_CI_TEST(test_stack_cli);
    CUNIT_CI_TEST(test_combined_1);

    return CU_CI_RUN_SUITES();
}
