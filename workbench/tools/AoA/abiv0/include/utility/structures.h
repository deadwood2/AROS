#ifndef ABIV0_UTILITY_STRUCTURES_H
#define ABIV0_UTILITY_STRUCTURES_H

#include "../exec/structures.h"

struct TagItemV0
{
    ULONG       ti_Tag;
    ULONG       ti_Data;
} __attribute__((packed));

/* A callback Hook */
struct HookV0
{
    struct MinNodeV0  h_MinNode;
    APTR32    h_Entry;        /* Main entry point */
    APTR32    h_SubEntry;     /* Secondary entry point */
    APTR32    h_Data;	    /* Whatever you want */
};

#endif
