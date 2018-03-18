/*
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc: libinit library - functions calling when opening/closing libs
*/

#include <exec/types.h>
#include <aros/symbolsets.h>

typedef int (*libfunc)(APTR libbase);
typedef int (*opendevfunc)
(
    void *libbase,
    void *ioreq,
    ULONG unitnum,
    ULONG flags
);
typedef int (*closedevfunc)
(
    void *libbase,
    void *ioreq
);

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

int set_call_libfuncs
(
    const void * const set[],
    int order,
    int test_fail,
    APTR libbase
)
{
    int pos, (*func)(APTR);
    struct Node *n;

    if (!set)
        return TRUE;

    int size = (IPTR)set[0];
    struct Node funcs[size];

    ForeachElementInSet(set, 1, pos, n)
    {
        funcs[pos - 1] = *n;
    }

    sort(funcs, size, order);

    for(pos = 0; pos < size; pos++)
    {
        func = (int (*)(APTR))(funcs[pos].ln_Name);
        if (test_fail)
        {
            if (!(*func)(libbase))
                return FALSE;
        }
        else
        {
            (void)(*func)(libbase);
        }
    }

    return TRUE;
}

int set_call_devfuncs
(
    const void * const set[],
    int order,
    int test_fail,
    void *libbase,
    void *ioreq,
    IPTR unitnum,
    ULONG flags
)
{
    int pos;

    if (!set)
        return TRUE;

    if (order>=0)
    {
        int (*func)(APTR, APTR, IPTR, ULONG);

        ForeachElementInSet(set, order, pos, func)
        {
            if (test_fail)
            {
                if (!(*func)(libbase, ioreq, unitnum, flags))
                    return FALSE;
            }
            else
            {
                (void)(*func)(libbase, ioreq, unitnum, flags);
            }
        }
    }
    else
    {
        int (*func)(APTR, APTR);

        ForeachElementInSet(set, order, pos, func)
        {
            if (test_fail)
            {
                if (!(*func)(libbase, ioreq))
                    return FALSE;
            }
            else
            {
                (void)(*func)(libbase, ioreq);
            }
        }
    }

    return TRUE;
}
