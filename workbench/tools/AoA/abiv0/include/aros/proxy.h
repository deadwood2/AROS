#ifndef ABIV0_AROS_PROXY_H
#define ABIV0_AROS_PROXY_H

#include "./asm.h"

#define EXTER_PROXY(name)       \
__asm__ volatile(               \
    "   .code32\n"              \
    "proxy_" #name ":\n"

#define LEAVE_PROXY             \
    "   ret\n"                  \
    "   .code64\n"              \
    :::"%ecx");

#define CALL_IMPL64(name)       \
    "   call abiv0_" #name "\n"

#define MAKE_PROXY_ARG_1(fname) \
void proxy_##fname();           \
void dummy_##fname()            \
{                               \
    EXTER_PROXY(fname)          \
    STORE_ESI_EDI               \
    ENTER64                     \
    ALIGN_STACK64               \
    SET_ARG64__1_FROM32         \
    CALL_IMPL64(fname)          \
    RESTORE_STACK64             \
    ENTER32                     \
    RESTORE_ESI_EDI             \
    LEAVE_PROXY                 \
}

#define MAKE_PROXY_ARG_2(fname) \
void proxy_##fname();           \
void dummy_##fname()            \
{                               \
    EXTER_PROXY(fname)          \
    STORE_ESI_EDI               \
    ENTER64                     \
    ALIGN_STACK64               \
    SET_ARG64__2_FROM32         \
    SET_ARG64__1_FROM32         \
    CALL_IMPL64(fname)          \
    RESTORE_STACK64             \
    ENTER32                     \
    RESTORE_ESI_EDI             \
    LEAVE_PROXY                 \
}

#define MAKE_PROXY_ARG_3(fname) \
void proxy_##fname();           \
void dummy_##fname()            \
{                               \
    EXTER_PROXY(fname)          \
    STORE_ESI_EDI               \
    ENTER64                     \
    ALIGN_STACK64               \
    SET_ARG64__3_FROM32         \
    SET_ARG64__2_FROM32         \
    SET_ARG64__1_FROM32         \
    CALL_IMPL64(fname)          \
    RESTORE_STACK64             \
    ENTER32                     \
    RESTORE_ESI_EDI             \
    LEAVE_PROXY                 \
}

#define MAKE_PROXY_ARG_4(fname) \
void proxy_##fname();           \
void dummy_##fname()            \
{                               \
    EXTER_PROXY(fname)          \
    STORE_ESI_EDI               \
    ENTER64                     \
    ALIGN_STACK64               \
    SET_ARG64__4_FROM32         \
    SET_ARG64__3_FROM32         \
    SET_ARG64__2_FROM32         \
    SET_ARG64__1_FROM32         \
    CALL_IMPL64(fname)          \
    RESTORE_STACK64             \
    ENTER32                     \
    RESTORE_ESI_EDI             \
    LEAVE_PROXY                 \
}

#define MAKE_PROXY_ARG_5(fname) \
void proxy_##fname();           \
void dummy_##fname()            \
{                               \
    EXTER_PROXY(fname)          \
    STORE_ESI_EDI               \
    ENTER64                     \
    ALIGN_STACK64               \
    SET_ARG64__5_FROM32         \
    SET_ARG64__4_FROM32         \
    SET_ARG64__3_FROM32         \
    SET_ARG64__2_FROM32         \
    SET_ARG64__1_FROM32         \
    CALL_IMPL64(fname)          \
    RESTORE_STACK64             \
    ENTER32                     \
    RESTORE_ESI_EDI             \
    LEAVE_PROXY                 \
}

#define MAKE_PROXY_ARG_6(fname) \
void proxy_##fname();           \
void dummy_##fname()            \
{                               \
    EXTER_PROXY(fname)          \
    STORE_ESI_EDI               \
    ENTER64                     \
    ALIGN_STACK64               \
    SET_ARG64__6_FROM32         \
    SET_ARG64__5_FROM32         \
    SET_ARG64__4_FROM32         \
    SET_ARG64__3_FROM32         \
    SET_ARG64__2_FROM32         \
    SET_ARG64__1_FROM32         \
    CALL_IMPL64(fname)          \
    RESTORE_STACK64             \
    ENTER32                     \
    RESTORE_ESI_EDI             \
    LEAVE_PROXY                 \
}

/* With arguments 7 and above, be sure to maintain 16-byte aligment of stack by always copying
   even number of arguments or pushing a dummy argument on stack */
#define MAKE_PROXY_ARG_12(fname)\
void proxy_##fname();           \
void dummy_##fname()            \
{                               \
    EXTER_PROXY(fname)          \
    STORE_ESI_EDI               \
    ENTER64                     \
    ALIGN_STACK64               \
    SET_ARG64_12_FROM32         \
    SET_ARG64_11_FROM32         \
    SET_ARG64_10_FROM32         \
    SET_ARG64__9_FROM32         \
    SET_ARG64__8_FROM32         \
    SET_ARG64__7_FROM32         \
    SET_ARG64__6_FROM32         \
    SET_ARG64__5_FROM32         \
    SET_ARG64__4_FROM32         \
    SET_ARG64__3_FROM32         \
    SET_ARG64__2_FROM32         \
    SET_ARG64__1_FROM32         \
    CALL_IMPL64(fname)          \
    RESTORE_STACK64             \
    ENTER32                     \
    RESTORE_ESI_EDI             \
    LEAVE_PROXY                 \
}

#endif
