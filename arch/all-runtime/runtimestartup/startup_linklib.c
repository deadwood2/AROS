/*
    Copyright © 2019, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <aros/symbolsets.h>

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <libgen.h>
#include <pwd.h>

//#define PACKAGED_BUILD

static void set_runtime_env()
{
    /* Paths needs to end with "/" */
    STRPTR RUNTIME_ROOT = NULL, AROSSYS = NULL, USERSYS = NULL;

    const int _size = 512;
    char buff[_size];
    RUNTIME_ROOT    = malloc(_size);
    AROSSYS         = malloc(_size);
    USERSYS         = malloc(_size);
    getcwd(buff, _size);

#if defined(PACKAGED_BUILD)
    /* Paths are relative to executable directory */
    strcat(RUNTIME_ROOT, buff);
    strcat(RUNTIME_ROOT, "/SYS/");

    strcat(AROSSYS, "ROOT:");
    strcat(AROSSYS, buff + 1);
    strcat(AROSSYS, "/SYS/");

    strcat(USERSYS, "ROOT:");
    strcat(USERSYS, buff + 1);
    strcat(USERSYS, "/USERSYS/");
#else
    /* Paths using build system paths */
    char *t;
    struct passwd *pw;

    t = dirname(strdup(buff));
    strcpy(buff, t);
    buff[strlen(t)] = 0;
    t = dirname(strdup(buff));
    strcpy(buff, t);
    buff[strlen(t)] = 0;

    /* Two directories up from Wanderer executable */
    strcat(RUNTIME_ROOT, buff);
    strcat(RUNTIME_ROOT, "/");

    strcat(AROSSYS, "ROOT:");
    strcat(AROSSYS, buff + 1);
    strcat(AROSSYS, "/");

    /* ~/.aros/ */
    pw = getpwuid(getuid());
    strcat(USERSYS, "ROOT:");
    strcat(USERSYS, pw->pw_dir + 1);
    strcat(USERSYS, "/.aros/");
#endif

    printf("RUNTIME_ROOT: %s\n", RUNTIME_ROOT);
    printf("AROSSYS     : %s\n", AROSSYS);
    printf("USERSYS     : %s\n", USERSYS);

    setenv("AROSRUNTIME_ROOT", RUNTIME_ROOT, 0);
    setenv("AROSSYS", AROSSYS, 0);
    setenv("USERSYS", USERSYS, 0);
}


DEFINESET(INIT_ARRAY);
DEFINESET(INIT);

int main();
void (*__kick_start)(void *, void *);

static void __run_program_sets(struct ExecBase *SysBase)
{
    /* This function is called back from startup code in library */
    /* By being in linklib it has access to application sets */
    if (!set_open_libraries()) asm("int3");
    if (!set_call_funcs(SETNAME(INIT), 1, 1)) asm("int3");
    set_call_funcs(SETNAME(INIT_ARRAY), 1, 0);
}

void __start_program()
{
    /* This thread is not an AROS Process/Task. Restrictions apply. */

    /* This is executed before main() via INIT_ARRAY section. The host thread
     * have not yet reached main(). It will never reach it, __kick_start has to
     * guarantee this.
     */
    set_runtime_env();

    //
    STRPTR root = getenv("AROSRUNTIME_ROOT");

    TEXT path[512] = { 0 };
    if (root) strncat(path, root, strlen(root));
    strncat(path, "Libs/runtimestartup.library", strlen("Libs/runtimestartup.library"));

    void *__so_handle = dlopen(path, RTLD_LAZY);
    __kick_start = (void (*)())dlsym(__so_handle, "__kick_start");

    __kick_start(main, __run_program_sets);
    //
}
