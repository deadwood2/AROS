#ifndef ABIV0_INPUT_STRUCTURES_H
#define ABIV0_INPUT_STRUCTURES_H

#include "../exec/structures.h"

struct InputEventV0
{
    APTR32 ie_NextEvent;

    UBYTE ie_Class;     /* see below for definitions */
    UBYTE ie_SubClass;  /* see below for definitions */
    UWORD ie_Code;      /* see below for definitions */
    UWORD ie_Qualifier; /* see below for definitions */

    union
    {
        struct
        {
            WORD ie_x;
            WORD ie_y;
        } ie_xy;

        APTR32 ie_addr;

        struct
        {
            UBYTE ie_prev1DownCode;
            UBYTE ie_prev1DownQual;
            UBYTE ie_prev2DownCode;
            UBYTE ie_prev2DownQual;
        } ie_dead;
    } ie_position;

    /* This is guaranteed to be increasing with time, but not guaranteed
       to contain absolute time */
    struct timeval32    ie_TimeStamp;
};

#endif
