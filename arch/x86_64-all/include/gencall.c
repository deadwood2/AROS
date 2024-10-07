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

static void aros_lc(int id, int flags)
{
    printf("#define __AROS_LC%d%s(t,a,", id, nr(flags));
    for (int i = 0; i < id; i++)
        printf("a%d,", i + 1);
    printf("bt,bn,o,s) \\\n"
           "({ \\\n"
    );

    if (!(flags & FLAG_NR)) {
        printf("  t __result; \\\n");
    }
    printf("  register bt baseptr __asm__(\"r12\") = (bt) bn; \\\n");
    printf("  APTR vec = (APTR)__AROS_GETVECADDR(baseptr, o); \\\n");
    // Extract the value from all argument triplets
    if (id >= 1) {
        printf("  register UQUAD arg1 __asm__(\"rdi\") = (UQUAD)__AROS_LCA(a1); \\\n");
    }
    if (id >= 2) {
        printf("  register UQUAD arg2 __asm__(\"rsi\") = (UQUAD)__AROS_LCA(a2); \\\n");
    }
    if (id >= 3) {
        printf("  register UQUAD arg3 __asm__(\"rdx\") = (UQUAD)__AROS_LCA(a3); \\\n");
    }
    if (id >= 4) {
        printf("  register UQUAD arg4 __asm__(\"rcx\") = (UQUAD)__AROS_LCA(a4); \\\n");
    }
    if (id >= 5) {
        printf("  register UQUAD arg5 __asm__(\"r8\") = (UQUAD)__AROS_LCA(a5); \\\n");
    }
    if (id >= 6) {
        printf("  register UQUAD arg6 __asm__(\"r9\") = (UQUAD)__AROS_LCA(a6); \\\n");
    }
    if (id >= 7) {
        printf("  UQUAD arg7 = (UQUAD)__AROS_LCA(a7); \\\n");
    }
    if (id >= 8) {
        printf("  UQUAD arg8 = (UQUAD)__AROS_LCA(a8); \\\n");
    }
    if (id >= 9) {
        printf("  UQUAD arg9 = (UQUAD)__AROS_LCA(a9); \\\n");
    }
    if (id >= 10) {
        printf("  UQUAD arg10 = (UQUAD)__AROS_LCA(a10); \\\n");
    }
    if (id >= 11) {
        printf("  UQUAD arg11 = (UQUAD)__AROS_LCA(a11); \\\n");
    }
    if (id >= 12) {
        printf("  UQUAD arg12 = (UQUAD)__AROS_LCA(a12); \\\n");
    }
    if (id >= 13) {
        printf("  UQUAD arg13 = (UQUAD)__AROS_LCA(a13); \\\n");
    }
    printf("  __asm__ __volatile__( \\\n"
           // Push r13 to stack
           "    \"push %%%%r13\\n\" \\\n"
           // Copy rsp to r13, we need to restore this stack position after the call
           "    \"movq %%%%rsp, %%%%r13\\n\" \\\n");
    // The stack needs to be aligned on 16 bytes before the call op,
    // but the callee also expects the stack args to have been pushed
    // directly before the call op where it reliably can find them.
    // That means we need to make alignment adjustments *before* pushing
    // args so the end result is 16 bytes aligned.
    // First clear the lowest four bits so we have the stack aligned on 16 bytes.
    printf("    \"andq $-16, %%%%rsp\\n\"  \\\n");
    if (id > 6) {
        if ((id - 6) % 2 != 0) {
            // Odd number of stack args -> need 8 more bytes to get correct alignment.
            printf("    \"subq $8, %%%%rsp\\n\" \\\n");
        }
    }
    // Push seventh and further args to stack, in reverse order.
    for (int i = id; i > 6; --i) {
        printf("    \"push %%[op_arg%d]\\n\" \\\n", i);
    }
    // Call LVO address
    printf("    \"call *%%[op_a]\\n\"  \\\n");
    // Copy r13 to rsp (restoring stack)
    printf("    \"movq %%%%r13, %%%%rsp\\n\" \\\n"
           // Pop r13 from stack
           "    \"pop %%%%r13\\n\" \\\n");
    // Store rax in __result, if applicable
    if (!(flags & FLAG_NR)) {
        printf("    : [op_result] \"=a\" (__result) \\\n");
    } else {
        printf("    : \\\n");
    }
    // Define input operands
    printf("    : [op_base] \"r\" (baseptr), [op_a] \"mr\" (vec)");
    if (id >= 1) {
        printf(", [op_arg1] \"D\" (arg1)");
    }
    if (id >= 2) {
        printf(", [op_arg2] \"S\" (arg2)");
    }
    if (id >= 3) {
        printf(", [op_arg3] \"d\" (arg3)");
    }
    if (id >= 4) {
        printf(", [op_arg4] \"c\" (arg4)");
    }
    if (id >= 5) {
        printf(", [op_arg5] \"r\" (arg5)");
    }
    if (id >= 6) {
        printf(", [op_arg6] \"r\" (arg6)");
    }
    for (int i = 7; i <= id; ++i) {
        printf(", [op_arg%d] \"m\" (arg%d)", i, i);
    }
    printf(" \\\n");
    // Define clobber list. It is inversely proportional to the number of args :-)
    printf("    : \"cc\", \"r10\", \"r11\", \"r13\"");
    if (id < 6) {
        printf(", \"r9\"");
    }
    if (id < 5) {
        printf(", \"r8\"");
    }
    if (id < 4) {
        printf(", \"rcx\"");
    }
    if (id < 3) {
        printf(", \"rdx\"");
    }
    if (id < 2) {
        printf(", \"rsi\"");
    }
    if (id < 1) {
        printf(", \"rdi\"");
    }
    printf("  ); \\\n");
    if (!(flags & FLAG_NR)) {
        // Return __result, which should have the return value from %rax
        printf("  __result; \\\n");
    }
    printf("})\n");

    printf("#define AROS_LC%d%s __AROS_LC%d%s\n", id, nr(flags), id, nr(flags));
}

