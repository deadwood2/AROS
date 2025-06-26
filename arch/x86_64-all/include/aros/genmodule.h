#ifndef AROS_X86_64_GENMODULE_H
#define AROS_X86_64_GENMODULE_H

/*
    Copyright © 1995-2025, The AROS Development Team. All rights reserved.
    $Id$

    Desc: genmodule specific definitions for x86_64
    Lang: english
*/

#include <exec/execbase.h>

/* Macros for generating library stub functions and aliases for stack libcalls. */

/* Macro: AROS_LIBFUNCNOBASESTUB(functionname, libbasename, lvo)
   This macro will generate code for a stub function for
   the function 'functionname' of library with libbase
   'libbasename' and 'lvo' number of the function in the
   vector table. lvo has to be a constant value (not a variable).
   Library base is not passed to library.

   Internals: a dummy function is used that will generate some
   unused junk code but otherwise we can't pass input arguments
   to the asm statement
*/
#define __AROS_GM_LIBFUNCNOBASESTUB(fname, libbasename, lvo) \
    void __ ## fname ## _ ## libbasename ## _wrapper(void) \
    { \
        asm volatile( \
            ".weak " #fname "\n" \
            "\tnop\n"               \
            "\tnop\n"               \
            "\tnop\n"               \
            #fname " :\n" \
            "\tmovabsq $" #libbasename ", %%r11\n" \
            "\tmovq (%%r11),%%r11\n" \
            "\tjmp  *%c0(%%r11)\n" \
            : : "i" ((-lvo)*LIB_VECTSIZE) \
        ); \
    }
#define AROS_GM_LIBFUNCNOBASESTUB(fname, libbasename, lvo) \
    __AROS_GM_LIBFUNCNOBASESTUB(fname, libbasename, lvo)

/* Macro: AROS_LIBFUNCVARARGSSTUB(functionname, libbasename, lvo)
   This macro will generate code for a stub function for
   the varargs function 'functionname' of library with libbase
   'libbasename' and 'lvo' number of the function in the
   vector table. lvo has to be a constant value (not a variable)

   Internals: a dummy function is used that will generate some
   unused junk code but otherwise we can't pass input arguments
   to the asm statement
*/
#define __AROS_GM_LIBFUNCVARARGSSTUB(fname, libbasename, lvo) \
    void __ ## fname ## _ ## libbasename ## _wrapper(void) \
    { \
        asm volatile( \
            ".weak " #fname "\n"    \
            "\tnop\n"               \
            "\tnop\n"               \
            "\tnop\n"               \
            #fname " :\n"           \
            "\tpushq %%rbp\n"       \
            "\tmov %%rsp,%%rbp\n"   \
            "\tmov %%rsp,%%r11\n"   \
            "\taddq $16,%%r11\n"    /* r11 = addres of last stack entry on previous frame */    \
            "\n"                    \
            "\tpushq %%r12\n"       /* backup current r12 value */                              \
            "\tpushq %%r13\n"       /* backup current r13 value */                              \
            "\n"                    \
            "\tmov (%%rbp),%%r13\n" \
            "\tsub %%r11,%%r13\n"   /* r13 = size of previous stack frame, which will be */     \
            "\n"                    /* wrong if there is no frame pointer in rbp */             \
            "\tcmp $0,%%r13\n"      \
            "\tje 1f\n"             /*   0 - treat as correct and no copy need */               \
            "\tjl 3f\n"             /* < 0 - treat as wrong, use fixed size */                  \
            "\tcmp $128,%%r13\n"    /* > 0 - check if not too big and use fixed size if yes */  \
            "\tjg 3f\n"             \
            "\tjmp 2f\n"            \
            "3:\n"                  \
            "\tmovabsq $__" #fname "_" #libbasename "_stack_varargs_cnt,%%r13\n" /* fixed size */  \
            "\tmov (%%r13),%%r13d\n"\
            "\tshl $3,%%r13d\n"     \
            "2:\n"                  \
            "\tsubq %%r13,%%rsp\n"  /* make space on stack for copy */                          \
            "\tpushq %%rcx\n"       \
            "\tpushq %%rdi\n"       \
            "\tpushq %%rsi\n"       \
            "\tmovq %%r13,%%rcx\n"  \
            "\tshrq $3,%%rcx\n"     \
            "\tmov %%r11,%%rsi\n"   \
            "\tmov %%rsp,%%rdi\n"   \
            "\taddq $24,%%rdi\n"    /* because of push rcx, rdi, rsi */                         \
            "\trep movsq\n"         /* make copy */                                             \
            "\tpopq %%rsi\n"        \
            "\tpopq %%rdi\n"        \
            "\tpopq %%rcx\n"        \
            "1:\n"                  \
            "\tmovabsq $" #libbasename ", %%r11\n" /* load library base */                      \
            "\tmovq (%%r11),%%r11\n"\
            "\n"                    \
            "\tmovq %%r11,%%r12\n"  \
            "\n"                    \
            "\tcallq *%c0(%%r11)\n" \
            "\taddq %%r13,%%rsp\n"  /* remove stack copy space */                               \
            "\tpopq %%r13\n"        /* restore r13 value */                                     \
            "\tpopq %%r12\n"        /* restore r12 value */                                     \
            "\tpop %%rbp\n"         \
            "\tret\n"               /* return to caller frame */                                \
            "\tnop\n"               \
            "\tnop\n"               \
            "\tnop\n"               \
            : : "i" ((-lvo)*LIB_VECTSIZE) \
        ); \
    }

