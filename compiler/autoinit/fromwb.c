/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$

    Desc: autoinit library - handle program started from workbench.
*/
#include <aros/symbolsets.h>
#include <aros/startup.h>
#include <dos/dosextens.h>
#include <proto/autoinit.h>

struct WBStartup *WBenchMsg;

int __nowbsupport __attribute__((weak)) = 0;

static void __startup_fromwb(struct ExecBase *SysBase)
{
    BPTR curdir = BNULL;

    FromWBInit2(WBenchMsg, &curdir, (STRPTR **)&__argv, &__argc);

    __startup_entries_next();

    FromWBExit(WBenchMsg, curdir);
}

ADD2SET(__startup_fromwb, PROGRAM_ENTRIES, -50);

/*
 * This should be executed as one of the first actions. The message
 * that is present on myproc->pr_MsgPort will break DOS functions
 * if not fetched. (example: autopened library using DOS in its
 * OpenLib function).
 */
static void __startup_fromwb_prelibopen(struct ExecBase *SysBase)
{
    struct Process *myproc = (struct Process *)FindTask(NULL);

     /* Do we have a CLI structure? */
     if (!myproc->pr_CLI)
     {
         /* Workbench startup. Get WBenchMsg and pass it to main() */

         WaitPort(&myproc->pr_MsgPort);
         WBenchMsg = (struct WBStartup *)GetMsg(&myproc->pr_MsgPort);
     }

    __startup_entries_next();
}

ADD2SET(__startup_fromwb_prelibopen, PROGRAM_ENTRIES, -70);
