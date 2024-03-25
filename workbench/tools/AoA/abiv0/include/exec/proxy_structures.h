#ifndef ABIV0_EXEC_PROXY_STRUCTURES_H
#define ABIV0_EXEC_PROXY_STRUCTURES_H

#include "./structures.h"

struct MsgPortProxy
{
    struct MsgPortV0 base;
    struct MsgPort   *native;
    struct MessageV0 *(*translate)(struct Message *);
};

#endif
