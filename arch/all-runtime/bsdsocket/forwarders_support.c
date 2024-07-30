/*
    Copyright (C) 2024, The AROS Development Team. All rights reserved.
*/

#include <aros/debug.h>

#include "forwarders_support.h"
#include <errno.h>
#include <sys/select.h>
#include <sys/ioctl.h>

/* AROS FD_SETSIZE is 64, all socket numbers must fit within that size. Even though the library base is per-task, the
   mapping is shared across all tasks. This is to simplify ReleseSocket/ObtainSocket and have it the same for AROS and
   AxRuntime programs. The limit of 64 for all tasks should not be a problem as each program is a new instance of
   runtime and gets it's own mapping. */
static int socket_mapping[64] = {[0 ... 63] = -1};

int __fs_obtain_mapping(int unix_s)
{
    for (int i = 4; i < 64; i++)
    {
        if (socket_mapping[i] == -1)
        {
            socket_mapping[i] = unix_s;
            return i;
        }
    }

    return -1;
}
int __fs_translate_socket(int aros_s)
{
    return socket_mapping[aros_s];
}

int __fs_release_mapping(int aros_s)
{
    int ret = socket_mapping[aros_s];
    socket_mapping[aros_s] = -1;
    return ret;
}

void __fs_fsset_conv_aros_unix(fd_set *_aros, int arosmaxfd, fd_set *_unix, int *unixmaxfd)
{
    for (int i = 0; i < arosmaxfd; i++)
    {
        if (FD_ISSET(i, _aros))
        {
            int ts = __fs_translate_socket(i);
            FD_SET(ts, _unix);
            if (*unixmaxfd < ts) *unixmaxfd = ts;
        }
    }
}

void __fs_fsset_sync_unix_aros(fd_set *_unix, fd_set *_aros, int arosmaxfd)
{
    for (int i = 0; i < arosmaxfd; i++)
    {
        if (FD_ISSET(i, _aros))
        {
            int ts = __fs_translate_socket(i);
            if (!FD_ISSET(ts, _unix))
                FD_CLR(i, _aros);
        }
    }
}

void __fs_translate_errno(int unix_errno, struct SocketBase *SocketBase)
{
    switch(unix_errno)
    {

    case(EAGAIN):
        *SocketBase->sb_ErrnoPtr = 35;
        break;
    case(EINPROGRESS):
        *SocketBase->sb_ErrnoPtr = 36;
        break;
    case(ECONNRESET):
        *SocketBase->sb_ErrnoPtr = 54;
    default:
        bug("<<WARN>>: %s. Translation for errno(%d) is not implemented. Please submit issuet at https://github.com/deadw00d/AROS/issues.\n",
            __FUNCTION__, unix_errno);
    }
}

unsigned long __fs_translate_ioctl_request(unsigned long aros_request)
{
    switch(aros_request)
    {
    case(0x8008667E):
        return FIONBIO;
    default:
        bug("<<WARN>>: %s. Translation for request(%ld) is not implemented. Please submit issuet at https://github.com/deadw00d/AROS/issues.\n",
            __FUNCTION__, aros_request);
    }

    return aros_request;
}
