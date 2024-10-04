/*
    Copyright (C) 1995-2021, The AROS Development Team. All rights reserved.
*/
/*
 * This program generates the libcall.h macroset for gcc
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GENCALL_MAX     (13 + 1)        /* Max number of arguments */

#define FLAG_NR         (1 << 2)

static inline const char *nr(int flags)
{
    if (flags & FLAG_NR)
        return "NR";
    else
        return "";
}

static void aros_call_internal(int id, int flags)
{
    int i;

    printf(
            "%s",
           (flags & FLAG_NR ? "    (( t (*)(" : "    t __ret = (( t (*)(")
    );

    for (i = 0; i < id; i++)
    {
        if (i > 0)
            printf(", ");
        printf("__AROS_LDA(a%d)", i+1);
    }
    printf("))(APTR)__func)(");

    for (i = 0; i < id; i++)
    {
        if (i > 0)
            printf(", ");
        printf("__AROS_LCA(a%d)", i+1);
    }
    printf("); \\\n"
           "    AROS_LIBCALL%s_EXIT \\\n"
           "})\n",
            (flags & FLAG_NR ? "NR" : "")
    );
}

static void aros_lc(int id, int flags)
{
    /* Note that due to case of bn = (Base->OtherBase), Base cannot occupy r12 for duration of this macro.
    _bn = bn changes r12, which would mean changing Base, however Base can be later referenced in one
    of the __AROS_LCA() macros */

    printf("#define __AROS_LC%d%s(t,a,", id, nr(flags));
    for (int i = 0; i < id; i++)
        printf("a%d,", i + 1);
    printf("bt,bn,o,s) \\\n"
           "({ \\\n"
    );

    if (id < 7) {
        // Phew, all args are passed in registers this far.

        /* Problem with existing call is the following: There doesn't seem to be a way to tell
           GCC 10.5 under -O2 not to use R12 after it was assigned value of base in AROS_LIBCALL_INIT.
           This was working with GCC 6.5 though the use of global register variable, but according to
           documentation was just a side effect. This has changes with GCC 7 and new register
           allocator.

           The problem now is that in some cases, GCC will use R12 as temporary register when filling
           out function argument register (i.e. RSI) in call call:
           t __ret = __func(a1, a2, ...)
           Example: (crash in Wanderer, inline_NewObjectA when opening disk window)

           Trying to put those arguments into local variable does not solve the problem as R12
           still ends up being used just to read from stack and put into RSI . GCC seems to be very
           strict on keeping argument registers assigment just before doing call.
           The workaround below manually fills in argument registers, then sets R12 and then
           does a zero argument function call. Since call is done from C code, GCC will
           account for aligning stack to 16 bytes and for managing registers according to ABI. When
           call was done from assembler template and it was an only call in the whole function, stack
           could remaining unaligned and movaps SSE instruction in following functions would generate
           error. Also in case of call from assembler, all clobber list needed to be full:

           #define __AROS_LC2(t,a,a1,a2,bt,bn,o,s) \
           ({ \
           APTR __sto; \
           APTR _bn = (APTR)bn; \
           register UQUAD _rax asm("rax"); \
           register UQUAD _rdi asm("rdi") = (UQUAD)__AROS_LCA(a1); \
           register UQUAD _rsi asm("rsi") = (UQUAD)__AROS_LCA(a2); \
           __asm__ __volatile__("movq %%r12, %1\n    movq %5, %%r12\n    call *%c4(%%r12)\n    movq %1, %%r12" \
           : "=r"(_rax), "+mr"(__sto), "+r"(_rdi), "+r"(_rsi) \
           : "i" (-1 * (o) * LIB_VECTSIZE), "mr"(_bn), "r"(_rdi), "r"(_rsi) \
           : "cc", "memory", "rdx", "rcx", "r8", "r9", "r10", "r11"); \
           (t)_rax; \
           })
        */

        printf("    APTR __sto; \\\n"
               "    APTR _bn = (APTR)bn; \\\n");
        if (id >= 1) {
            printf("    register UQUAD _rdi __asm__(\"rdi\") = (UQUAD)__AROS_LCA(a1); \\\n");
        }
        if (id >= 2) {
            printf("    register UQUAD _rsi __asm__(\"rsi\") = (UQUAD)__AROS_LCA(a2); \\\n");
        }
        if (id >= 3) {
            printf("    register UQUAD _rdx __asm__(\"rdx\") = (UQUAD)__AROS_LCA(a3); \\\n");
        }
        if (id >= 4) {
            printf("    register UQUAD _rcx __asm__(\"rcx\") = (UQUAD)__AROS_LCA(a4); \\\n");
        }
        if (id >= 5) {
            printf("    register UQUAD _r8 __asm__(\"r8\") = (UQUAD)__AROS_LCA(a5); \\\n");
        }
        if (id >= 6) {
            printf("    register UQUAD _r9 __asm__(\"r9\") = (UQUAD)__AROS_LCA(a6); \\\n");
        }
        int bn_index;
        if (id <= 6) {
            bn_index = 2 + id;
        }
        else {
            // This should not happen
            printf("id=%d not supported yet\n", id);
            exit(1);
        }
        printf("    register APTR  _r12 __asm__(\"r12\"); \\\n"
               "    __asm__ __volatile__(\"movq %%%%r12, %%0\\n    movq %%%d, %%%%r12\\n\" \\\n"
               "    : \"+mr\"(__sto), \"+r\"(_r12)", bn_index);
        if (id >= 1) {
            printf(", \"+r\"(_rdi)");
        }
        if (id >= 2) {
            printf(", \"+r\"(_rsi)");
        }
        if (id >= 3) {
            printf(", \"+r\"(_rdx)");
        }
        if (id >= 4) {
            printf(", \"+r\"(_rcx)");
        }
        if (id >= 5) {
            printf(", \"+r\"(_r8)");
        }
        if (id >= 6) {
            printf(", \"+r\"(_r9)");
        }
        printf("\\\n"
               "    : \"mr\"(_bn) \\\n"
               "    :); \\\n");
        if (!(flags & FLAG_NR)) {
            printf("    t __ret = (( t (*)())(APTR)__AROS_GETVECADDR(_r12, o))(); \\\n");
        }
        else {
            printf("    (( t (*)())(APTR)__AROS_GETVECADDR(_r12, o))(); \\\n");
        }
        printf("    __asm__ __volatile__(\"movq %%0, %%%%r12 \" : : \"rm\"(__sto) : \"r12\"); \\\n");
        if (!(flags & FLAG_NR)) {
            printf("    __ret; \\\n");
        }
        printf("})\n");
    }
    else {
        // A lot of arguments, might not work out
        printf("    AROS_LIBCALL_INIT(bn, o) \\\n");

        aros_call_internal(id, flags);
    }

    printf("#define AROS_LC%d%s __AROS_LC%d%s\n", id, nr(flags), id, nr(flags));
}
static void aros_call(int id, int flags)
{
    /* Note that due to case of bn = (Base->OtherBase), Base cannot occupy r12 for duration of this macro.
       _bn = bn changes r12, which would mean changing Base, however Base can be later referenced in one
       of the __AROS_LCA() macros */

    printf("#define __AROS_CALL%d%s(t,a,", id, nr(flags));
    for (int i = 0; i < id; i++)
        printf("a%d,", i + 1);
    printf("bt,bn) \\\n"
           "({ \\\n"
           /* "__asm__ __volatile__( \\\n" */
           // Push r12 to stack
           // Copy input operand to r12 (callee needs r12 to be set for a specific purpose)
           // Push r13 to stack
           // Copy rsp to r13
           // Push seventh and further args to stack
           // Do any final stack alignment. Some trick with "and"?
           // Copy args 1-6 to registers
           // Call function
           // Copy r13 to rsp (restoring stack)
           // Pop r13 from stack
           // Pop r12 from stack
           // Store rax in output operand
           /* "                    );\\\n" */
           "    APTR __sto; \\\n"
           "    bt _bn = (bt)bn;\\\n"
           "    APTR __func = (APTR)a; \\\n"
           "    __asm__ __volatile__(\"movq %%%%r12, %%0\\n    movq %%1, %%%%r12\" : \"=rm\"(__sto) : \"rm\"(_bn) : \"r12\"); \\\n"
    );

    aros_call_internal(id, flags);

    printf("#define AROS_CALL%d%s __AROS_CALL%d%s\n", id, nr(flags), id, nr(flags));
}

