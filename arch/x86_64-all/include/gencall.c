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

#define FLAG_NR            (1 << 2)
#define FLAG_RETURN_DOUBLE (1 << 3)

static char *long_registers[] =  {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

static inline const char *nr(int flags)
{
    if (flags & FLAG_NR)
        return "NR";
    else
        return "";
}

static void generate_arg_variables(int id, int flags, char *regs[], int numregs)
{
    for (int i=1; i<=numregs; ++i) {
        if (regs[i-1][0] == 'x') {
            // This is a double
            printf("register double arg%d __asm__(\"%s\") = __AROS_LCAQUAD(a%d); \\\n", i, regs[i-1], i);
        } else {
            printf("register UQUAD arg%d __asm__(\"%s\") = (UQUAD)__AROS_LCA(a%d); \\\n"
                   "if (sizeof(__AROS_LPA(a%d)) <= 4) { \\\n"
                   "    arg%d &= 0xFFFFFFFFL; \\\n"
                   "} \\\n", i, regs[i-1], i, i, i);
        }
    }
    // This generator currently does not handle double args on the stack!
    // Easy to fix if somebody needs it, though. Just add some type array.
    for (int i = 7; i <= id; ++i) {
        printf("UQUAD arg%d = (UQUAD)__AROS_LCA(a%d); \\\n"
               "if (sizeof(__AROS_LPA(a%d)) <= 4) { \\\n"
               "  arg%d &= 0xFFFFFFFFL; \\\n"
               "} \\\n", i, i, i, i);
    }
}

static void generate_asm_body(int id)
{
           // Save r12 so it can be restored on exit
    printf("    \"push %%%%r12\\n\" \\\n"
           // Save r13 so it can be restored on exit
           "    \"push %%%%r13\\n\" \\\n"
           // Load base pointer into r12
           "    \"movq %%[op_base], %%%%r12\\n\" \\\n"
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
           // Restore r13 from stack
           "    \"pop %%%%r13\\n\" \\\n"
           // Restore r12 from stack
           "    \"pop %%%%r12\\n\" \\\n");
}

static void generate_asm_operands(int id, int flags, char *result_register)
{
    // Store the register defined for __result, if applicable
    if (!(flags & FLAG_NR)) {
        printf("    : [op_result] \"=%s\" (__result)", result_register);
        if (id >= 1) {
            // Function arg(s) will follow
            printf(", ");
        }
    } else {
        printf("    : ");
    }
    // The registers of these args may be changed by the callee, so they are read-write.
    if (id >= 1) {
        printf("[op_arg1] \"+r\" (arg1)");
    }
    if (id >= 2) {
        printf(", [op_arg2] \"+r\" (arg2)");
    }
    if (id >= 3) {
        printf(", [op_arg3] \"+r\" (arg3)");
    }
    if (id >= 4) {
        printf(", [op_arg4] \"+r\" (arg4)");
    }
    if (id >= 5) {
        printf(", [op_arg5] \"+r\" (arg5)");
    }
    if (id >= 6) {
        printf(", [op_arg6] \"+r\" (arg6)");
    }
    printf("\\\n");
    // Define input operands
    printf("    : [op_base] \"m\" (baseptr), [op_a] \"mr\" (vec)");
    for (int i = 7; i <= id; ++i) {
        printf(", [op_arg%d] \"m\" (arg%d)", i, i);
    }
    printf(" \\\n");
}

static void generate_clobber_list(int stdregs, int xregs, int flags)
{
    // Define clobber list. The scratch registers are either here or operands.
    printf("    : \"cc\", \"memory\", \"r10\", \"r11\", \"r12\", \"r13\"");
    if (flags & FLAG_NR || flags & FLAG_RETURN_DOUBLE) {
        // In this case rax is not an operand, so it must be on the clobber list.
        printf(", \"rax\"");
    }
    if (stdregs < 6) {
        printf(", \"r9\"");
    }
    if (stdregs < 5) {
        printf(", \"r8\"");
    }
    if (stdregs < 4) {
        printf(", \"rcx\"");
    }
    if (stdregs < 3) {
        printf(", \"rdx\"");
    }
    if (stdregs < 2) {
        printf(", \"rsi\"");
    }
    if (stdregs < 1) {
        printf(", \"rdi\"");
    }
    if (xregs < 6) {
        printf(", \"xmm5\"");
    }
    if (xregs < 5) {
        printf(", \"xmm4\"");
    }
    if (xregs < 4) {
        printf(", \"xmm3\"");
    }
    if (xregs < 3) {
        printf(", \"xmm2\"");
    }
    if (xregs < 2) {
        printf(", \"xmm1\"");
    }
    if (xregs < 1 && !(flags & FLAG_RETURN_DOUBLE)) {
        printf(", \"xmm0\"");
    }
}

static void aros_lc(int id, int flags)
{
    int numregs = id > 6 ? 6 : id;
    printf("#define __AROS_LC%d%s(t,a,", id, nr(flags));
    for (int i = 1; i <= id; i++)
        printf("a%d,", i);
    printf("bt,bn,o,s) \\\n"
           "({ \\\n"
    );

    if (!(flags & FLAG_NR)) {
        printf("  t __result; \\\n");
    }
    printf("  bt baseptr = (bt) bn; \\\n");
    printf("  APTR vec = (APTR)__AROS_GETVECADDR(baseptr, o); \\\n");
    // The return value should be in different registers depending on return type
    printf("  if (TYPE_IS_DOUBLE(t)) { \\\n");
    // Generate code for double return type
    generate_arg_variables(id, flags, long_registers, numregs);
    printf("__asm__ __volatile__( \\\n");
    generate_asm_body(id);
    generate_asm_operands(id, flags, "Yz");
    flags |= FLAG_RETURN_DOUBLE;
    generate_clobber_list(numregs, 0, flags);
    printf("); \\\n");
    printf("  } else { \\\n");
    // Generate code for other return type
    generate_arg_variables(id, flags, long_registers, numregs);
    printf("__asm__ __volatile__( \\\n");
    generate_asm_body(id);
    generate_asm_operands(id, flags, "a");
    flags &= ~FLAG_RETURN_DOUBLE;
    generate_clobber_list(numregs, 0, flags);
    printf("); \\\n");
    printf("  } \\\n");
    if (!(flags & FLAG_NR)) {
        // __result is the value to return
        printf("  __result; \\\n");
    }
    printf("})\n");

    printf("#define AROS_LC%d%s __AROS_LC%d%s\n\n", id, nr(flags), id, nr(flags));
}

static void aros_call(int id, int flags)
{
    int numregs = id > 6 ? 6 : id;
    printf("#define __AROS_CALL%d%s(t,a,", id, nr(flags));
    for (int i = 1; i <= id; i++)
        printf("a%d,", i);
    printf("bt,bn) \\\n"
           "({ \\\n");
    if (!(flags & FLAG_NR)) {
        printf("  t __result; \\\n");
    }
    printf("  APTR vec = a; \\\n");
    printf("  bt baseptr = bn; \\\n");
    // The return value should be in different registers depending on return type
    printf("  if (TYPE_IS_DOUBLE(t)) { \\\n");
    // Generate code for double return type
    generate_arg_variables(id, flags, long_registers, numregs);
    printf("__asm__ __volatile__( \\\n");
    generate_asm_body(id);
    generate_asm_operands(id, flags, "Yz");
    flags |= FLAG_RETURN_DOUBLE;
    generate_clobber_list(numregs, 0, flags);
    printf("); \\\n");
    printf("  } else { \\\n");
    // Generate code for other return type
    generate_arg_variables(id, flags, long_registers, numregs);
    printf("__asm__ __volatile__( \\\n");
    generate_asm_body(id);
    generate_asm_operands(id, flags, "a");
    flags &= ~FLAG_RETURN_DOUBLE;
    generate_clobber_list(numregs, 0, flags);
    printf("); \\\n");
    printf("  } \\\n");
    if (!(flags & FLAG_NR)) {
        // __result is the value to return
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

const static char extra_lh[] =
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
"\n";

static void generate_extra_lc(void)
{
    int id = 1;
    int flags = 0;
    int numregs = 1;
    char *arg_registers[] = {"xmm0"};
    printf(
"#define AROS_LCQUAD1(t,n,a1,bt,bn,o,s) \\\n"
"({ \\\n");
    printf("  t __result; \\\n");
    printf("  bt baseptr = (bt) bn; \\\n");
    printf("  APTR vec = (APTR)__AROS_GETVECADDR(baseptr, o); \\\n");
    // The return value should be in different registers depending on return type
    printf("  if (TYPE_IS_DOUBLE(t)) { \\\n");
    // Generate code for double return type
    generate_arg_variables(id, flags, arg_registers, numregs);
    printf("__asm__ __volatile__( \\\n");
    generate_asm_body(id);
    generate_asm_operands(id, flags, "Yz");
    flags |= FLAG_RETURN_DOUBLE;
    generate_clobber_list(0, numregs, flags);
    printf("); \\\n");
    printf("  } else { \\\n");
    // Generate code for other return type
    generate_arg_variables(id, flags, arg_registers, numregs);
    printf("__asm__ __volatile__( \\\n");
    generate_asm_body(id);
    generate_asm_operands(id, flags, "a");
    flags &= ~FLAG_RETURN_DOUBLE;
    generate_clobber_list(0, numregs, flags);
    printf("); \\\n");
    printf("  } \\\n");
    if (!(flags & FLAG_NR)) {
        // __result is the value to return
        printf("  __result; \\\n");
    }
    printf("})\n\n");

    id = 2;
    flags = 0;
    numregs = 2;
    char *arg_registers2[] = {"xmm0", "xmm1"};
    printf(
"#define AROS_LCQUAD2(t,n,a1,a2,bt,bn,o,s) \\\n"
"({ \\\n");
    printf("  t __result; \\\n");
    printf("  bt baseptr = (bt) bn; \\\n");
    printf("  APTR vec = (APTR)__AROS_GETVECADDR(baseptr, o); \\\n");
    // The return value should be in different registers depending on return type
    printf("  if (TYPE_IS_DOUBLE(t)) { \\\n");
    // Generate code for double return type
    generate_arg_variables(id, flags, arg_registers2, numregs);
    printf("__asm__ __volatile__( \\\n");
    generate_asm_body(id);
    generate_asm_operands(id, flags, "Yz");
    flags |= FLAG_RETURN_DOUBLE;
    generate_clobber_list(0, numregs, flags);
    printf("); \\\n");
    printf("  } else { \\\n");
    // Generate code for other return type
    generate_arg_variables(id, flags, arg_registers2, numregs);
    printf("__asm__ __volatile__( \\\n");
    generate_asm_body(id);
    generate_asm_operands(id, flags, "a");
    flags &= ~FLAG_RETURN_DOUBLE;
    generate_clobber_list(0, numregs, flags);
    printf("); \\\n");
    printf("  } \\\n");
    if (!(flags & FLAG_NR)) {
        // __result is the value to return
        printf("  __result; \\\n");
    }
    printf("})\n\n");
    printf(
"#define AROS_LC1QUAD1(t,n,a1,a2,bt,bn,o,s) \\\n"
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
"\n");
}

const static char extra_ld[] =
"#define AROS_LDQUAD1(t,n,a1,bt,bn,o,s) \\\n"
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

    printf("#if defined(__cplusplus)\n"
           "#include <type_traits>\n"
           "#define TYPE_IS_DOUBLE(t) std::is_same<t, double>::value\n"
           "#else\n"
           "#define TYPE_IS_DOUBLE(t) __builtin_types_compatible_p(t, double)\n"
           "#endif\n\n");

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

    printf("%s\n", extra_lh);

    generate_extra_lc();

    printf("%s\n", extra_ld);

    printf("#endif /* AROS_X86_64_LIBCALL_H */\n");
    return 0;
}