#define AROS_GM_LIBFUNCVARARGSSTUB(fname, libbasename, lvo) \
    __AROS_GM_LIBFUNCVARARGSSTUB(fname, libbasename, lvo)

/* Macro: AROS_GM_RELLIBFUNCNOBASESTUB(functionname, libbasename, lvo)
   Same as AROS_GM_LIBFUNCNOBASESTUB but finds libbase at an offset in
   the current libbase
*/
#define __AROS_GM_RELLIBFUNCNOBASESTUB(fname, libbasename, lvo) \
    void __ ## fname ## _ ## libbasename ## _relwrapper(void) \
    { \
        asm volatile( \
            ".weak " #fname "\n" \
            "\tnop\n"               \
            "\tnop\n"               \
            "\tnop\n"               \
            "\t" #fname " :\n"   \
            "\tpushq %%rax\n"     \
            "\tpushq %%rdi\n"     \
            "\tpushq %%rsi\n"     \
            "\tpushq %%rdx\n"     \
            "\tpushq %%rcx\n"     \
            "\tpushq %%r8\n"      \
            "\tpushq %%r9\n"      \
            "\tmovabsq $__aros_getoffsettable,%%r11\n" \
            "\tcall *%%r11\n" \
            "\taddq __aros_rellib_offset_" #libbasename "(%%rip), %%rax\n" \
            "\tmovq (%%rax),%%r11\n" \
            "\tpopq %%r9\n"      \
            "\tpopq %%r8\n"      \
            "\tpopq %%rcx\n"     \
            "\tpopq %%rdx\n"     \
            "\tpopq %%rsi\n"     \
            "\tpopq %%rdi\n"     \
            "\tpopq %%rax\n"     \
            "\tjmp  *%c0(%%r11)\n" \
            : : "i" ((-lvo)*LIB_VECTSIZE) \
        ); \
    }
#define AROS_GM_RELLIBFUNCNOBASESTUB(fname, libbasename, lvo) \
    __AROS_GM_RELLIBFUNCNOBASESTUB(fname, libbasename, lvo)

/* Macro: AROS_GM_RELLIBFUNCVARARGSSTUB(functionname, libbasename, lvo)
   Same as AROS_GM_LIBFUNCVARARGSSTUB but finds libbase at an offset in
   the current libbase
*/

