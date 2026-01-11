/*
    Copyright (C) 2025-2026, The AROS Development Team. All rights reserved.
*/

#ifndef ABIV0_AROS_CALL32_H
#define ABIV0_AROS_CALL32_H

#include "./asm.h"

#define CALL32_ARG_1(res, faddr, arg1)  \
    __asm__ volatile (          \
    "subq $4, %%rsp\n"          \
    "movl %2, %%eax\n"          \
    "movl %%eax, (%%rsp)\n"     \
    "movl %1, %%eax\n"          \
    ENTER32                     \
    "call *%%eax\n"             \
    ENTER64                     \
    "addq $4, %%rsp\n"          \
    "movl %%eax, %0\n"          \
    :"=m"(res)                  \
    :"mr"(faddr), "mr"(arg1)    \
    : SCRATCH_REGS_64_TO_32 );

#define CALL32_ARG_2(res, faddr, arg1, arg2)  \
    __asm__ volatile (          \
    "subq $8, %%rsp\n"          \
    "movl %3, %%eax\n"          \
    "movl %%eax, 4(%%rsp)\n"    \
    "movl %2, %%eax\n"          \
    "movl %%eax, (%%rsp)\n"     \
    "movl %1, %%eax\n"          \
    ENTER32                     \
    "call *%%eax\n"             \
    ENTER64                     \
    "addq $8, %%rsp\n"          \
    "movl %%eax, %0\n"          \
    :"=m"(res)                  \
    :"mr"(faddr), "mr"(arg1), "mr"(arg2)  \
    : SCRATCH_REGS_64_TO_32 );

#define CALL32_ARG_3(res, faddr, arg1, arg2, arg3)  \
    __asm__ volatile (          \
    "subq $12, %%rsp\n"         \
    "movl %4, %%eax\n"          \
    "movl %%eax, 8(%%rsp)\n"    \
    "movl %3, %%eax\n"          \
    "movl %%eax, 4(%%rsp)\n"    \
    "movl %2, %%eax\n"          \
    "movl %%eax, (%%rsp)\n"     \
    "movl %1, %%eax\n"          \
    ENTER32                     \
    "call *%%eax\n"             \
    ENTER64                     \
    "addq $12, %%rsp\n"         \
    "movl %%eax, %0\n"          \
    :"=m"(res)                  \
    :"mr"(faddr), "mr"(arg1), "mr"(arg2), "mr"(arg3)    \
    : SCRATCH_REGS_64_TO_32 );

#define CALL32_ARG_3_NR(faddr, arg1, arg2, arg3)    \
    {                                               \
        LONG _dummy;                                \
        CALL32_ARG_3(_dummy, faddr, arg1, arg2, arg3)\
    }
#endif
