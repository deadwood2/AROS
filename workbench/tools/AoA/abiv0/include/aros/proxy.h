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

#define MAKE_PROXY_ARG_2(fname) \
void proxy_##fname();           \
void dummy_##fname()            \
{                               \
    EXTER_PROXY(fname)          \
    ENTER64                     \
    COPY_ARG_1                  \
    COPY_ARG_2                  \
    CALL_IMPL64(fname)          \
    ENTER32                     \
    LEAVE_PROXY                 \
}

#define MAKE_PROXY_ARG_3(fname) \
void proxy_##fname();           \
void dummy_##fname()            \
{                               \
    EXTER_PROXY(fname)          \
    ENTER64                     \
    COPY_ARG_1                  \
    COPY_ARG_2                  \
    COPY_ARG_3                  \
    CALL_IMPL64(fname)          \
    ENTER32                     \
    LEAVE_PROXY                 \
}

#define MAKE_PROXY_ARG_4(fname) \
void proxy_##fname();           \
void dummy_##fname()            \
{                               \
    EXTER_PROXY(fname)          \
    ENTER64                     \
    COPY_ARG_1                  \
    COPY_ARG_2                  \
    COPY_ARG_3                  \
    COPY_ARG_4                  \
    CALL_IMPL64(fname)          \
    ENTER32                     \
    LEAVE_PROXY                 \
}

#define MAKE_PROXY_ARG_6(fname) \
void proxy_##fname();           \
void dummy_##fname()            \
{                               \
    EXTER_PROXY(fname)          \
    ENTER64                     \
    COPY_ARG_1                  \
    COPY_ARG_2                  \
    COPY_ARG_3                  \
    COPY_ARG_4                  \
    COPY_ARG_5                  \
    COPY_ARG_6                  \
    CALL_IMPL64(fname)          \
    ENTER32                     \
    LEAVE_PROXY                 \
}

#endif