#ifndef ABIV0_AROS_ASM_H
#define ABIV0_AROS_ASM_H

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
    "2:\n"                      \
    "   push $0x2b\n"           \
    "   pop %%ds\n"             \
    "   push $0x2b\n"           \
    "   pop %%es\n"


#define STORE_ESI_EDI           \
    "   push %%edi\n"           \
    "   push %%esi\n"           \

#define RESTORE_ESI_EDI         \
    "   pop %%esi\n"            \
    "   pop %%edi\n"            \

/* +8 because of stored esi/edi */

#define COPY_ARG_1              \
    "   movl  4+8(%%rsp), %%edi\n"

#define COPY_ARG_2              \
    "   movl  8+8(%%rsp), %%esi\n"

#define COPY_ARG_3              \
    "   movl 12+8(%%rsp), %%edx\n"

#define COPY_ARG_4              \
    "   movl 16+8(%%rsp), %%ecx\n"

#define COPY_ARG_5              \
    "   movl 20+8(%%rsp), %%r8d\n"

#define COPY_ARG_6              \
    "   movl 24+8(%%rsp), %%r9d\n"

#define ALIGN_STACK64           \
    "   push %%r12\n"           \
    "   movq %%rsp, %%r12\n"    \
    "   andq $-16, %%rsp\n"     \

#define RESTORE_STACK64         \
    "   movq %%r12, %%rsp\n"    \
    "   popq %%r12\n"           \

#endif
