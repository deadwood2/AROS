#ifndef ABIV0_EXEC_PROXY_STRUCTURES_H
#define ABIV0_EXEC_PROXY_STRUCTURES_H

#include "./structures.h"

struct MsgPortProxy
{
    struct MsgPortV0 base;
    struct MsgPort   *native;
    struct MessageV0 *(*translate)(struct Message *);
};

#define DEVPROXY_TYPE_INPUT     (1)
#define DEVPROXY_TYPE_TIMER     (2)
#define DEVPROXY_TYPE_AHI       (3)
#define DEVPROXY_TYPE_CONSOLE   (4)

struct DeviceProxy
{
    struct DeviceV0     base;
    struct Device       *native;
    ULONG               type;
    struct IOStdReq     *io;
    struct Unit*        nativeunit;
};

#endif
