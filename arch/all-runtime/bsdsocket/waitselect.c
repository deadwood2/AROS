/*
    Copyright © 2020, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <sys/select.h>
#include <proto/dos.h>
#include <aros/debug.h>

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
        struct Library *, SocketBase, 21, BSDSocket)

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
    struct timeval _t = {0, 20}; /* do pooling with some small sleep*/
    ULONG _tsmask = sigmask ? *sigmask : 0;

    if (timeout)
    {
        /* use couter = timeout/_t to exit with timeout after counter iterations of pooling */
        (bug("<<WARN>>: Handling 'timeout' in WaitSelect is not implemented. Please submit issuet at https://github.com/deadw00d/AROS/issues.\n"));
        return __selectresult;
    }

    do
    {
        rcvd = CheckSignal(_tsmask);
        __selectresult = select(nfds, readfds, writefds, exceptfds, &_t);

        if (rcvd != 0 || __selectresult != 0)
        {
            cont = FALSE;
            /* 
             * If select timeouts, __selectresult = 0, if found FDs, __selectresult is > 0
             * In both cases the value is good for returning even if there was signal raised
             */
        }

    }while (cont);

    if (__selectresult >= 0 && sigmask)
        *sigmask &= rcvd;

    return __selectresult;

    AROS_LIBFUNC_EXIT
} /* WaitSelect */

