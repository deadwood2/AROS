/*
    Copyright © 2020, The AROS Development Team. All rights reserved.
    $Id$
*/


#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

#include <proto/dos.h>

#include "internal.h"

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

LONG __shims_amiga2host(const char *amigapath, char *hostpath)
{
    /* Make copy */
    strcpy(hostpath, amigapath);

    if (DOSBase) /* PROGDIR resolution works only when dos.library is up */
    {
printf("!Y!\n");
        /* Get to path starting from ROOT: */
        if (strstr(hostpath, "PROGDIR:") != NULL)
        {
            char progdir[1024];
            NameFromLock(GetProgramDir(), progdir, 1024);
            strcat(progdir, "/");
            strreplace(hostpath, "PROGDIR:", progdir);
        }
    }
    else printf("!N!\n");

    /* Convert path to Linux */
    strreplace(hostpath, "ROOT:","/");

    return 0;
}

