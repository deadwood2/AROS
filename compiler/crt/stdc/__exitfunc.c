/*
    Copyright (C) 1995-2013, The AROS Development Team. All rights reserved.
*/


#include <aros/symbolsets.h>
#include <exec/lists.h>
#include "__exitfunc.h"
#include "../crt/__crt_intbase.h"
#include "../crt/__crt_progonly.h"

int __progonly_addexitfunc(struct AtExitNode *aen)
{
    struct CrtProgCtx *ProgCtx = __aros_get_ProgCtx();

    if (ProgCtx == NULL) /* In C++ atexit() can be called from CTORS set, before CrtProgCtx is created */
    {
        struct CrtIntBase *CrtBase = (struct CrtIntBase *)__aros_getbase_CrtBase();
        ADDHEAD((struct List *)&CrtBase->early_atexit_list, (struct Node *)aen);
        return 0;
    }

    ADDHEAD((struct List *)&ProgCtx->atexit_list, (struct Node *)aen);

    return 0;
}

int __progonly_init_atexit(struct CrtProgCtx *ProgCtx)
{
    NEWLIST((struct List *)&ProgCtx->atexit_list);

    struct CrtIntBase *CrtBase = (struct CrtIntBase *)__aros_getbase_CrtBase();
    if (CrtBase) /* CrtBase is NULL during vfork launcher */
    {
        struct AtExitNode *aen;

        /* Move atexit nodes from "early" list to main list */
        while ((aen = (struct AtExitNode *) REMHEAD((struct List *) &CrtBase->early_atexit_list)))
        {
            ADDHEAD((struct List *)&ProgCtx->atexit_list, (struct Node *)aen);
        }
    }

    return 1;
}

void __progonly_callexitfuncs(void)
{
    struct CrtProgCtx *ProgCtx = __aros_get_ProgCtx();
    struct AtExitNode *aen;

    while (
        (aen = (struct AtExitNode *) REMHEAD((struct List *) &ProgCtx->atexit_list))
    )
    {
        switch (aen->node.ln_Type)
        {
        case AEN_VOID:
            aen->func.fn();
            break;

        case AEN_ON:
            {
                int *errorptr = __progonly_get_errorptr();
                aen->func.on.fn(errorptr != NULL ? *errorptr : 0, aen->func.on.arg);
            }
            break;

        case AEN_CXA:
            {
                int *errorptr = __progonly_get_errorptr();
                aen->func.cxa.fn(aen->func.cxa.arg, errorptr != NULL ? *errorptr : 0);
            }
            break;

        }
    }
}

int __init_early_atexit(struct CrtIntBase *CrtBase)
{
    NEWLIST((struct List *)&CrtBase->early_atexit_list);

    return 1;
}

ADD2OPENLIB(__init_early_atexit, 100);
