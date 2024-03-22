#ifndef ABIV0_UTILITY_STRUCTURES_H
#define ABIV0_UTILITY_STRUCTURES_H

#include "../exec/structures.h"

struct TagItemV0
{
    ULONG       ti_Tag;
    ULONG       ti_Data;
} __attribute__((packed));

#endif
