/*
    Copyright (C) 2024, The AROS Development Team. All rights reserved.
*/

#ifndef FORWARDERS_SUPPORT_H
#define FORWARDERS_SUPPORT_H

#include "socketbase.h"

void __fs_translate_errno(int unix_errno, struct SocketBase *SocketBase);

int __fs_obtain_mapping(int unix_s);
int __fs_translate_socket(int aros_s);
int __fs_release_mapping(int aros_s);

unsigned long __fs_translate_ioctl_request(unsigned long aros_request);

#endif
