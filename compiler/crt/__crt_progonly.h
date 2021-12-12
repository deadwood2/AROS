/*
    Copyright (C) 2021, The AROS Development Team. All rights reserved.

*/
#ifndef __CRT_PROGONLY_H
#define __CRT_PROGONLY_H

#include <setjmp.h>

struct CrtProgCtx;

void __progonly_set_exitjmp(jmp_buf exitjmp, jmp_buf previousjmp);
void __progonly_jmp2exit(int normal, int retcode);
int *__progonly_get_errorptr(void);
int *__progonly_set_errorptr(int *errorptr);
void __progonly_program_startup_internal(struct CrtProgCtx *ProgCtx, jmp_buf exitjmp, int *errorptr);
int __progonly_init_atexit(struct CrtProgCtx *ProgCtx);

void __progonly__Exit (int code);
void __progonly_abort (void);

#endif //__CRT_PROGONLY_H
