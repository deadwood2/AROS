#ifndef EXEC_INTERRUPTS_H
#define EXEC_INTERRUPTS_H

/*
    Copyright © 1995-2011, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Interrupt structures
    Lang: english
*/

#ifndef EXEC_LISTS_H
#   include <exec/lists.h>
#endif
#ifndef EXEC_NODES_H
#   include <exec/nodes.h>
#endif

/* CPU-dependent struct ExceptionContext */
#if defined __i386__
#include <aros/i386/cpucontext.h>
#elif defined __x86_64__
#include <aros/x86_64/cpucontext.h>
#elif defined __mc68000__
#include <aros/m68k/cpucontext.h>
#elif defined __powerpc__
#include <aros/ppc/cpucontext.h>
#elif defined __aarch64__
#include <aros/aarch64/cpucontext.h>
#elif defined __arm__
#include <aros/arm/cpucontext.h>
#else
#error unsupported CPU type
#endif

struct Interrupt
{
    struct Node is_Node;
    APTR        is_Data;
    VOID     (* is_Code)(); /* server code entry */
};

/* PRIVATE */
struct IntVector
{
    APTR          iv_Data;
    VOID       (* iv_Code)();
    struct Node * iv_Node;
};

/* PRIVATE */
struct SoftIntList
{
    struct List sh_List;
    UWORD       sh_Pad;
};

#define SIH_PRIMASK (0xf0)

#define INTB_NMI      15
#define INTF_NMI (1L<<15)

/* Offset of kernel interrupt vectors.
 *
 * Usage:
 *   AddIntServer(INTB_KERNEL + irq, irq_handler);
 *   RemIntServer(INTB_KERNEL + irq, irq_handler);
 */
#define INTB_KERNEL       (16)

#ifdef __AROS__
/* AROS helper macros.
 * These are used to abstract away architectural
 * differences between AROS ports.
 */
#include <aros/asmcall.h>

/* ABI_V0 compatibility:
 * Definitions below are adjusted to required method signatures
 */

/* Define a function prototype and call methods
 * for SetIntVector() and AddIntServer()
 * struct Interrupt is_Code fields.
 *
 * The prototype matches:
 *
 * ULONG func(struct Custom *custom, APTR data, ULONG intmask,
 *            struct ExecBase *sysBase, VOID_FUNC code)
 *            (A0, A1, A5, A6, D1)
 *
 * Handler should return TRUE (interrupt handled)
 * or FALSE (continue interrupt processing)
 */

/* ABI_V0 compatibility:
 * Original AROS codes accepts only first 4 parameters. To
 * access the code parameter, the source code needs to be
 * changed to the macro below. Passing the additional paramer
 * will not break compatibility as parameters are passed in
 * reversed order, meaning the offset of first four parameters
 * from EBP remains unchanged
 */

#define AROS_INTP(n)                                       \
        AROS_UFP5(ULONG, n,                                \
          AROS_UFPA(APTR,      __ufi_custom,  A0),         \
          AROS_UFPA(APTR,      __ufi_data,    A1),         \
          AROS_UFPA(ULONG,     __ufi_intmask, A5),         \
          AROS_UFPA(struct ExecBase *, __ufi_SysBase, A6),\
          AROS_UFPA(VOID_FUNC, __ufi_code,    D1))


#define AROS_INTC4(n, data, intmask, custom, code)         \
        AROS_UFC5(ULONG, n,                                \
                AROS_UFCA(APTR,      custom,  A0),         \
                AROS_UFCA(APTR,      data,    A1),         \
                AROS_UFCA(ULONG,     intmask, A5),         \
                AROS_UFCA(struct ExecBase *, SysBase, A6),\
                AROS_UFCA(VOID_FUNC, code,    D1))