static void aros_lh(int id, int is_ignored)
{
    int i;

    printf("#define __AROS_LH%d%s(t,n,", id, is_ignored ? "I" : "");
    for (i = 0; i < id; i++)
        printf("a%d,", i + 1);
    printf("bt,bn,o,s) \\\n");
    printf("    t AROS_SLIB_ENTRY(n,s,o) (");
    if (i == 0)
        printf("void");
    for (i = 0; i < id; i++)
    {
        if (i > 0)
            printf(", ");
        printf("__AROS_LHA(a%d)", i + 1);
    }
    printf(") {");
    if (!is_ignored)
        printf(" \\\n    register bt __attribute__((unused)) bn = ({register APTR __r __asm__(\"r12\");__asm__ __volatile__(\"\":\"=r\"(__r):\"0\"(__r));(bt)__r;});");
    printf("\n");
    printf("#define AROS_LH%d%s __AROS_LH%d%s\n", id, is_ignored ? "I" : "", id, is_ignored ? "I" : "");
}

static void aros_ld(int id, int is_ignored)
{
    int i;

    printf("#define __AROS_LD%d%s(t,n,", id, is_ignored ? "I" : "");
    for (i = 0; i < id; i++)
        printf("a%d,", i + 1);
    printf("bt,bn,o,s) \\\n");
    printf("    t AROS_SLIB_ENTRY(n,s,o) (");
    if (i == 0)
        printf("void");
    for (i = 0; i < id; i++)
    {
        if (i > 0)
            printf(", ");
        printf("__AROS_LDA(a%d)", i + 1);
    }
    printf(");\n");
    printf("#define AROS_LD%d%s __AROS_LD%d%s\n", id, is_ignored ? "I" : "", id, is_ignored ? "I" : "");
}