#define __AROS_GM_RELLIBFUNCVARARGSSTUB(fname, libbasename, lvo) \
    void __ ## fname ## _ ## libbasename ## _wrapper(void) \
    { \
        asm volatile( \
            ".weak " #fname "\n"    \
            "\tnop\n"               \
            "\tnop\n"               \
            "\tnop\n"               \
            #fname " :\n"           \
            "\tpushq %%rbp\n"       \
            "\tmov %%rsp,%%rbp\n"   \
            "\tmov %%rsp,%%r11\n"   \
            "\taddq $16,%%r11\n"    /* r11 = addres of last stack entry on previous frame */    \
            "\n"                    \
            "\tpushq %%r12\n"       /* backup current r12 value */                              \
            "\tpushq %%r13\n"       /* backup current r13 value */                              \
            "\n"                    \
            "\tmov (%%rbp),%%r13\n" \
            "\tsub %%r11,%%r13\n"   /* r13 = size of previous stack frame, which will be */     \
            "\n"                    /* wrong if there is no frame pointer in rbp */             \
            "\tcmp $0,%%r13\n"      \
            "\tje 1f\n"             /*   0 - treat as correct and no copy need */               \
            "\tjl 3f\n"             /* < 0 - treat as wrong, use fixed size */                  \
            "\tcmp $128,%%r13\n"    /* > 0 - check if not too big and use fixed size if yes */  \
            "\tjg 3f\n"             \
            "\tjmp 2f\n"            \
            "3:\n"                  \
            "\tmovabsq $__" #fname "_" #libbasename "_stack_varargs_cnt,%%r13\n" /* fixed size */  \
            "\tmov (%%r13),%%r13d\n"\
            "\tshl $3,%%r13d\n"     \
            "2:\n"                  \
            "\tsubq %%r13,%%rsp\n"  /* make space on stack for copy */                          \
            "\tpushq %%rcx\n"       \
            "\tpushq %%rdi\n"       \
            "\tpushq %%rsi\n"       \
            "\tmovq %%r13,%%rcx\n"  \
            "\tshrq $3,%%rcx\n"     \
            "\tmov %%r11,%%rsi\n"   \
            "\tmov %%rsp,%%rdi\n"   \
            "\taddq $24,%%rdi\n"    /* because of push rcx, rdi, rsi */                         \
            "\trep movsq\n"         /* make copy */                                             \
            "\tpopq %%rsi\n"        \
            "\tpopq %%rdi\n"        \
            "\tpopq %%rcx\n"        \
            "1:\n"                  \
            "\tpushq %%rax\n"       \
            "\tpushq %%rdi\n"       \
            "\tpushq %%rsi\n"       \
            "\tpushq %%rdx\n"       \
            "\tpushq %%rcx\n"       \
            "\tpushq %%r8\n"        \
            "\tpushq %%r9\n"        \
            "\tmovabsq $__aros_getoffsettable,%%r11\n"                                          \
            "\tcall *%%r11\n"       \
            "\taddq __aros_rellib_offset_" #libbasename "(%%rip), %%rax\n"                      \
            "\tmovq (%%rax),%%r11\n" /* load library base */                                    \
            "\tpopq %%r9\n"         \
            "\tpopq %%r8\n"         \
            "\tpopq %%rcx\n"        \
            "\tpopq %%rdx\n"        \
            "\tpopq %%rsi\n"        \
            "\tpopq %%rdi\n"        \
            "\tpopq %%rax\n"        \
            "\n"                    \
            "\tmovq %%r11,%%r12\n"  \
            "\n"                    \
            "\tcallq *%c0(%%r11)\n" \
            "\taddq %%r13,%%rsp\n"  /* remove stack copy space */                               \
            "\tpopq %%r13\n"        /* restore r13 value */                                     \
            "\tpopq %%r12\n"        /* restore r12 value */                                     \
            "\tpop %%rbp\n"         \
            "\tret\n"               /* return to caller frame */                                \
            "\tnop\n"               \
            "\tnop\n"               \
            "\tnop\n"               \
            : : "i" ((-lvo)*LIB_VECTSIZE) \
        ); \
    }

#define AROS_GM_RELLIBFUNCVARARGSSTUB(fname, libbasename, lvo) \
    __AROS_GM_RELLIBFUNCVARARGSSTUB(fname, libbasename, lvo)

/* Macro: AROS_GM_LIBFUNCALIAS(functionname, alias)
   This macro will generate an alias 'alias' for function
   'functionname'
*/
#define __AROS_GM_LIBFUNCALIAS(fname, alias) \
    asm(".weak " #alias "\n" \
        "\t.set " #alias "," #fname \
    );
#define AROS_GM_LIBFUNCALIAS(fname, alias) \
    __AROS_GM_LIBFUNCALIAS(fname, alias)

/******************* Library Side Thunks ******************/

#define __GM_STRINGIZE(x) #x

/* Macro: AROS_GM_STACKALIAS(functionname, libbasename, lvo)
   This macro will generate an alias 'alias' for function
   'functionname'
*/

#if defined(__clang__)
  #define AROS_GM_STACKALIAS(fn, lib, lvo)
#else
 #define __AROS_GM_STACKALIAS(fname, alias) \
    void alias(void); \
    asm(".weak " __GM_STRINGIZE(alias) "\n" \
        "\t.set " __GM_STRINGIZE(alias) "," #fname \
    );
 #define AROS_GM_STACKALIAS(fname, libbasename, lvo) \
    __AROS_GM_STACKALIAS(fname, AROS_SLIB_ENTRY(fname, libbasename, lvo))
#endif
/* Macro: AROS_GM_GETOFFSETTABLE
   This macro will retrieve library base from within library
*/
#define AROS_GM_GETOFFSETTABLE \
        return ({register APTR __r asm("r12");asm volatile("":"=r"(__r):"0"(__r));(char *)__r;});

/* Macro: AROS_GM_SETOFFSETTABLE
   This macro will set library base from within library
*/
#define AROS_GM_SETOFFSETTABLE \
        asm volatile("movq %0, %%r12" : : "rm"(base) : "r12"); \
        return TRUE;

#endif /* AROS_X86_64_GENMODULE_H */
