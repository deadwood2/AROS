#ifndef CLIB_BSDSOCKET_PROTOS_H
#define CLIB_BSDSOCKET_PROTOS_H

/*
    Copyright © 1995-2016, The AROS Development Team. All rights reserved.
*/

#include <utility/tagitem.h>
#include <aros/libcall.h>
#include <sys/types.h>
#include <sys/select.h>

AROS_LP3(int, IoctlSocket,
         AROS_LPA(int, s, D0),
         AROS_LPA(unsigned long, request, D1),
         AROS_LPA(char *, argp, A0),
         LIBBASETYPEPTR, SocketBase, 19, BSDSocket
);
AROS_LP1(int, CloseSocket,
         AROS_LPA(int, s, D0),
         LIBBASETYPEPTR, SocketBase, 20, BSDSocket
);
AROS_LP6(int, WaitSelect,
         AROS_LPA(int, nfds, D0),
         AROS_LPA(fd_set *, readfds, A0),
         AROS_LPA(fd_set *, writefds, A1),
         AROS_LPA(fd_set *, exceptfds, A2),
         AROS_LPA(struct timeval *, timeout, A3),
         AROS_LPA(ULONG *, sigmask, D1),
         LIBBASETYPEPTR, SocketBase, 21, BSDSocket
);
AROS_LP3(void, SetSocketSignals,
         AROS_LPA(ULONG, intrmask, D0),
         AROS_LPA(ULONG, iomask, D1),
         AROS_LPA(ULONG, urgmask, D2),
         LIBBASETYPEPTR, SocketBase, 22, BSDSocket
);
AROS_LP4(LONG, ObtainSocket,
         AROS_LPA(LONG, id, D0),
         AROS_LPA(LONG, domain, D1),
         AROS_LPA(LONG, type, D2),
         AROS_LPA(LONG, protocol, D3),
         LIBBASETYPEPTR, SocketBase, 24, BSDSocket
);
AROS_LP2(LONG, ReleaseSocket,
         AROS_LPA(LONG, sd, D0),
         AROS_LPA(LONG, id, D1),
         LIBBASETYPEPTR, SocketBase, 25, BSDSocket
);
AROS_LP2(LONG, ReleaseCopyOfSocket,
         AROS_LPA(LONG, sd, D0),
         AROS_LPA(LONG, id, D1),
         LIBBASETYPEPTR, SocketBase, 26, BSDSocket
);
AROS_LP0(LONG, Errno,
         LIBBASETYPEPTR, SocketBase, 27, BSDSocket
);
AROS_LP2(void, SetErrnoPtr,
         AROS_LPA(void *, ptr, A0),
         AROS_LPA(int, size, D0),
         LIBBASETYPEPTR, SocketBase, 28, BSDSocket
);
AROS_LP1(char *, Inet_NtoA,
         AROS_LPA(unsigned long, in, D0),
         LIBBASETYPEPTR, SocketBase, 29, BSDSocket
);
AROS_LP1(unsigned long, Inet_LnaOf,
         AROS_LPA(unsigned long, in, D0),
         LIBBASETYPEPTR, SocketBase, 31, BSDSocket
);
AROS_LP1(unsigned long, Inet_NetOf,
         AROS_LPA(unsigned long, in, D0),
         LIBBASETYPEPTR, SocketBase, 32, BSDSocket
);
AROS_LP2(unsigned long, Inet_MakeAddr,
         AROS_LPA(int, net, D0),
         AROS_LPA(int, lna, D1),
         LIBBASETYPEPTR, SocketBase, 33, BSDSocket
);
AROS_LP2(int, Dup2Socket,
         AROS_LPA(int, fd1, D0),
         AROS_LPA(int, fd2, D1),
         LIBBASETYPEPTR, SocketBase, 44, BSDSocket
);
AROS_LP1(ULONG, SocketBaseTagList,
         AROS_LPA(struct TagItem *, tagList, A0),
         LIBBASETYPEPTR, SocketBase, 49, BSDSocket
);
AROS_LP1(LONG, GetSocketEvents,
         AROS_LPA(ULONG *, eventsp, A0),
         LIBBASETYPEPTR, SocketBase, 50, BSDSocket
);
#endif /* CLIB_BSDSOCKET_PROTOS_H */
