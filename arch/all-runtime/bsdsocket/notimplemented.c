#include <aros/debug.h>

#include <proto/socket.h>

#define NOT_IMPL (bug("<<WARN>>: %s is not implemented. Please submit issuet at https://github.com/deadw00d/AROS/issues.\n", __FUNCTION__));

AROS_LH3(void, SetSocketSignals,
         AROS_LHA(ULONG, intrmask, D0),
         AROS_LHA(ULONG, iomask, D1),
         AROS_LHA(ULONG, urgmask, D2),
         struct Library *, SocketBase, 22, BSDSocket)
{
    AROS_LIBFUNC_INIT

    NOT_IMPL

    AROS_LIBFUNC_EXIT
}

AROS_LH2(LONG, ReleaseSocket,
         AROS_LHA(LONG, sd, D0),
         AROS_LHA(LONG, id, D1),
         struct Library *, SocketBase, 25, BSDSocket)
{
    AROS_LIBFUNC_INIT

    NOT_IMPL
    return 0;

    AROS_LIBFUNC_EXIT
}

AROS_LH0(LONG, Errno,
         struct Library *, SocketBase, 27, BSDSocket)
{
    AROS_LIBFUNC_INIT

    NOT_IMPL
    return 0;

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, SetErrnoPtr,
         AROS_LHA(void *, ptr, A0),
         AROS_LHA(int, size, D0),
         struct Library *, SocketBase, 28, BSDSocket)
{
    AROS_LIBFUNC_INIT

    NOT_IMPL

    AROS_LIBFUNC_EXIT
}

AROS_LH1(char *, Inet_NtoA,
         AROS_LHA(unsigned long, in, D0),
         struct Library *, SocketBase, 29, BSDSocket)
{
    AROS_LIBFUNC_INIT

    NOT_IMPL
    return NULL;

    AROS_LIBFUNC_EXIT
}

AROS_LH1(unsigned long, Inet_LnaOf,
         AROS_LHA(unsigned long, in, D0),
         struct Library *, SocketBase, 31, BSDSocket)
{
    AROS_LIBFUNC_INIT

    NOT_IMPL
    return 0;

    AROS_LIBFUNC_EXIT
}

AROS_LH1(unsigned long, Inet_NetOf,
         AROS_LHA(unsigned long, in, D0),
         struct Library *, SocketBase, 32, BSDSocket)
{
    AROS_LIBFUNC_INIT

    NOT_IMPL
    return 0;

    AROS_LIBFUNC_EXIT
}

AROS_LH2(unsigned long, Inet_MakeAddr,
         AROS_LHA(int, net, D0),
         AROS_LHA(int, lna, D1),
         struct Library *, SocketBase, 33, BSDSocket)
{
    AROS_LIBFUNC_INIT

    NOT_IMPL
    return 0;

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, Dup2Socket,
         AROS_LHA(int, fd1, D0),
         AROS_LHA(int, fd2, D1),
         struct Library *, SocketBase, 44, BSDSocket)
{
    AROS_LIBFUNC_INIT

    NOT_IMPL
    return 0;

    AROS_LIBFUNC_EXIT
}

AROS_LH1(LONG, GetSocketEvents,
         AROS_LHA(ULONG *, eventsp, A0),
         struct Library *, SocketBase, 50, BSDSocket)
{
    AROS_LIBFUNC_INIT

    NOT_IMPL
    return 0;

    AROS_LIBFUNC_EXIT
}

