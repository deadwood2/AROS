#ifndef ABIV0_EXEC_PROXY_STRUCTURES_H
#define ABIV0_EXEC_PROXY_STRUCTURES_H

#include "./structures.h"

struct MsgPortProxy
{
    struct MsgPortV0 base;
    struct MsgPort   *native;
    struct MessageV0 *(*translate)(struct Message *);
};

#define DEVPROXY_TYPE_INPUT (1)
#define DEVPROXY_TYPE_TIMER (2)
struct DeviceProxy
{
    struct LibraryV0    base;
    struct IOStdReq     *io;
    struct MsgPort      *mp;

    ULONG               type;
    APTR                user1;
    APTR                user2;
};

#endif
