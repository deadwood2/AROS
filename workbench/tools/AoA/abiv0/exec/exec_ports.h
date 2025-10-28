#ifndef _EXEC_PORTS_H
#define _EXEC_PORTS_H

struct MsgPortProxy * MsgPortV0_getproxy(struct MsgPortV0 *port);
struct MsgPort * MsgPortV0_getnative(struct MsgPortV0 *port);

void Exec_Ports_init(struct ExecBaseV0 *abiv0SysBase);

#endif
