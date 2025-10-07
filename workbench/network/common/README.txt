This directory is for common includes and utilities for AROS TCP/IP
applications compliant to the bsdsocket.library interface, regardless
of the underlying network stack.

The following structures are considered public as they are exposed in
one or more of bsdsocket.library functions. All other structures and
headers are considered internal.

Based on review of common/include/clib/#?_protos.h,
common/include/libraries/bsdsocket.h and code in libnetlib.a

*Exposed and used*

net/if.h
    struct if_nameindex [*]

net/if_dl.h
    struct sockaddr_dl

netinet/in.h
    struct sockaddr_in
    struct in_addr
    in_addr_t

sys/net_types.h
    fdset
    fdmask [*] <-- conflict with sys/select.h
    caddr_t

sys/socket.h
    struct sockaddr
    struct msghdr [*]

netdb.h
    struct addrinfo [*]
    struct hostent
    struct protoent
    struct netent [*]
    struct servent


*Exposed but not used (functionality not implemented)*

net/if.h
    struct ifnet <--- pointer is a part of struct IPFilterMsg
        struct ifaddr
        struct if_data [*]

net/route.h
    struct rt_msghdr [*]
    struct rt_metrics [*]

netinet/tcp.h
    struct tcphdr <--- pointer is a part of struct TCPConnectMonitorMessage

netinet/udp.h
    struct udphdr <--- pointer is a prt of struct UDPMonitorMessage

netinet/ip_icmp.h
    struct icmp <--- pointer is a part of struct ICMPMonitorMessage

netinet/ip.h
    struct ip <--- pointer is a part of struct IPFilterMsg

sys/mbuf.h
    struct mbuf <--- pointer is a part of struct IPFilterMsg

[*] - structure contains 'long' or 'ulong' variable which makes the field
have different size on 32-bit vs 64-bit

!! Using long in interface
IoctlSocket
Inet_NtoA
inet_addr
Inet_LnaOf
Inet_NetOf
Inet_MakeAddr
inet_network
gethostid
most of ROADSHOW interface

