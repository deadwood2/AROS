/*
    Copyright (C) 2019-2020, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/symbolsets.h>
#include <proto/dos.h>
#include <aros/debug.h>
#include <aros/startup.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

#include "rt_version.h"

/*****************************************************************************/
/* AxRT-side startup                                                         */
/*     Note: code copied from compiler/startup/startup.c                     */

struct DosLibrary *DOSBase __attribute__((weak));

THIS_PROGRAM_HANDLES_SYMBOLSET(PROGRAM_ENTRIES)

extern int main(int argc, char ** argv);
int (*__main_function_ptr)(int argc, char ** argv) __attribute__((__weak__)) = main;

/* if the programmer hasn't defined a symbol with the name __nocommandline
   then the code to handle the commandline will be included from the autoinit.lib
*/
extern int __nocommandline;
asm(".set __importcommandline, __nocommandline");

/* if the programmer hasn't defined a symbol with the name __nostdiowin
   then the code to open a window for stdio will be included from the autoinit.lib
*/
extern int __nostdiowin;
asm(".set __importstdiowin, __nostdiowin");

/* if the programmer hasn't defined a symbol with the name __nowbsupport
   then the code to handle support for programs started from WB will be included from
   the autoinit.lib
*/
extern int __nowbsupport;
asm(".set __importnowbsupport, __nowbsupport");

/* if the programmer hasn't defined a symbol with the name __noinitexitsets
   then the code to handle support for calling the INIT, EXIT symbolset functions
   and the autoopening of libraries is called from the autoinit.lib
*/
extern int __noinitexitsets;
asm(".set __importnoinitexitsets, __noinitexitsets");

extern void __startup_entries_init(void);

__attribute__((visibility("default"))) __attribute__((__weak__))
int __startup_entry(STRPTR argstr, LONG argsize, struct ExecBase *SysBase)
{
    D(bug("Entering __startup_entry(\"%s\", %d, %x)\n", argstr, argsize, SysBase));

    /*
        No one program will be able to do anything useful without the dos.library,
        so we open it here instead of using the automatic opening system
    */
    DOSBase = (struct DosLibrary *)OpenLibrary((CONST_STRPTR)DOSNAME, 0);
    if (!DOSBase) return RETURN_FAIL;

    __argstr  = (char *)argstr;
    __argsize = argsize;
    __startup_error = RETURN_FAIL;

    __startup_entries_init();
    __startup_entries_next();

    CloseLibrary((struct Library *)DOSBase);

    D(bug("Leaving __startup_entry\n"));

    return __startup_error;
} /* entry */

static void __startup_main(struct ExecBase *SysBase)
{
    D(bug("Entering __startup_main\n"));

    /* Invoke the main function. A weak symbol is used as function name so that
       it can be overridden (for *nix stuff, for instance).  */
    __startup_error = (*__main_function_ptr) (__argc, __argv);

    D(bug("Leaving __startup_main\n"));
}

ADD2SET(__startup_main, PROGRAM_ENTRIES, 127);






/*****************************************************************************/
/*  Linux-side startup                                                       */

const char dl_loader[] __attribute__((section(".interp"))) = "/lib64/ld-linux-x86-64.so.2";

int __kick_start(void *, int);
void __set_runtime_env(int);

static int __runtimestartup(int argc, char **argv, char **evnp)
{
    /* This thread is not an AROS Process/Task. Restrictions apply. */

    __set_runtime_env(RT_VER);

    // TODO: error handling

    return __kick_start(__startup_entry, RT_VER);
}

/* Note: this declaration is missing last argument. It is pushed "manually"
   in assembler to control stack operations */
int __libc_start_main(int (*main) (int, char * *, char * *),int argc, char * * ubp_av,
    void (*init) (void), void (*fini) (void), void (*rtld_fini) (void) /*, void (* stack_end)*/);

/* This is custom entry point (see axrt.specs) to the binary. __runtimestartup is
   the "main" on Linux side */
void __attribute__((naked)) _axrt_start(void)
{
    int argc;
    char **argv;
    asm volatile( \
        /* Read argc from stack */      \
        "\tmov (%%rsp), %0\n"           \
        /* Read argv from stack */      \
        "\tmov %%rsp, %1\n"             \
        "\tadd $8, %1\n"                \
        /* Aling stack to 16 bytes */   \
        "\tand $~15, %%rsp\n"           \
        /* Push 8 bytes */              \
        "\tpush %%rax\n"                \
        /* Push last argument to __libc_start_main */   \
        "\tpush %%rsp\n"                \
        : "=r" (argc), "=r" (argv)
    );
    __libc_start_main(__runtimestartup, argc, argv, NULL, NULL, NULL);
    asm volatile( \
        "\thlt\n"
    );
}

/*****************************************************************************/

/* These functions need to be complied into application to properly proxy
   to the shims in runtime. */
int __shims_libc_open64(const char *, int);

__attribute__((visibility("default"))) int open64(const char * pathname, int oflag)
{
    return __shims_libc_open64(pathname, oflag);
}
