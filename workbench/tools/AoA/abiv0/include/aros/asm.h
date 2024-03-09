#ifndef _AROS_ASM_H
#define _AROS_ASM_H

#define EXTER_PROXY(name)       \
__asm__ volatile(               \
    "   .code32\n"              \
    "proxy_" #name ":\n"

#define LEAVE_PROXY             \
    "   ret\n"                  \
    "   .code64\n"              \
    :::"%ecx");

#define ENTER64                 \
    "   subl $8, %%esp\n"       \
    "   movl $0x33, 4(%%esp)\n" \
    "   lea 1f,%%ecx\n"         \
    "   movl %%ecx, (%%esp)\n"  \
    "   lret\n"                 \
    "   .code64\n"              \
    "1:\n"

#define ENTER32                 \
    "   subq $8, %%rsp\n"       \
    "   movl $0x23, 4(%%rsp)\n" \
    "   lea 2f,%%ecx\n"         \
    "   movl %%ecx, (%%rsp)\n"  \
    "   lret\n"                 \
    "   .code32\n"              \
    "2:\n"

#define CALL_IMPL64(name)       \
    "   call abiv0_" #name "\n"

#define COPY_ARG_1              \
    "   movl 4(%%rsp), %%edi\n"

#define COPY_ARG_3              \
    "   movl 8(%%rsp), %%esi\n"

#define COPY_ARG_2              \
    "   movl 12(%%rsp), %%edx\n"

#endif