const static char extra[] =
"\n"
"#define __AROS_QUADt(type,name,reg1,reg2) type\n"
"#define __AROS_QUADn(type,name,reg1,reg2) name\n"
"#define __AROS_QUADr(type,name,reg1,reg2) reg1##reg2\n"
"\n"
"#define AROS_LHDOUBLE1(t,n,a1,bt,bn,o,s) \\\n"
"    AROS_LH1(t,n, \\\n"
"             AROS_LHA(__AROS_QUADt(a1), __AROS_QUADn(a1), __AROS_QUADr(a1)), \\\n"
"             bt, bn, o, s \\\n"
"    )\n"
"#define AROS_LHDOUBLE2(t,n,a1,a2,bt,bn,o,s) \\\n"
"    AROS_LH2(t,n, \\\n"
"             AROS_LHA(__AROS_QUADt(a1), __AROS_QUADn(a1), __AROS_QUADr(a1)), \\\n"
"             AROS_LHA(__AROS_QUADt(a2), __AROS_QUADn(a2), __AROS_QUADr(a2)), \\\n"
"             bt, bn, o, s \\\n"
"    )\n"
"#define AROS_LH1DOUBLE1(t,n,a1,a2,bt,bn,o,s) \\\n"
"    AROS_LH2(t,n, \\\n"
"             AROS_LHA(a1), \\\n"
"             AROS_LHA(__AROS_QUADt(a2), __AROS_QUADn(a2), __AROS_QUADr(a2)), \\\n"
"             bt, bn, o, s \\\n"
"    )\n"
"#define AROS_LH2QUAD1(t,n,a1,a2,a3,bt,bn,o,s) \\\n"
"    AROS_LH3(t,n, \\\n"
"             AROS_LHA(a1), \\\n"
"             AROS_LHA(a2), \\\n"
"             AROS_LHA(__AROS_QUADt(a3), __AROS_QUADn(a3), __AROS_QUADr(a3)), \\\n"
"             bt, bn, o, s \\\n"
"    )\n"
"#define AROS_LH3QUAD1(t,n,a1,a2,a3,a4,bt,bn,o,s) \\\n"
"    AROS_LH4(t,n, \\\n"
"             AROS_LHA(a1), \\\n"
"             AROS_LHA(a2), \\\n"
"             AROS_LHA(a3), \\\n"
"             AROS_LHA(__AROS_QUADt(a4), __AROS_QUADn(a4), __AROS_QUADr(a4)), \\\n"
"             bt, bn, o, s \\\n"
"    )\n"
"\n"
"\n"
"#define AROS_LCDOUBLE1(t,n,a1,bt,bn,o,s) \\\n"
"    AROS_LC1(t,n, \\\n"
"             AROS_LCA(__AROS_QUADt(a1), __AROS_QUADn(a1), __AROS_QUADr(a1)), \\\n"
"             bt, bn, o, s \\\n"
"    )\n"
"#define AROS_LCDOUBLE2(t,n,a1,a2,bt,bn,o,s) \\\n"
"    AROS_LC2(t,n, \\\n"
"             AROS_LCA(__AROS_QUADt(a1), __AROS_QUADn(a1), __AROS_QUADr(a1)), \\\n"
"             AROS_LCA(__AROS_QUADt(a2), __AROS_QUADn(a2), __AROS_QUADr(a2)), \\\n"
"             bt, bn, o, s \\\n"
"    )\n"
"#define AROS_LC1DOUBLE1(t,n,a1,a2,bt,bn,o,s) \\\n"
"    AROS_LC2(t,n, \\\n"
"             AROS_LCA(a1), \\\n"
"             AROS_LCA(__AROS_QUADt(a2), __AROS_QUADn(a2), __AROS_QUADr(a2)), \\\n"
"             bt, bn, o, s \\\n"
"    )\n"
"#define AROS_LC2QUAD1(t,n,a1,a2,a3,bt,bn,o,s) \\\n"
"    AROS_LC3(t,n, \\\n"
"             AROS_LCA(a1), \\\n"
"             AROS_LCA(a2), \\\n"
"             AROS_LCA(__AROS_QUADt(a3), __AROS_QUADn(a3), __AROS_QUADr(a3)), \\\n"
"             bt, bn, o, s \\\n"
"    )\n"
"#define AROS_LC3QUAD1(t,n,a1,a2,a3,a4,bt,bn,o,s) \\\n"
"    AROS_LC4(t,n, \\\n"
"             AROS_LCA(a1), \\\n"
"             AROS_LCA(a2), \\\n"
"             AROS_LCA(a3), \\\n"
"             AROS_LCA(__AROS_QUADt(a4), __AROS_QUADn(a4), __AROS_QUADr(a4)), \\\n"
"             bt, bn, o, s \\\n"
"    )\n"
"\n"
"#define AROS_LDDOUBLE1(t,n,a1,bt,bn,o,s) \\\n"
"     AROS_LD1(t,n, \\\n"
"         AROS_LDA(__AROS_QUADt(a1), __AROS_QUADn(a1), __AROS_QUADr(a1)), \\\n"
"         bt, bn, o, s \\\n"
"     )\n"
"#define AROS_LDDOUBLE2(t,n,a1,a2,bt,bn,o,s) \\\n"
"     AROS_LD2(t,n, \\\n"
"         AROS_LDA(__AROS_QUADt(a1), __AROS_QUADn(a1), __AROS_QUADr(a1)), \\\n"
"         AROS_LDA(__AROS_QUADt(a2), __AROS_QUADn(a2), __AROS_QUADr(a2)), \\\n"
"         bt, bn, o, s \\\n"
"     )\n"
"#define AROS_LD1DOUBLE1(t,n,a1,a2,bt,bn,o,s) \\\n"
"     AROS_LD2(t,n, \\\n"
"         AROS_LDA(a1), \\\n"
"         AROS_LDA(__AROS_QUADt(a2), __AROS_QUADn(a2), __AROS_QUADr(a2)), \\\n"
"         bt, bn, o, s \\\n"
"     )\n"
"#define AROS_LD2QUAD1(t,n,a1,a2,a3,bt,bn,o,s) \\\n"
"     AROS_LD3(t,n, \\\n"
"         AROS_LDA(a1), \\\n"
"         AROS_LDA(a2), \\\n"
"         AROS_LDA(__AROS_QUADt(a3), __AROS_QUADn(a3), __AROS_QUADr(a3)), \\\n"
"         bt, bn, o, s \\\n"
"     )\n"
"#define AROS_LD3QUAD1(t,n,a1,a2,a3,a4,bt,bn,o,s) \\\n"
"     AROS_LD4(t,n, \\\n"
"         AROS_LDA(a1), \\\n"
"         AROS_LDA(a2), \\\n"
"         AROS_LDA(a3), \\\n"
"         AROS_LDA(__AROS_QUADt(a4), __AROS_QUADn(a4), __AROS_QUADr(a4)), \\\n"
"         bt, bn, o, s \\\n"
"     )\n"
"\n";

