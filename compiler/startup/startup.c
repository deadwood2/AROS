
/*
    Copyright © 1995-2020, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Common startup code
    Lang: english
*/
#define DEBUG 0

#include <dos/dos.h>
#include <exec/memory.h>
#include <exec/tasks.h>
#include <workbench/startup.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <aros/asmcall.h>
#include <aros/debug.h>
#include <aros/symbolsets.h>
#include <aros/startup.h>

struct DosLibrary *DOSBase __attribute__((weak));
extern const LONG __aros_libreq_DOSBase __attribute__((weak));

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

/* Guarantee that __startup_entry is placed at the beginning of the binary */
__startup AROS_PROCH(__startup_entry, argstr, argsize, SysBase)
{
    AROS_PROCFUNC_INIT

    D(bug("Entering __startup_entry(\"%s\", %d, %x)\n", argstr, argsize, SysBase));

    /*
        No one program will be able to do anything useful without the dos.library,
        so we open it here instead of using the automatic opening system
    */
    DOSBase = (struct DosLibrary *)OpenLibrary((CONST_STRPTR)DOSNAME, 0);
    if (!DOSBase) return RETURN_FAIL;
    if (((struct Library *)DOSBase)->lib_Version < __aros_libreq_DOSBase)
        return RETURN_FAIL;

    __argstr  = (char *)argstr;
    __argsize = argsize;
    __startup_error = RETURN_FAIL;

    __startup_entries_init();
    __startup_entries_next();

    CloseLibrary((struct Library *)DOSBase);

    D(bug("Leaving __startup_entry\n"));

    return __startup_error;

    AROS_PROCFUNC_EXIT
} /* entry */


/* AROS: honor the AmigaOS-style __stack symbol (requested stack size, bytes).
   Weak -> resolves to 0 when the program does not define it (no behavior change). */
IPTR __stack __attribute__((weak)) = 0;  /* weak DEFINITION: 0 unless program overrides (avoids weak-undef link-gate failure) */

static IPTR __startup_call_main(IPTR argc, IPTR argv)
{
    return (IPTR)(*__main_function_ptr)((int)argc, (char **)argv);
}

static void __startup_main(struct ExecBase *SysBase)
{
    D(bug("Entering __startup_main\n"));

    /* Invoke the main function. A weak symbol is used as function name so that
       it can be overridden (for *nix stuff, for instance).  */
    {
        IPTR wantstack = (&__stack) ? __stack : 0;
        APTR newmem = (wantstack >= (IPTR)0x10000) ? AllocMem(wantstack, MEMF_ANY) : NULL;
        if (newmem)
        {
            struct StackSwapStruct sss;
            struct StackSwapArgs ssa;
            sss.stk_Lower   = newmem;
            sss.stk_Upper   = (APTR)((IPTR)newmem + wantstack);
            sss.stk_Pointer = sss.stk_Upper;
            ssa.Args[0] = (IPTR)__argc;
            ssa.Args[1] = (IPTR)__argv;
            __startup_error = (int)NewStackSwap(&sss, (APTR)__startup_call_main, &ssa);
            FreeMem(newmem, wantstack);
        }
        else
            __startup_error = (*__main_function_ptr) (__argc, __argv);
    }

    D(bug("Leaving __startup_main\n"));
}

ADD2SET(__startup_main, PROGRAM_ENTRIES, 127);

/*
    Stub function for GCC __main().

    The __main() function is originally used for C++ style constructors
    and destructors in C. This replacement does nothing and gets rid of
    linker-errors about references to __main().
*/
#ifdef AROS_NEEDS___MAIN
void __main(void)
{
    /* Do nothing. */
}
#endif
