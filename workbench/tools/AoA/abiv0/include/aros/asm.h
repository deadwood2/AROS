#ifndef ABIV0_AROS_ASM_H
#define ABIV0_AROS_ASM_H

/* To be used in 32-bit code */

#define ENTER64                 \
    "   subl $8, %%esp\n"       \
    "   movl $0x33, 4(%%esp)\n" \
    "   lea 1f,%%ecx\n"         \
    "   movl %%ecx, (%%esp)\n"  \
    "   lret\n"                 \
    "   .code64\n"              \
    "1:\n"

#define STORE_ESI_EDI           \
    "   push %%edi\n"           \
    "   push %%esi\n"           \

#define RESTORE_ESI_EDI         \
    "   pop %%esi\n"            \
    "   pop %%edi\n"            \


/* To be used in 64-bit code */

#define ENTER32                 \
    "   subq $8, %%rsp\n"       \
    "   movl $0x23, 4(%%rsp)\n" \
    "   lea 2f,%%ecx\n"         \
    "   movl %%ecx, (%%rsp)\n"  \
    "   lret\n"                 \
    "   .code32\n"              \
    "2:\n"                      \
    "   push $0x2b\n"           \
    "   pop %%ds\n"             \
    "   push $0x2b\n"           \
    "   pop %%es\n"

#define ALIGN_STACK64           \
    "   push %%r12\n"           \
    "   movq %%rsp, %%r12\n"    \
    "   andq $-16, %%rsp\n"     \

#define RESTORE_STACK64         \
    "   movq %%r12, %%rsp\n"    \
    "   popq %%r12\n"           \

/* +8 because of stored esi/edi */
/* +8 because of stored r12     */

#define SET_ARG64__1_FROM32     \
    "   movl  4+8+8(%%r12), %%edi\n"

#define SET_ARG64__2_FROM32     \
    "   movl  8+8+8(%%r12), %%esi\n"

#define SET_ARG64__3_FROM32     \
    "   movl 12+8+8(%%r12), %%edx\n"

#define SET_ARG64__4_FROM32     \
    "   movl 16+8+8(%%r12), %%ecx\n"

#define SET_ARG64__5_FROM32     \
    "   movl 20+8+8(%%r12), %%r8d\n"

#define SET_ARG64__6_FROM32     \
    "   movl 24+8+8(%%r12), %%r9d\n"

#define SET_ARG64__7_FROM32     \
    "   movl 28+8+8(%%r12), %%eax\n" \
    "   push %%rax\n"

#define SET_ARG64__8_FROM32     \
    "   movl 32+8+8(%%r12), %%eax\n" \
    "   push %%rax\n"

#define SET_ARG64__9_FROM32     \
    "   movl 36+8+8(%%r12), %%eax\n" \
    "   push %%rax\n"

#define SET_ARG64_10_FROM32     \
    "   movl 40+8+8(%%r12), %%eax\n" \
    "   push %%rax\n"

#define SET_ARG64_11_FROM32     \
    "   movl 44+8+8(%%r12), %%eax\n" \
    "   push %%rax\n"

#define SET_ARG64_12_FROM32     \
    "   movl 48+8+8(%%r12), %%eax\n" \
    "   push %%rax\n"

#endif
