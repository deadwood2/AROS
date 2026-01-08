/*
    Copyright (C) 2025-2026, The AROS Development Team. All rights reserved.
*/

#ifndef _EXEC_PORTS_H
#define _EXEC_PORTS_H

struct MsgPortProxy * MsgPortV0_getproxy(struct MsgPortV0 *port);
struct MsgPort * MsgPortV0_getnative(struct MsgPortV0 *port);
BOOL MsgPortV0_containsnative(struct MsgPortV0 *port);
void MsgPortV0_fixed_connectnative(struct MsgPortV0 *portv0, struct MsgPort *portnative);

void Exec_Ports_init(struct ExecBaseV0 *abiv0SysBase);

#endif
