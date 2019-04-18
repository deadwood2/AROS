/*
    Copyright © 2019, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <aros/symbolsets.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

DEFINESET(INIT_ARRAY);
DEFINESET(INIT);

int main();
void (*__kick_start)(void *, void *);
void (*__set_runtime_env)(void);


//FIXME: isn't there a set function already available for this?
//FIXME: how this will work with applications not compiled through build system?
static void __run_program_sets(struct ExecBase *SysBase)
{
    /* This function is called back from startup code in library */
    /* By being in linklib it has access to application sets */
    if (!set_open_libraries()) asm("int3");
    if (!set_call_funcs(SETNAME(INIT), 1, 1)) asm("int3");
    set_call_funcs(SETNAME(INIT_ARRAY), 1, 0);
}

#define RUNTIMESTARTUP  "runtimestartup.so"

void __init_runtime()
{
    /* This thread is not an AROS Process/Task. Restrictions apply. */

    /* This is executed before main() via INIT_ARRAY section. The host thread
     * have not yet reached main(). It will never reach it, __kick_start has to
     * guarantee this.
     */

    void *__so_handle = NULL;

    __so_handle = dlopen("./"RUNTIMESTARTUP, RTLD_LAZY);
    if (__so_handle == NULL)
        __so_handle = dlopen(RUNTIMESTARTUP, RTLD_LAZY);

    // TODO: error handling

    __set_runtime_env   = (void (*)())dlsym(__so_handle, "__set_runtime_env");
    __kick_start        = (void (*)(void *, void *))dlsym(__so_handle, "__kick_start");

    // TODO: error handling

    __set_runtime_env();

    // TODO: error handling

    __kick_start(main, __run_program_sets);

    // TODO: error handling
}
