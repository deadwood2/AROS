/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$

    Desc: autoinit library - functions for handling the PROGRAM_ENTRIES symbolset
*/
#include <aros/symbolsets.h>

DEFINESET(PROGRAM_ENTRIES);

static int __startup_entry_pos;

static struct Node funcs[32];

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

void __startup_entries_init(void)
{
    int pos;
    struct Node *n;
    __startup_entry_pos = 1;

    int size = (IPTR)SETNAME(PROGRAM_ENTRIES)[0];
    ForeachElementInSet(SETNAME(PROGRAM_ENTRIES), 1, pos, n)
    {
        funcs[pos - 1] = *n;
    }

    sort(funcs, size, 1);
}

void ___startup_entries_next(struct ExecBase *SysBase)
{
    void (*entry_func)(struct ExecBase *SysBase);
 
    entry_func = (void (*)(struct ExecBase *SysBase))(funcs[__startup_entry_pos].ln_Name);
    if (entry_func)
    {
        __startup_entry_pos++;
        entry_func(SysBase);
    }
}
