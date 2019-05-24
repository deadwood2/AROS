/*
    Copyright © 2019, The AROS Development Team. All rights reserved.
    $Id$
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

/*****************************************************************************/
/* Code copied from compiler/startup/startup.c                               */

struct DosLibrary *DOSBase;

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

void (*__kick_start)(void *);
void (*__set_runtime_env)(void);

#define RUNTIMESTARTUP      "runtimestartup.so"
#define FUNC_SETRUNTIMEENV  "__set_runtime_env"
#define FUNC_KICKSTART      "__kick_start"

static void __bye()
{
    printf("Exiting...\n");
    exit(100);
}

void __runtimestartup()
{
    /* This thread is not an AROS Process/Task. Restrictions apply. */

    /* This is executed before main() via INIT_ARRAY section. The host thread
     * have not yet reached main(). It will never reach it, __kick_start has to
     * guarantee this.
     */

    void *__so_handle = NULL;

    __so_handle = dlopen("./"RUNTIMESTARTUP, RTLD_GLOBAL | RTLD_LAZY);
    if (__so_handle == NULL)
        __so_handle = dlopen(RUNTIMESTARTUP, RTLD_GLOBAL | RTLD_LAZY);

    if (__so_handle == NULL)
    {
        printf("<<ERROR>>Loader "RUNTIMESTARTUP" not found at either ./ or /usr/lib\n");
        __bye();
    }

    __set_runtime_env = (void (*)())dlsym(__so_handle, FUNC_SETRUNTIMEENV);
    if (__set_runtime_env == NULL)
    {
        printf("<<ERROR>> Function "FUNC_SETRUNTIMEENV" not found in "RUNTIMESTARTUP"\n");
        dlclose(__so_handle);
        __bye();
    }

    __kick_start = (void (*)(void *))dlsym(__so_handle, FUNC_KICKSTART);
    if (__kick_start == NULL)
    {
        printf("<<ERROR>> Function "FUNC_KICKSTART" not found in "RUNTIMESTARTUP"\n");
        dlclose(__so_handle);
        __bye();
    }

    __set_runtime_env();

    // TODO: error handling

    __kick_start(__startup_entry);

    // TODO: error handling
}
