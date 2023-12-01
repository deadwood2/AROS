/*
    Copyright (C) 2023, The AROS Development Team. All rights reserved.
*/

#include <exec/libraries.h>
#include <proto/exec.h>
#include <aros/symbolsets.h>
#include <exec/lists.h>

static struct List allowed;
static struct SignalSemaphore sem;
struct AllowedNode
{
    struct Node     a_Node;
    struct Task     *a_Task;
    struct Library  *a_Base;
};

int InitLib(void)
{
    NEWLIST(&allowed);
    InitSemaphore(&sem);
    return 1;
}

ADD2INITLIB(InitLib, 0);

void Peropener_AllowShareable(struct Library *base)
{
    struct Task *me = FindTask(NULL);
    struct AllowedNode *an = AllocMem(sizeof(struct AllowedNode), MEMF_ANY | MEMF_CLEAR);
    an->a_Task = me;
    an->a_Base = base;
    ObtainSemaphore(&sem);
    AddHead(&allowed, (struct Node *)an);
    ReleaseSemaphore(&sem);
}

struct Library * Peropener_GetShareable(struct Library *base)
{
    struct Task *me = FindTask(NULL);
    struct AllowedNode *an;

    ObtainSemaphore(&sem);
    ForeachNode(&allowed, an)
    {
        if (an->a_Task == me)
            return an->a_Base;
    }
    ReleaseSemaphore(&sem);

    return base;
}

void Peropener_DisallowShareable(struct Library *base)
{
    struct Task *me = FindTask(NULL);
    struct AllowedNode *an, *an2;

    ObtainSemaphore(&sem);
    ForeachNodeSafe(&allowed, an, an2)
    {
        if (an->a_Task == me)
        {
            Remove((struct Node *)an);
            FreeMem(an, sizeof(struct AllowedNode));
        }
    }
    ReleaseSemaphore(&sem);

    return;
}