#define AROS_INTC3(n, data, intmask, custom) AROS_INTC4(n, data, intmask, custom, (VOID_FUNC)(n))
#define AROS_INTC2(n, data, intmask)         AROS_INTC4(n, data, intmask, (APTR)(IPTR)0xdff000, (VOID_FUNC)(n));
#define AROS_INTC1(n, data)                  AROS_INTC4(n, data, 0, (APTR)(IPTR)0xdff000, (VOID_FUNC)(n));



#define AROS_INTH4(n, type, data, intmask, custom, code)   \
        AROS_UFH5(ULONG, n,                                \
          AROS_UFHA(APTR,      custom,     A0),            \
          AROS_UFHA(APTR,      __ufi_data, A1),            \
          AROS_UFHA(ULONG,     intmask,    A5),            \
          AROS_UFHA(struct ExecBase *, SysBase, A6),       \
          AROS_UFHA(VOID_FUNC, code,       D1)              \
        ) { AROS_USERFUNC_INIT                             \
            type __unused data = __ufi_data;

#define AROS_INTH3(n, type, data, intmask, custom) AROS_INTH4(n, type, data, intmask, custom, __ufi_code)
#define AROS_INTH2(n, type, data, intmask)         AROS_INTH4(n, type, data, intmask, __ufi_custom, __ufi_code)
#define AROS_INTH1(n, type, data)                  AROS_INTH4(n, type, data, __ufi_intmask, __ufi_custom, __ufi_code)
#define AROS_INTH0(n)                              AROS_INTH4(n, APTR, data, __ufi_intmask, __ufi_custom, __ufi_code)

/* ABI_V0 compatibility:
 * Define a function prototype and call methods
 * for Cause() and AddResetCallback()
 * struct Interrupt is_Code fields.
 *
 * The prototype matches:
 *
 * ULONG func(APTR data, VOID_FUNC code, struct ExecBase *sysBase)
 *            (A1, A5, A6)
 */

#define AROS_SOFTINTP(n)                                    \
        AROS_UFP3(ULONG, n,                                 \
          AROS_UFPA(APTR,      __ufi_data,    A1),          \
          AROS_UFPA(VOID_FUNC, __ufi_code,    A5),          \
          AROS_UFPA(struct ExecBase *, __ufi_SysBase, A6))

#define AROS_SOFTINTC2(n, data, code)         \
        AROS_UFC3(ULONG, n,                                 \
                AROS_UFCA(APTR,      data,    A1),          \
                AROS_UFCA(VOID_FUNC, code,    A5),          \
                AROS_UFCA(struct ExecBase *, SysBase, A6))  \

#define AROS_SOFTINTC1(n, data)                     AROS_SOFTINTC2(n, data, (VOID_FUNC)(n));

#define AROS_SOFTINTH2(n, type, data, code)                 \
        AROS_UFH3(ULONG, n,                                \
          AROS_UFHA(APTR,      __ufi_data, A1),            \
          AROS_UFHA(VOID_FUNC, code,       A5),             \
          AROS_UFHA(struct ExecBase *, SysBase, A6)         \
        ) { AROS_USERFUNC_INIT                             \
            type __unused data = __ufi_data;

#define AROS_SOFTINTH1(n, type, data)               AROS_SOFTINTH2(n, type, data, __ufi_code)

#ifdef __mc68000
/* Special hack for setting the 'Z' condition code upon exit
 * for m68k architectures.
 */
#define AROS_INTFUNC_INIT inline ULONG _handler(void) {
#define AROS_INTFUNC_EXIT }; register ULONG _res asm ("d0") = _handler();     \
                             asm volatile ("tst.l %0\n" : : "r" (_res)); \
                             return _res; /* gcc only generates movem/unlk/rts */   \
                             AROS_USERFUNC_EXIT }
#else  /* ! __mc68000 */
/* Everybody else */
#define AROS_INTFUNC_INIT
#define AROS_INTFUNC_EXIT       AROS_USERFUNC_EXIT }
#endif /* ! __mc68000 */

#endif /* __AROS__ */

#endif /* EXEC_INTERRUPTS_H */
