This directory is for common includes and utilities for AROS TCP/IP
applications compliant to the bsdsocket.library interface, regardless
of the underlying network stack.


*Exposed in bsdsocket.library/miami.library calls*
    Highly likely to be used in user applications. Keep compatible.

net/if.h
    struct if_nameindex [*]

net/if_dl.h
    struct sockaddr_dl

netinet/in.h
    in_addr_t
    struct in_addr
    struct sockaddr_in

sys/net_types.h
    fdset
    fdmask [*] <-- conflict with sys/select.h
    caddr_t

sys/socket.h
    struct linger [*]
    struct sockaddr
    struct msghdr [*]

sys/un.h
    struct sockaddr_un

netdb.h
    struct hostent
    struct netent [*]
    struct servent
    struct protoent
    struct addrinfo [*]

IoctlSocket(FIONBIO) [*]


*Exposed via IoctlSocket *
    Specialized calls. Low propabality of being used in user application.
    Keep compatible, unless a fix is necessary.

net/if.h
    struct ifreq
        SIOCGIFFLAGS, SIOCGIFMETRIC, SIOCSIFMETRIC, SIOCGIFMTU, SIOCSIFMTU,
        SIOCDIFADDR, SIOCGIFADDR, SIOCGIFNETMASK, SIOCGIFDSTADDR,
        SIOCGIFBRDADDR
    struct ifaliasreq
        SIOCAIFADDR
    struct ifconf
        SIOCGIFCONF
net/if_arp.h
    struct arpreq
        SIOCSARP, SIOCGARP, SIOCDARP
    struct arptabreq [*]
        SIOCGARPT
net/route.h
    struct ortentry [*]
        SIOCADDRT, SIOCDELRT


*Exposed but not used (functionality not implemented)*
    Not functioning. No propability of being used. Allow fixes and changes.

libraries/bsdsocket.h
    struct IPFilterMsg
    struct TCPConnectMonitorMessage
    struct UDPMonitorMessage
    struct ICMPMonitorMessage

*Exposed but not used (nothing takes these as arguments)*
    No propability of being used. Allow fixes and changes.

net/if.h
    struct if_data [*]
    struct ifnet [*]
    struct ifaddr
    struct if_msghdr
    struct ifa_msghdr

net/if_arp.h
    struct arphdr

netinet/in.h
    struct ip_opts
    struct in6_addr
    struct sockaddr_in6

netinet/tcp.h
    tcp_seq
    tcp_cc
    struct tcphdr

sys/net_types.h
    dev_t
    ino_t
    off_t

sys/socket.h
    struct sockproto
    struct cmsghdr [*]
    struct osockaddr
    struct omsghdr [*]



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

