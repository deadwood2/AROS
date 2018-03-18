/*
    Copyright © 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

    Desc: autoinit library - functions sets handling
    Lang: english
*/

#define DEBUG 0

#include <aros/symbolsets.h>
#include <aros/debug.h>

static void sort
(
    struct Node funcs[],
    int size,
    int order
)
{
    int pos;
    int again = 1;

    if (size == 1) return;

    while (again)
    {
        again = 0;
        for (pos = 0; pos < size - 1; pos++)
        {
            if (((funcs[pos].ln_Pri > funcs[pos + 1].ln_Pri) && order > 0) ||
                ((funcs[pos].ln_Pri < funcs[pos + 1].ln_Pri) && order < 0))
                { struct Node t = funcs[pos]; funcs[pos] = funcs[pos + 1]; funcs[pos + 1] = t; again = 1; break; }
        }
    }
}

int _set_call_funcs(const void * const set[], int direction, int test_fail, struct ExecBase *SysBase)
{
    int pos, (*func)(struct ExecBase *SysBase);
    struct Node *n;

    if (!set)
        return 1;

    int size = (IPTR)set[0];
    struct Node funcs[size];

    ForeachElementInSet(set, 1, pos, n)
    {
        funcs[pos - 1] = *n;
    }

    sort(funcs, size, direction);

    for(pos = 0; pos < size; pos++)
    {
        func = (int (*)(struct ExecBase *SysBase))(funcs[pos].ln_Name);
        if (test_fail)
        {
            int ret = (*func)(SysBase);
            if (!ret)
                return 0;
        }
        else
        {
            (void)(*func)(SysBase);
        }
    }

    return 1;
}
