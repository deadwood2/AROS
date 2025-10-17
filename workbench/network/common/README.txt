This directory is for common includes and utilities for AROS TCP/IP
applications compliant to the bsdsocket.library interface, regardless
of the underlying network stack.


*Exposed in bsdsocket.library/miami.library calls*
    Highly likely to be used in user applications. Keep compatible.

net/if.h
    struct if_nameindex [*]

net/if_dl.h
    struct sockaddr_dl (not passed in, but sockaddr can be casted to
                        this structure for reading on client side)

netinet/in.h
    in_addr_t
    in_port_t
    sa_family_t
    struct in_addr
    socklen_t
    struct sockaddr_in

sys/net_types.h
    fdset
    fdmask [*] <-- conflict with sys/select.h
    caddr_t

sys/_sockaddr_storage.h
    struct sockaddr_storage (used to replace sockaddr_in in client code)

sys/socket.h
    struct linger [*]
    struct sockaddr
    struct msghdr [*]
    struct splice

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
    struct arpstat

netinet/in.h
    struct ip_opts
    struct in6_addr
    struct sockaddr_in6

netinet/tcp.h
    tcp_seq
    tcp_cc
    struct tcphdr
    struct tcp_perf_info
    struct tcp_info
    struct tcp_fastopen
    struct stack_specific_info
    struct tcp_function_set
    struct tcp_snd_req
    union tcp_log_userdata
    struct tcp_log_user
    struct tcp_hybrid_req

sys/net_types.h
    dev_t
    ino_t
    off_t

sys/socket.h
    struct accept_filter_arg
    struct sockproto
    struct cmsghdr
    struct cmsgcred
    struct sockcred
    struct sockcred2
    struct sock_timestamp_info
    struct osockaddr
    struct omsghdr
    struct sf_hdtr
    struct mmsghdr



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

