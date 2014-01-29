/*
    Copyright © 1995-2012, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "__arosc_privdata.h"

#include <aros/symbolsets.h>
#include <exec/lists.h>
#include <proto/exec.h>
#include <string.h>
#include "__exitfunc.h"

static void __exitfuncsquirk(struct aroscbase *aroscbase)
{
    if (strcmp(FindTask(NULL)->tc_Node.ln_Name, "zsnes") == 0)
    {
        /* Run application atexit (which uses TimerIO) before SDL atexit (which frees TimerIO) */
        struct Node * tmp = REMHEAD((struct List *) &aroscbase->acb_atexit_list);
        ADDTAIL((struct List *)&aroscbase->acb_atexit_list, tmp);
    }
}
int __addexitfunc(struct AtExitNode *aen)
{
    struct aroscbase *aroscbase = __aros_getbase_aroscbase();
    
    ADDHEAD((struct List *)&aroscbase->acb_atexit_list, (struct Node *)aen);

    return 0;
}

int __init_atexit(struct aroscbase *aroscbase)
{
    NEWLIST((struct List *)&aroscbase->acb_atexit_list);

    return 1;
}

void __callexitfuncs(void)
{
    struct aroscbase *aroscbase = __aros_getbase_aroscbase();
    struct AtExitNode *aen;

    __exitfuncsquirk(aroscbase);

    while (
        (aen = (struct AtExitNode *) REMHEAD((struct List *) &aroscbase->acb_atexit_list))
    )
    {
        switch (aen->node.ln_Type)
        {
        case AEN_VOID:
            aen->func.fvoid();
            break;

        case AEN_PTR:
            {
                int *errorptr = __arosc_get_errorptr();
                aen->func.fptr(errorptr != NULL ? *errorptr : 0, aen->ptr);
            }
            break;
        }
    }
}

ADD2OPENLIB(__init_atexit, 100);

/* ABI_V0 compatibility */
/*
 * Older ABI_V0 binaries did not call __arosc_program_end and relied on closing of arosc.library
 * to run atexit handlers. Example: ltris
 */
void __exit_atexit(void)
{
    struct aroscbase *aroscbase = __aros_getbase_aroscbase();

    if (!(aroscbase->acb_flags & (VFORK_PARENT | ACPD_NEWSTARTUP)))
    {
        if (!(aroscbase->acb_flags & ABNORMAL_EXIT))
            __callexitfuncs();
    }
}

ADD2CLOSELIB(__exit_atexit, 100);
