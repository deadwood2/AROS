/*
    Copyright (C) 2026, The AROS Development Team. All rights reserved.
*/

#ifndef _EXEC_LIBRARIES
#define _EXEC_LIBRARIES

void Exec_Libraries_init(struct ExecBaseV0 *abiv0SysBase);

struct ResidentV0 * findResident(BPTR seg, CONST_STRPTR name);

#endif
