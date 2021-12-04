/*
    Copyright (C) 1995-2013, The AROS Development Team. All rights reserved.
*/

#include "../posixc/__crtext_intbase.h"

#include <aros/symbolsets.h>
#include <exec/lists.h>
#include "__exitfunc.h"

int __progonly_addexitfunc(struct AtExitNode *aen)
{
    struct CrtExtProgCtx *ProgCtx = __aros_get_ProgCtx();
    
    ADDHEAD((struct List *)&ProgCtx->atexit_list, (struct Node *)aen);

    return 0;
}

int __progonly_init_atexit(struct CrtExtProgCtx *ProgCtx)
{
    NEWLIST((struct List *)&ProgCtx->atexit_list);

    return 1;
}

void __progonly_callexitfuncs(void)
{
    struct CrtExtProgCtx *ProgCtx = __aros_get_ProgCtx();
    struct AtExitNode *aen;

    while (
        (aen = (struct AtExitNode *) REMHEAD((struct List *) &ProgCtx->atexit_list))
    )
    {
        switch (aen->node.ln_Type)
        {
        case AEN_VOID:
            aen->func.fvoid();
            break;

        case AEN_PTR:
            {
                int *errorptr = __progonly_get_errorptr();
                aen->func.fptr(errorptr != NULL ? *errorptr : 0, aen->ptr);
            }
            break;
        }
    }
}