/*
    Copyright (C) 2024, The AROS Development Team. All rights reserved.
*/

#ifndef FORWARDERS_SUPPORT_H
#define FORWARDERS_SUPPORT_H

#include "socketbase.h"

void __fs_translate_errno(int unix_errno, struct SocketBase *SocketBase);

#endif
