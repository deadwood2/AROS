/*
    Copyright © 2019, The AROS Development Team. All rights reserved.
    $Id$
*/


#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/dos.h>

struct aa
{
    struct ExecBase *sysBase;
    struct Library *dosBase;
};

struct aa AA;

#define SysBase AA.sysBase
#define DOSBase AA.dosBase

BOOL inited = FALSE;

static void strreplace(STRPTR target, CONST_STRPTR from, CONST_STRPTR to)
{
    STRPTR pos = strstr(target, from);
    if (pos != NULL)
    {
        TEXT buff[512] = {0};
        ULONG targetlen = strlen(target);
        ULONG headlen = pos - target;
        ULONG taillen = targetlen - strlen(from) - headlen;

        // add head
        strncat(buff, target, headlen);

        // add to
        strncat(buff, to, strlen(to));

        // add tail
        strncat(buff, pos + strlen(from), taillen);

        // copy back
        strncpy(target, buff, strlen(buff));
        target[strlen(buff)] = 0;
    }
}

APTR __get_sysbase();

static void init()
{
    if (inited)
        return;

    AA.sysBase = (struct ExecBase *)__get_sysbase();

    if (!AA.sysBase)
    {
        printf("SysBase not acquired\n");
        return;
    }

    AA.dosBase = OpenLibrary("dos.library", 0L);

    if (!AA.dosBase)
    {
        printf("DOSBase not acquired\n");
        return;
    }

    inited = TRUE;
    printf("Init completed\n");

}

__attribute__((visibility("default"))) FILE * fopen (const char * restrict pathname, const char * restrict mode)
{
    FILE *_return = NULL;
    FILE *(*fun)(const char * restrict pathname, const char * restrict mode);
    char lpathname[1024];

    init();

    /* Make copy */
    strcpy(lpathname, pathname);

    if (inited)
    {
        /* Get to path starting from ROOT: */
        if (strstr(lpathname, "PROGDIR:") != NULL)
        {
            char progdir[1024];
            NameFromLock(GetProgramDir(), progdir, 1024);
            strcat(progdir, "/");
            strreplace(lpathname, "PROGDIR:", progdir);
        }
    }

    /* Convert path to Linux */
    strreplace(lpathname, "ROOT:","/");
    printf("path: %s\n", lpathname);

    fun = dlsym(RTLD_NEXT, "fopen");
    _return = fun(lpathname, mode);

    return _return;
}
