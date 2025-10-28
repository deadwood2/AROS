#ifndef ABIV0_AROS_CALL32_H
#define ABIV0_AROS_CALL32_H

#include "./asm.h"

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
    :"=m"(_ret)                 \
    :"m"(faddr), "m"(arg1), "m"(arg2)  \
    : SCRATCH_REGS_64_TO_32 );

#endif
