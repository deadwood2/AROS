/*
    Copyright (C) 1995-2017, The AROS Development Team. All rights reserved.

    Desc: Search for a node by name.
*/
#include <string.h>
#include <aros/debug.h>

#include "../include/exec/structures.h"

struct NodeV0 * abiv0_FindName(struct ListV0 *list, CONST_STRPTR name, struct ExecBaseV0 *SysBaseV0)
{
    struct NodeV0 * node;
/* FIX !
        FindName supplied with a NULL list defaults to the exec port list
        Changed in lists.c as well....
*/
    if( !list )
    {
#if defined(__AROSEXEC_SMP__)
        bug("[EXEC] %s: called with NULL list!\n", __func__);
#endif
        list = &SysBaseV0->PortList;
    }

/*    ASSERT(list != NULL); */
    ASSERT(name);

    /* Look through the list */
    for (node=GetHeadV0(list); node; node=GetSuccV0(node))
    {
        /* Only compare the names if this node has one. */
        if(node->ln_Name)
        {
            /* Check the node. If we found it, stop. */
            if (!strcmp ((APTR)(IPTR)node->ln_Name, name))
                break;
        }
    }

    /*
        If we found a node, this will contain the pointer to it. If we
        didn't, this will be NULL (either because the list was
        empty or because we tried all nodes in the list)
    */
    return node;
} /* FindName */