int main(int argc, char **argv)
{
    int i;

    printf("/* AUTOGENERATED by arch/x86_64-all/include/gencall.c */\n");
    printf("\n");
    printf("#ifndef AROS_X86_64_LIBCALL_H\n");
    printf("#define AROS_X86_64_LIBCALL_H\n");
    printf("\n");
    printf("/* Reserver r12 from being used by compiler in compilation unit including \n");
    printf("   directly or indirectly libcall.h */\n");
    printf("register void * __fixedreg __asm__(\"r12\");\n");
    printf("\n");

    printf("#define AROS_LIBCALL_INIT(bn, o) \\\n"
           "    APTR __sto; \\\n"
           "    APTR _bn = (APTR)bn;\\\n"
           "    APTR __func = __AROS_GETVECADDR(_bn, o); \\\n"
           "    __asm__ __volatile__(\"movq %%%%r12, %%0\\n    movq %%1, %%%%r12\" : \"=rm\"(__sto) : \"rm\"(_bn) : \"r12\");\n"
           "\n"
    );

    printf("#define AROS_LIBCALL_EXIT \\\n"
           "    __asm__ __volatile__(\"movq %%0, %%%%r12 \" : : \"rm\"(__sto) : \"r12\"); \\\n"
           "    __ret; \n"
           "\n"
    );

    printf("#define AROS_LIBCALLNR_EXIT \\\n"
           "    __asm__ __volatile__(\"movq %%0, %%%%r12 \" : : \"rm\"(__sto) : \"r12\");\n"
           "\n"
    );

    printf("#define __AROS_CPU_SPECIFIC_LH\n\n");

    for (i = 0; i < GENCALL_MAX; i++)
        aros_lh(i, 0);

    for (i = 0; i < GENCALL_MAX; i++)
        aros_lh(i, 1);

    printf("\n");
    printf("#define __AROS_CPU_SPECIFIC_LC\n\n");

    for (i = 0; i < GENCALL_MAX; i++)
        aros_lc(i, 0);

    for (i = 0; i < GENCALL_MAX; i++)
        aros_lc(i, FLAG_NR);

    for (i = 0; i < GENCALL_MAX; i++)
        aros_call(i, 0);

    for (i = 0; i < GENCALL_MAX; i++)
        aros_call(i, FLAG_NR);

    printf("\n");
    printf("#define __AROS_CPU_SPECIFIC_LD\n\n");

    for (i = 0; i < GENCALL_MAX; i++)
        aros_ld(i, 0);

    for (i = 0; i < GENCALL_MAX; i++)
        aros_ld(i, 1);

    printf("%s\n", extra);

    printf("#endif /* AROS_X86_64_LIBCALL_H */\n");
    return 0;
}
