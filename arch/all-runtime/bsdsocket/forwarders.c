/*
    Copyright (C) 2023-2024, The AROS Development Team. All rights reserved.
*/

#include <aros/symbolsets.h>
#include <proto/exec.h>
#include <aros/debug.h>

#include <netdb.h>
#include <string.h>
#include <netinet/tcp.h>
#include <errno.h>

#include "forwarders_support.h"
#include "socketbase.h"

/*
    AxRuntime programs use directly API of Linux-side network stack for common functions and bsdsocket.library for
    AROS-specific functions. Thus public api of this library needs to be limited to AROS-specific functions.
    AROS x86_64 programs however expect full api of bsdsocket.library, including common network functions. This means
    some of the LVOs need to be forwarded to Linux-side network stack to provided the needed functionality. These
    functions are not exposed via genmodule build to make sure they are not visible as public API.

    Any code in this file can safely assume it will only be called by AROS x86_64 programs and never by AxRuntime
    programs
*/

AROS_LH1(struct hostent *, f_gethostbyname,
         AROS_LHA(const char *, name, A0),
         struct SocketBase *, SocketBase, 35, BSDSocket)
{
    AROS_LIBFUNC_INIT

    SocketBase->sb_Flags |= SB_FLAG_CLIENT_IS_AROS_PROGRAM;

    return gethostbyname(name);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(int, f_socket,
         AROS_LHA(int, domain, D0),
         AROS_LHA(int, type, D1),
         AROS_LHA(int, protocol, D2),
         struct SocketBase *, SocketBase, 5, BSDSocket)
{
    AROS_LIBFUNC_INIT

    SocketBase->sb_Flags |= SB_FLAG_CLIENT_IS_AROS_PROGRAM;

    int ret = socket(domain, type, protocol);
    return __fs_obtain_mapping(ret);

    AROS_LIBFUNC_EXIT
}

struct f_sockaddr {
    unsigned char   sa_len;         /* total length */
    unsigned char   sa_family;      /* address family */
    char            sa_data[14];    /* actually longer; address value */
};

AROS_LH3(int, f_connect,
         AROS_LHA(int, s, D0),
         AROS_LHA(struct f_sockaddr *, name, A0),
         AROS_LHA(socklen_t, namelen, D1),
         struct SocketBase *, SocketBase, 9, BSDSocket)
{
    AROS_LIBFUNC_INIT

    struct sockaddr tmp_name = {0};
    tmp_name.sa_family    = name->sa_family;
    memcpy(&tmp_name.sa_data, &name->sa_data, sizeof(tmp_name.sa_data));

    int ret = connect(__fs_translate_socket(s), &tmp_name, namelen);
    if (ret == -1) __fs_translate_errno(errno, SocketBase);

    return ret;

    AROS_LIBFUNC_EXIT
}

static int _translate_level(int level)
{
    switch (level)
    {
    case(0x0006): return IPPROTO_TCP;
    case(0xFFFF): return SOL_SOCKET;
    default:
        bug("<<WARN>>: level (%d) is not translated. Please submit issuet at https://github.com/deadw00d/AROS/issues.\n", level);
        return level;
    }
}

static int _translate_optname(int optname)
{
    switch (optname)
    {
    case(0x0001): return TCP_NODELAY;
    case(0x1007): return SO_ERROR;
    case(0x1001): return SO_SNDBUF;
    case(0x0004): return SO_REUSEADDR;
    default:
        bug("<<WARN>>: optname (%d) is not translated. Please submit issuet at https://github.com/deadw00d/AROS/issues.\n", optname);
        return optname;
    }
}

AROS_LH5(int, f_getsockopt,
         AROS_LHA(int, s, D0),
         AROS_LHA(int, level, D1),
         AROS_LHA(int, optname, D2),
         AROS_LHA(void *, optval, A0),
         AROS_LHA(socklen_t *, optlen, A1),
         struct Library *, SocketBase, 16, BSDSocket)
{
    AROS_LIBFUNC_INIT

