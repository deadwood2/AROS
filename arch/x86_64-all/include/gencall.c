/*
    Copyright (C) 1995-2021, The AROS Development Team. All rights reserved.
*/
/*
 * This program generates the libcall.h macroset for gcc
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GENCALL_MAX     (13 + 1)        /* Max number of arguments */

#define FLAG_NR            (1 << 2)
#define FLAG_RETURN_DOUBLE (1 << 3)

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
    printf("#define __AROS_LC%d%s(t,a,", id, nr(flags));
    for (int i = 1; i <= id; i++)
        printf("a%d,", i);
    printf("bt,bn,o,s) \\\n"
           "({ \\\n"
           "    AROS_LIBCALL_INIT(bn, o) \\\n");
    aros_call_internal(id, flags);
    printf("#define AROS_LC%d%s __AROS_LC%d%s\n\n", id, nr(flags), id, nr(flags));
}

static void aros_call(int id, int flags)
{
    printf("#define __AROS_CALL%d%s(t,a,", id, nr(flags));
    for (int i = 1; i <= id; i++)
        printf("a%d,", i);
    printf("bt,bn) \\\n"
           "({ \\\n"
           "    APTR __sto; \\\n"
           "    bt _bn = (bt)bn;\\\n"
           "    APTR __func = (APTR)a; \\\n"
           "    __asm__ __volatile__(\"movq %%%%r12, %%0\\n    movq %%1, %%%%r12\" : \"=&rm\"(__sto) : \"rm\"(_bn) : \"r12\"); \\\n"
    );

    aros_call_internal(id, flags);
    printf("#define AROS_CALL%d%s __AROS_CALL%d%s\n\n", id, nr(flags), id, nr(flags));
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

const static char extra_lc[] =
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
"\n";
    
const static char extra_ld[] =
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

    printf("#define AROS_LIBCALL_INIT(bn, o) \\\n"
           "    APTR __sto; \\\n"
           "    APTR _bn = (APTR)bn;\\\n"
           "    APTR __func = __AROS_GETVECADDR(_bn, o); \\\n"
           "    __asm__ __volatile__(\"movq %%%%r12, %%0\\n    movq %%1, %%%%r12\" : \"=&rm\"(__sto) : \"rm\"(_bn) : \"r12\");\n"
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

    printf("%s\n", extra_lc);

    printf("%s\n", extra_ld);

    printf("#endif /* AROS_X86_64_LIBCALL_H */\n");
    return 0;
}
