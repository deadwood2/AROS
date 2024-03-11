#ifndef ABIV0_TIMER_STRUCTURES_H
#define ABIV0_TIMER_STRUCTURES_H

#include <aros/types/timeval_s.h> /* get struct timeval */
#include "../exec/structures.h"

/* Request used with 32-bit commands */
struct timerequestV0
{
    struct IORequestV0 tr_node;
    struct timeval32 tr_time;
};

#endif