    return getsockopt(__fs_translate_socket(s), _translate_level(level), _translate_optname(optname), optval, optlen);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(int, f_setsockopt,
         AROS_LHA(int, s, D0),
         AROS_LHA(int, level, D1),
         AROS_LHA(int, optname, D2),
         AROS_LHA(void *, optval, A0),
         AROS_LHA(socklen_t, optlen, D3),
         struct Library *, SocketBase, 15, BSDSocket)
{
    AROS_LIBFUNC_INIT

    return setsockopt(__fs_translate_socket(s), _translate_level(level), _translate_optname(optname), optval, optlen);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(int, f_getpeername,
         AROS_LHA(int, s, D0),
         AROS_LHA(struct f_sockaddr *, name, A0),
         AROS_LHA(socklen_t *, namelen, A1),
         struct Library *, SocketBase, 18, BSDSocket)
{
    AROS_LIBFUNC_INIT
    struct sockaddr tmp_name = {0};
    int tmp_namelen = sizeof(struct sockaddr);
    int res = getpeername(__fs_translate_socket(s), &tmp_name, &tmp_namelen);
    name->sa_len = 0;
    name->sa_family = tmp_name.sa_family;
    memcpy(name->sa_data, tmp_name.sa_data, sizeof(name->sa_data));
    *namelen = tmp_namelen;

    return res;

    AROS_LIBFUNC_EXIT
}

AROS_LH3(int, f_getsockname,
         AROS_LHA(int, s, D0),
         AROS_LHA(struct f_sockaddr *, name, A0),
         AROS_LHA(socklen_t *, namelen, A1),
         struct Library *, SocketBase, 17, BSDSocket)
{
    AROS_LIBFUNC_INIT
    struct sockaddr tmp_name = {0};
    int tmp_namelen = sizeof(struct sockaddr);
    int res = getsockname(__fs_translate_socket(s), &tmp_name, &tmp_namelen);
    name->sa_len = 0;
    name->sa_family = tmp_name.sa_family;
    memcpy(name->sa_data, tmp_name.sa_data, sizeof(name->sa_data));
    *namelen = tmp_namelen;

    return res;

    AROS_LIBFUNC_EXIT
}

AROS_LH4(int, f_send,
         AROS_LHA(int, s, D0),
         AROS_LHA(const void *, msg, A0),
         AROS_LHA(int, len, D1),
         AROS_LHA(int, flags, D2),
         struct SocketBase *, SocketBase, 11, BSDSocket)
{
    AROS_LIBFUNC_INIT

    int ret = send(__fs_translate_socket(s), msg, len, flags | MSG_NOSIGNAL);
    if (ret == -1) __fs_translate_errno(errno, SocketBase);

    return ret;

    AROS_LIBFUNC_EXIT
}

AROS_LH4(int, f_recv,
         AROS_LHA(int, s, D0),
         AROS_LHA(void *, buf, A0),
         AROS_LHA(int, len, D1),
         AROS_LHA(int, flags, D2),
         struct SocketBase *, SocketBase, 13, BSDSocket)
{
    AROS_LIBFUNC_INIT

    int ret = recv(__fs_translate_socket(s), buf, len, flags);
    if (ret == -1) __fs_translate_errno(errno, SocketBase);

    return ret;

    AROS_LIBFUNC_EXIT
}

AROS_LH3(int, f_bind,
         AROS_LHA(int, s, D0),
         AROS_LHA(struct f_sockaddr *, name, A0),
         AROS_LHA(socklen_t, namelen, D1),
         struct SocketBase *, SocketBase, 6, BSDSocket)
{
    AROS_LIBFUNC_INIT

    struct sockaddr tmp_name = {0};
    int tmp_namelen = sizeof(struct sockaddr);
    tmp_name.sa_family    = name->sa_family;
    memcpy(&tmp_name.sa_data, &name->sa_data, sizeof(tmp_name.sa_data));