static void aros_call(int id, int flags)
{
    printf("#define __AROS_CALL%d%s(t,a,", id, nr(flags));
    for (int i = 0; i < id; ++i)
        printf("a%d,", i + 1);
    printf("bt,bn) \\\n"
           "({ \\\n");
    if (!(flags & FLAG_NR)) {
        printf("  t __result; \\\n");
    }
    printf("  APTR vec = a; \\\n");
    printf("  register bt baseptr __asm__(\"r12\") = bn; \\\n");
    // Extract the value from all argument triplets
    if (id >= 1) {
        printf("  register UQUAD arg1 __asm__(\"rdi\") = (UQUAD)__AROS_LCA(a1); \\\n");
    }
    if (id >= 2) {
        printf("  register UQUAD arg2 __asm__(\"rsi\") = (UQUAD)__AROS_LCA(a2); \\\n");
    }
    if (id >= 3) {
        printf("  register UQUAD arg3 __asm__(\"rdx\") = (UQUAD)__AROS_LCA(a3); \\\n");
    }
    if (id >= 4) {
        printf("  register UQUAD arg4 __asm__(\"rcx\") = (UQUAD)__AROS_LCA(a4); \\\n");
    }
    if (id >= 5) {
        printf("  register UQUAD arg5 __asm__(\"r8\") = (UQUAD)__AROS_LCA(a5); \\\n");
    }
    if (id >= 6) {
        printf("  register UQUAD arg6 __asm__(\"r9\") = (UQUAD)__AROS_LCA(a6); \\\n");
    }
    if (id >= 7) {
        printf("  UQUAD arg7 = (UQUAD)__AROS_LCA(a7); \\\n");
    }
    if (id >= 8) {
        printf("  UQUAD arg8 = (UQUAD)__AROS_LCA(a8); \\\n");
    }
    if (id >= 9) {
        printf("  UQUAD arg9 = (UQUAD)__AROS_LCA(a9); \\\n");
    }
    if (id >= 10) {
        printf("  UQUAD arg10 = (UQUAD)__AROS_LCA(a10); \\\n");
    }
    if (id >= 11) {
        printf("  UQUAD arg11 = (UQUAD)__AROS_LCA(a11); \\\n");
    }
    if (id >= 12) {
        printf("  UQUAD arg12 = (UQUAD)__AROS_LCA(a12); \\\n");
    }
    if (id >= 13) {
        printf("  UQUAD arg13 = (UQUAD)__AROS_LCA(a13); \\\n");
    }
    printf("  __asm__ __volatile__( \\\n"
           // Push r13 to stack
           "    \"push %%%%r13\\n\" \\\n"
           // Copy rsp to r13, we need to restore this stack position after the call
           "    \"movq %%%%rsp, %%%%r13\\n\" \\\n");
    // The stack needs to be aligned on 16 bytes before the call op,
    // but the callee also expects the stack args to have been pushed
    // directly before the call op where it reliably can find them.
    // That means we need to make alignment adjustments *before* pushing
    // args so the end result is 16 bytes aligned.
    // First clear the lowest four bits so we have the stack aligned on 16 bytes.
    printf("    \"andq $-16, %%%%rsp\\n\"  \\\n");
    if (id > 6) {
        if ((id - 6) % 2 != 0) {
            // Odd number of stack args -> need 8 more bytes to get correct alignment.
            printf("    \"subq $8, %%%%rsp\\n\" \\\n");
        }
    }
    // Push seventh and further args to stack, in reverse order.
    for (int i = id; i > 6; --i) {
        printf("    \"push %%[op_arg%d]\\n\" \\\n", i);
    }
    // Call LVO address
    printf("    \"call *%%[op_a]\\n\"  \\\n");
    // Copy r13 to rsp (restoring stack)
    printf("    \"movq %%%%r13, %%%%rsp\\n\" \\\n"
           // Pop r13 from stack
           "    \"pop %%%%r13\\n\" \\\n");
    // Store rax in __result, if applicable
    if (!(flags & FLAG_NR)) {
        printf("    : [op_result] \"=a\" (__result) \\\n");
    } else {
        printf("    : \\\n");
    }
    // Define input operands
    printf("    : [op_base] \"r\" (baseptr), [op_a] \"mr\" (vec)");
    if (id >= 1) {
        printf(", [op_arg1] \"D\" (arg1)");
    }
    if (id >= 2) {
        printf(", [op_arg2] \"S\" (arg2)");
    }
    if (id >= 3) {
        printf(", [op_arg3] \"d\" (arg3)");
    }
    if (id >= 4) {
        printf(", [op_arg4] \"c\" (arg4)");
    }
    if (id >= 5) {
        printf(", [op_arg5] \"r\" (arg5)");
    }
    if (id >= 6) {
        printf(", [op_arg6] \"r\" (arg6)");
    }
    for (int i = 7; i <= id; ++i) {
        printf(", [op_arg%d] \"m\" (arg%d)", i, i);
    }
    printf(" \\\n");
    // Define clobber list. It is inversely proportional to the number of args :-)
    printf("    : \"cc\", \"r10\", \"r11\", \"r13\" \\\n");
    if (id < 6) {
        printf(", \"r9\"");
    }
    if (id < 5) {
        printf(", \"r8\"");
    }
    if (id < 4) {
        printf(", \"rcx\"");
    }
    if (id < 3) {
        printf(", \"rdx\"");
    }
    if (id < 2) {
        printf(", \"rsi\"");
    }
    if (id < 1) {
        printf(", \"rdi\"");
    }
    printf("  ); \\\n");
    if (!(flags & FLAG_NR)) {
        // Return __result, which should have the return value from %rax
        printf("  __result; \\\n");
    }
    printf("})\n");

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
