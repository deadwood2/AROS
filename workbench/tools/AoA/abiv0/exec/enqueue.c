/*
    Copyright (C) 1995-2010, The AROS Development Team. All rights reserved.

    Desc: Add a node into a sorted list
*/
#include <aros/debug.h>
#include "../include/exec/functions.h"

/*****************************************************************************

    NAME */

void abiv0_Enqueue(struct ListV0 *list, struct NodeV0 *node, struct ExecBaseV0 *SysBaseV0)
{
    struct NodeV0 * next;

    ASSERT(list != NULL);
    ASSERT(node != NULL);

    /* Look through the list */
    ForeachNodeV0 (list, next)
    {
        /*
            Look for the first node with a lower pri as the node
            we have to insert into the list.
        */
        if (node->ln_Pri > next->ln_Pri)
            break;
    }

    /* Insert the node before(!) next. The situation looks like this:

            A<->next<->B *<-node->*

        ie. next->ln_Pred points to A, A->ln_Succ points to next,
        next->ln_Succ points to B, B->ln_Pred points to next.
        ln_Succ and ln_Pred of node contain illegal pointers.
    */

    /* Let node point to A: A<-node */
    node->ln_Pred          = next->ln_Pred;

    /* Make node point to next: A<->node->next<->B */
    node->ln_Succ          = (APTR32)(IPTR)next;

    /* Let A point to node: A->node */
    ((struct NodeV0 *)(IPTR)next->ln_Pred)->ln_Succ = (APTR32)(IPTR)node;

    /* Make next point to node: A<->node<-next<->B */
    next->ln_Pred          = (APTR32)(IPTR)node;
} /* Enqueue */
