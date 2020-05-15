#ifndef DEFINES_BSDSOCKET_PROTOS_H
#define DEFINES_BSDSOCKET_PROTOS_H

/*
    Copyright © 1995-2017, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for bsdsocket
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

#define __IoctlSocket_WB(__SocketBase, __arg1, __arg2, __arg3) \
        AROS_LC3(int, IoctlSocket, \
                  AROS_LCA(int,(__arg1),D0), \
                  AROS_LCA(unsigned long,(__arg2),D1), \
                  AROS_LCA(char *,(__arg3),A0), \
        struct Library *, (__SocketBase), 19, BSDSocket)

#define IoctlSocket(arg1, arg2, arg3) \
    __IoctlSocket_WB(SocketBase, (arg1), (arg2), (arg3))

#define __CloseSocket_WB(__SocketBase, __arg1) \
        AROS_LC1(int, CloseSocket, \
                  AROS_LCA(int,(__arg1),D0), \
        struct Library *, (__SocketBase), 20, BSDSocket)

#ifndef PTHREAD_H
#define CloseSocket(arg1) \
    __CloseSocket_WB(SocketBase, (arg1))
#endif

#define __WaitSelect_WB(__SocketBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6(int, WaitSelect, \
                  AROS_LCA(int,(__arg1),D0), \
                  AROS_LCA(fd_set *,(__arg2),A0), \
                  AROS_LCA(fd_set *,(__arg3),A1), \
                  AROS_LCA(fd_set *,(__arg4),A2), \
                  AROS_LCA(struct timeval *,(__arg5),A3), \
                  AROS_LCA(ULONG *,(__arg6),D1), \
        struct Library *, (__SocketBase), 21, BSDSocket)

#ifndef PTHREAD_H
#define WaitSelect(arg1, arg2, arg3, arg4, arg5, arg6) \
    __WaitSelect_WB(SocketBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))
#endif

#define __SetSocketSignals_WB(__SocketBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, SetSocketSignals, \
                  AROS_LCA(ULONG,(__arg1),D0), \
                  AROS_LCA(ULONG,(__arg2),D1), \
                  AROS_LCA(ULONG,(__arg3),D2), \
        struct Library *, (__SocketBase), 22, BSDSocket)

#define SetSocketSignals(arg1, arg2, arg3) \
    __SetSocketSignals_WB(SocketBase, (arg1), (arg2), (arg3))

#define __ObtainSocket_WB(__SocketBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(LONG, ObtainSocket, \
                  AROS_LCA(LONG,(__arg1),D0), \
                  AROS_LCA(LONG,(__arg2),D1), \
                  AROS_LCA(LONG,(__arg3),D2), \
                  AROS_LCA(LONG,(__arg4),D3), \
        struct Library *, (__SocketBase), 24, BSDSocket)

#define ObtainSocket(arg1, arg2, arg3, arg4) \
    __ObtainSocket_WB(SocketBase, (arg1), (arg2), (arg3), (arg4))

#define __ReleaseSocket_WB(__SocketBase, __arg1, __arg2) \
        AROS_LC2(LONG, ReleaseSocket, \
                  AROS_LCA(LONG,(__arg1),D0), \
                  AROS_LCA(LONG,(__arg2),D1), \
        struct Library *, (__SocketBase), 25, BSDSocket)

#define ReleaseSocket(arg1, arg2) \
    __ReleaseSocket_WB(SocketBase, (arg1), (arg2))

#define __ReleaseCopyOfSocket_WB(__SocketBase, __arg1, __arg2) \
        AROS_LC2(LONG, ReleaseCopyOfSocket, \
                  AROS_LCA(LONG,(__arg1),D0), \
                  AROS_LCA(LONG,(__arg2),D1), \
        struct Library *, (__SocketBase), 26, BSDSocket)

#define ReleaseCopyOfSocket(arg1, arg2) \
    __ReleaseCopyOfSocket_WB(SocketBase, (arg1), (arg2))

#define __Errno_WB(__SocketBase) \
        AROS_LC0(LONG, Errno, \
        struct Library *, (__SocketBase), 27, BSDSocket)

#define Errno() \
    __Errno_WB(SocketBase)

#define __SetErrnoPtr_WB(__SocketBase, __arg1, __arg2) \
        AROS_LC2NR(void, SetErrnoPtr, \
                  AROS_LCA(void *,(__arg1),A0), \
                  AROS_LCA(int,(__arg2),D0), \
        struct Library *, (__SocketBase), 28, BSDSocket)

#define SetErrnoPtr(arg1, arg2) \
    __SetErrnoPtr_WB(SocketBase, (arg1), (arg2))

#define __Inet_NtoA_WB(__SocketBase, __arg1) \
        AROS_LC1(char *, Inet_NtoA, \
                  AROS_LCA(unsigned long,(__arg1),D0), \
        struct Library *, (__SocketBase), 29, BSDSocket)

#define Inet_NtoA(arg1) \
    __Inet_NtoA_WB(SocketBase, (arg1))

#define __Inet_LnaOf_WB(__SocketBase, __arg1) \
        AROS_LC1(unsigned long, Inet_LnaOf, \
                  AROS_LCA(unsigned long,(__arg1),D0), \
        struct Library *, (__SocketBase), 31, BSDSocket)

#define Inet_LnaOf(arg1) \
    __Inet_LnaOf_WB(SocketBase, (arg1))

#define __Inet_NetOf_WB(__SocketBase, __arg1) \
        AROS_LC1(unsigned long, Inet_NetOf, \
                  AROS_LCA(unsigned long,(__arg1),D0), \
        struct Library *, (__SocketBase), 32, BSDSocket)

#define Inet_NetOf(arg1) \
    __Inet_NetOf_WB(SocketBase, (arg1))

#define __Inet_MakeAddr_WB(__SocketBase, __arg1, __arg2) \
        AROS_LC2(unsigned long, Inet_MakeAddr, \
                  AROS_LCA(int,(__arg1),D0), \
                  AROS_LCA(int,(__arg2),D1), \
        struct Library *, (__SocketBase), 33, BSDSocket)

#define Inet_MakeAddr(arg1, arg2) \
    __Inet_MakeAddr_WB(SocketBase, (arg1), (arg2))

#define __Dup2Socket_WB(__SocketBase, __arg1, __arg2) \
        AROS_LC2(int, Dup2Socket, \
                  AROS_LCA(int,(__arg1),D0), \
                  AROS_LCA(int,(__arg2),D1), \
        struct Library *, (__SocketBase), 44, BSDSocket)

#define Dup2Socket(arg1, arg2) \
    __Dup2Socket_WB(SocketBase, (arg1), (arg2))

#define __SocketBaseTagList_WB(__SocketBase, __arg1) \
        AROS_LC1(ULONG, SocketBaseTagList, \
                  AROS_LCA(struct TagItem *,(__arg1),A0), \
        struct Library *, (__SocketBase), 49, BSDSocket)

#define SocketBaseTagList(arg1) \
    __SocketBaseTagList_WB(SocketBase, (arg1))

#if !defined(NO_INLINE_STDARG) && !defined(BSDSOCKET_NO_INLINE_STDARG)
#define SocketBaseTags(...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    SocketBaseTagList((struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __GetSocketEvents_WB(__SocketBase, __arg1) \
        AROS_LC1(LONG, GetSocketEvents, \
                  AROS_LCA(ULONG *,(__arg1),A0), \
        struct Library *, (__SocketBase), 50, BSDSocket)

#define GetSocketEvents(arg1) \
    __GetSocketEvents_WB(SocketBase, (arg1))

#ifdef PTHREAD_H
#include <defines/pthreadsocket.h>
#endif

#endif /* DEFINES_BSDSOCKET_PROTOS_H */
