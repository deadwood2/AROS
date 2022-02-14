/*
    Copyright (C) 2020, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dos.h>
#include <dos/dostags.h>

#ifndef __AROS__
#define IPTR ULONG
#define CLI_DEFAULTSTACK_UNIT sizeof(IPTR)
typedef struct RAWARG_s { } *RAWARG;
#endif

/*

This is output of the test from AmigaOS 3.1 Shell
:> stack 50000
:> clicreatenewproc
Main, tc_SPUpper/SPLower:  50000, pr_StackSize:   3200, cli_DefaultStack:  50000

Creating subprocess, no stack set, no cli
Sub , tc_SPUpper/SPLower:   4000, pr_StackSize:   4000, cli_DefaultStack:     -1

Creating subprocess, no stack set, cli
Sub , tc_SPUpper/SPLower:   4000, pr_StackSize:   4000, cli_DefaultStack:   4000

------------------------------------------------------
Creating subprocess, stack set to 64000, no cli
Sub , tc_SPUpper/SPLower:  64000, pr_StackSize:  64000, cli_DefaultStack:     -1

Main, tc_SPUpper/SPLower:  50000, pr_StackSize:   3200, cli_DefaultStack:  50000

Creating subprocess, no stack set, no cli
Sub , tc_SPUpper/SPLower:   4000, pr_StackSize:   4000, cli_DefaultStack:     -1

Creating subprocess, no stack set, cli
Sub , tc_SPUpper/SPLower:   4000, pr_StackSize:   4000, cli_DefaultStack:   4000

------------------------------------------------------
Creating subprocess, stack set to 96000, cli
Sub , tc_SPUpper/SPLower:  96000, pr_StackSize:  96000, cli_DefaultStack:  96000

Main, tc_SPUpper/SPLower:  50000, pr_StackSize:   3200, cli_DefaultStack:  50000

Creating subprocess, no stack set, no cli
Sub , tc_SPUpper/SPLower:   4000, pr_StackSize:   4000, cli_DefaultStack:     -1

Creating subprocess, no stack set, cli
Sub , tc_SPUpper/SPLower:   4000, pr_StackSize:   4000, cli_DefaultStack:   4000

------------------------------------------------------

This is output of the test from AROS
:> stack 50000
:> clicreatenewproc
Main, tc_SPUpper/SPLower:  50000, pr_StackSize: 262144, cli_DefaultStack:  50000

Creating subprocess, no stack set, no cli
Sub , tc_SPUpper/SPLower:  50000, pr_StackSize:  50000, cli_DefaultStack:     -1

Creating subprocess, no stack set, cli
Sub , tc_SPUpper/SPLower:  50000, pr_StackSize:  50000, cli_DefaultStack:  50000

------------------------------------------------------
Creating subprocess, stack set to 64000, no cli
Sub , tc_SPUpper/SPLower:  64000, pr_StackSize:  64000, cli_DefaultStack:     -1

Main, tc_SPUpper/SPLower:  50000, pr_StackSize: 262144, cli_DefaultStack:  50000

Creating subprocess, no stack set, no cli
Sub , tc_SPUpper/SPLower:  50000, pr_StackSize:  50000, cli_DefaultStack:     -1

Creating subprocess, no stack set, cli
Sub , tc_SPUpper/SPLower:  50000, pr_StackSize:  50000, cli_DefaultStack:  50000

------------------------------------------------------
Creating subprocess, stack set to 96000, cli
Sub , tc_SPUpper/SPLower:  96000, pr_StackSize:  96000, cli_DefaultStack:  96000

Main, tc_SPUpper/SPLower:  50000, pr_StackSize: 262144, cli_DefaultStack:  50000

Creating subprocess, no stack set, no cli
Sub , tc_SPUpper/SPLower:  50000, pr_StackSize:  50000, cli_DefaultStack:     -1

Creating subprocess, no stack set, cli
Sub , tc_SPUpper/SPLower:  50000, pr_StackSize:  50000, cli_DefaultStack:  50000

------------------------------------------------------

On AROS, unless NP_StackStack size is provided, child processes inherit stack
from parent process cli_DefaultStack if available.

*/

static BYTE sig;
static struct Task *task;

static void __printf_info(struct Process *p, struct CommandLineInterface *cli)
{
    LONG argl[3];
    argl[0] = (LONG)(((UBYTE *)p->pr_Task.tc_SPUpper) - ((UBYTE *)p->pr_Task.tc_SPLower));
    argl[1] = p->pr_StackSize;
    argl[2] = cli ? cli->cli_DefaultStack * CLI_DEFAULTSTACK_UNIT: -1;

    VPrintf(", tc_SPUpper/SPLower: %6ld, pr_StackSize: %6ld, cli_DefaultStack: %6ld\n\n", (RAWARG)argl);
}
static void SubProcess(void)
{
    struct CommandLineInterface *cli;
    struct Process *me;

    me = (struct Process *)FindTask(NULL);
    cli = Cli();
    VPrintf("Sub ", NULL);__printf_info(me, cli);

    Signal(task, 1<<sig);
}

static void NoStack_NoCli_Cli()
{

    VPrintf("Creating subprocess, no stack set, no cli\n", NULL);
    CreateNewProcTags(NP_Entry, (IPTR) SubProcess, NP_Output, Output(), NP_CloseOutput, FALSE, TAG_DONE);
    Wait(1<<sig);

    VPrintf("Creating subprocess, no stack set, cli\n", NULL);
    CreateNewProcTags(NP_Entry, (IPTR) SubProcess, NP_Output, Output(), NP_CloseOutput, FALSE,
        NP_Cli, TRUE, TAG_DONE);
    Wait(1<<sig);

    VPrintf("------------------------------------------------------\n", NULL);
}

int main(void)
{
    struct CommandLineInterface *cli;
    struct Process *me;

    sig = AllocSignal(-1);
    task = FindTask(NULL);

    me = (struct Process *)task;
    cli = Cli();

    VPrintf("Main", NULL);__printf_info(me, cli);

    NoStack_NoCli_Cli();

    VPrintf("Creating subprocess, stack set to 64000, no cli\n", NULL);
    CreateNewProcTags(NP_Entry, (IPTR) SubProcess, NP_Output, Output(), NP_CloseOutput, FALSE,
        NP_StackSize, 64000, TAG_DONE);
    Wait(1<<sig);

    VPrintf("Main", NULL);__printf_info(me, cli);

    NoStack_NoCli_Cli();

    VPrintf("Creating subprocess, stack set to 96000, cli\n", NULL);
    CreateNewProcTags(NP_Entry, (IPTR) SubProcess, NP_Output, Output(), NP_CloseOutput, FALSE,
        NP_StackSize, 96000, NP_Cli, TRUE, TAG_DONE);
    Wait(1<<sig);

    VPrintf("Main", NULL);__printf_info(me, cli);

    NoStack_NoCli_Cli();

    FreeSignal(sig);

    return 0;
}