    int ret = bind(__fs_translate_socket(s), &tmp_name, tmp_namelen);
    if (ret == -1) __fs_translate_errno(errno, SocketBase);

    return ret;

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, f_listen,
         AROS_LHA(int, s, D0),
         AROS_LHA(int, backlog, D1),
        struct SocketBase *, SocketBase, 7, BSDSocket)
{
    AROS_LIBFUNC_INIT

    int ret = listen(__fs_translate_socket(s), backlog);
    if (ret == -1) __fs_translate_errno(errno, SocketBase);

    return ret;

    AROS_LIBFUNC_EXIT
}

AROS_LH3(int, f_accept,
         AROS_LHA(int, s, D0),
         AROS_LHA(struct f_sockaddr *, name, A0),
         AROS_LHA(socklen_t *, namelen, A1),
         struct SocketBase *, SocketBase, 8, BSDSocket)
{
    AROS_LIBFUNC_INIT

    struct sockaddr tmp_name = {0};
    int tmp_namelen = sizeof(struct sockaddr);

    int ret = accept(__fs_translate_socket(s), &tmp_name, &tmp_namelen);
    if (ret == -1) __fs_translate_errno(errno, SocketBase);
    else
    {
        ret = __fs_obtain_mapping(ret);
        if (name != NULL && namelen != NULL)
        {
            name->sa_len = 0;
            name->sa_family = tmp_name.sa_family;
            memcpy(name->sa_data, tmp_name.sa_data, sizeof(name->sa_data));
            *namelen = tmp_namelen;
        }
    }

    return ret;

    AROS_LIBFUNC_EXIT
}

static int BSDSocket_InitForwarders(struct Library *SocketBase)
{
    SetFunction(SocketBase, (LONG)(-35 * LIB_VECTSIZE), AROS_SLIB_ENTRY(f_gethostbyname,    BSDSocket, 35));
    SetFunction(SocketBase, (LONG)( -5 * LIB_VECTSIZE), AROS_SLIB_ENTRY(f_socket,           BSDSocket,  5));
    SetFunction(SocketBase, (LONG)(-15 * LIB_VECTSIZE), AROS_SLIB_ENTRY(f_setsockopt,       BSDSocket, 15));
    SetFunction(SocketBase, (LONG)( -9 * LIB_VECTSIZE), AROS_SLIB_ENTRY(f_connect,          BSDSocket,  9));
    SetFunction(SocketBase, (LONG)(-16 * LIB_VECTSIZE), AROS_SLIB_ENTRY(f_getsockopt,       BSDSocket, 16));
    SetFunction(SocketBase, (LONG)(-18 * LIB_VECTSIZE), AROS_SLIB_ENTRY(f_getpeername,      BSDSocket, 18));
    SetFunction(SocketBase, (LONG)(-17 * LIB_VECTSIZE), AROS_SLIB_ENTRY(f_getsockname,      BSDSocket, 17));
    SetFunction(SocketBase, (LONG)(-11 * LIB_VECTSIZE), AROS_SLIB_ENTRY(f_send,             BSDSocket, 11));
    SetFunction(SocketBase, (LONG)(-13 * LIB_VECTSIZE), AROS_SLIB_ENTRY(f_recv,             BSDSocket, 13));
    SetFunction(SocketBase, (LONG)( -6 * LIB_VECTSIZE), AROS_SLIB_ENTRY(f_bind,             BSDSocket,  6));
    SetFunction(SocketBase, (LONG)( -7 * LIB_VECTSIZE), AROS_SLIB_ENTRY(f_listen,           BSDSocket,  7));
    SetFunction(SocketBase, (LONG)( -8 * LIB_VECTSIZE), AROS_SLIB_ENTRY(f_accept,           BSDSocket,  8));
    return 1;
}

ADD2OPENLIB(BSDSocket_InitForwarders, 1);
