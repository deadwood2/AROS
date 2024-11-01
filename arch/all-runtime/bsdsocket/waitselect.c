/*
    Copyright (C) 2020-2024, The AROS Development Team. All rights reserved.
*/

#include <unistd.h>
#include <sys/select.h>
#include <proto/dos.h>
#include <aros/debug.h>

#include "forwarders_support.h"
void __fs_fsset_conv_aros_unix(fd_set *_aros, int arosmaxfd, fd_set *_unix, int *unixmaxfd);
void __fs_fsset_sync_unix_aros(fd_set *_unix, fd_set *_aros, int arosmaxfd);

/*****************************************************************************

    NAME */
#include <proto/socket.h>

        AROS_LH6(int, WaitSelect,

/*  SYNOPSIS */
        AROS_LHA(int,              nfds,      D0),
        AROS_LHA(fd_set *,         readfds,   A0),
        AROS_LHA(fd_set *,         writefds,  A1),
        AROS_LHA(fd_set *,         exceptfds, A2),
        AROS_LHA(struct timeval *, timeout,   A3),
        AROS_LHA(ULONG *,          sigmask,   D1),

/*  LOCATION */
        struct SocketBase *, SocketBase, 21, BSDSocket)

/*  FUNCTION

    INPUTS

    RESULT

    TAGS

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY


*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    BOOL cont = TRUE;
    ULONG rcvd = 0;
    int __selectresult = 0;
    ULONG _tsmask = sigmask ? *sigmask : 0;
    int maxfd = nfds - 1;
    fd_set *pread = readfds; fd_set *pwrite = writefds; fd_set *perror = exceptfds;
    int timeoutiters = 0;
    const int ITER_SLEEP = 50; /* microseconds */

    fd_set tmpreadfds;
    fd_set tmpwritefds;
    fd_set tmperrorfds;

    /* Timeout handling: >0, 0, NULL
        if > 0, loop for timeoutiters iterations of ITER_SLEEP length
        if == 0, don't wait in select, don't loop
        if == NULL, loop for infinite number of iterations of ITER_SLEEP length and wait for
                    signal to happen
    */
    if (timeout)
    {
        timeoutiters = timeout->tv_usec / ITER_SLEEP;
    }

    if (SocketBase->sb_Flags & SB_FLAG_CLIENT_IS_AROS_PROGRAM)
    {
        maxfd = -1;
        FD_ZERO(&tmpreadfds); FD_ZERO(&tmpwritefds); FD_ZERO(&tmperrorfds);

        __fs_fsset_conv_aros_unix(readfds, nfds,   &tmpreadfds, &maxfd);
        __fs_fsset_conv_aros_unix(writefds, nfds,  &tmpwritefds, &maxfd);
        __fs_fsset_conv_aros_unix(exceptfds, nfds, &tmperrorfds, &maxfd);

        pread = &tmperrorfds; pwrite = &tmpwritefds; perror = &tmperrorfds;

        if (timeout)
        {
            /* AROS timeval is 32-bit */
            timeoutiters = timeout->tv_micro / ITER_SLEEP;
        }
    }

    do
    {
        /* do pooling with some small sleep, re-assign each iteration as select() clears _t */
        struct timeval _t;
        _t.tv_sec  = 0;
        _t.tv_usec = (timeout == NULL || timeoutiters > 0) ? ITER_SLEEP : 0;

        __selectresult = select(maxfd + 1, pread, pwrite, perror, &_t);
        rcvd = SetSignal(0L, _tsmask);

        if (rcvd != 0 || __selectresult != 0)
        {
            cont = FALSE;
            /* 
             * If select timeouts, __selectresult = 0, if found FDs, __selectresult is > 0
             * In both cases the value is good for returning even if there was signal raised
             */
        }

        if (timeout)
        {
            if (timeoutiters == 0) cont = FALSE;
            else timeoutiters--;
        }

    }while (cont);

    if (__selectresult >= 0 && sigmask)
        *sigmask &= rcvd;

    if (SocketBase->sb_Flags & SB_FLAG_CLIENT_IS_AROS_PROGRAM)
    {
        __fs_fsset_sync_unix_aros(&tmpreadfds,  readfds, nfds);
        __fs_fsset_sync_unix_aros(&tmpwritefds, writefds, nfds);
        __fs_fsset_sync_unix_aros(&tmperrorfds, exceptfds, nfds);
    }

    return __selectresult;

    AROS_LIBFUNC_EXIT
} /* WaitSelect */

