/*
    Copyright (C) 2020, The AROS Development Team. All rights reserved.
*/


#include <string.h>
#include <stdio.h>

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

static BOOL extractvolume(const char* amigapath, char *volume, int vsize)
{
    /* Copy letters up to : or size */
    for (int i = 0; i < vsize - 1; i++)
    {
        /* Exit early on / before : */
        if (amigapath[i] == '/') return FALSE;

        volume[i] = amigapath[i];

        if (amigapath[i] == ':')
        {
            volume[i + 1] = 0;
            return TRUE;
        }
    }
    return FALSE;
}

LONG __shims_amiga2host(const char* func, const char *amigapath, char *hostpath)
{
    /* Make copy */
    strcpy(hostpath, amigapath);

    if (hostpath[0] != '/' && DOSBase) /* amiga path resolution works only when dos.library is up */
    {
        char volume[64];
        char adir[1024];

        adir[0] = 0;

        /* Get to path starting from ROOT: */

        if (strcasestr(hostpath, "ROOT:") == hostpath)
        {
            /* Do nothing */
        }
        else if (strcasestr(hostpath, "PROGDIR:") == hostpath)
        {
            extractvolume(hostpath, volume, 64);
            NameFromLock(GetProgramDir(), adir, 1024);
        }
        else if (strcasecmp(hostpath, "Console:") == 0)
        {
            strcpy(hostpath, "ROOT:dev/stdout");
        }
        else if (extractvolume(hostpath, volume, 64))
        {
            BPTR lock = Lock(volume, SHARED_LOCK);
            if (lock != BNULL)
            {
                NameFromLock(lock, adir, 1024);
                UnLock(lock);
            }
        }

        /* If the lock was resolved, modify the path */
        if (adir[0] != 0)
        {
            strcat(adir, "/");
            strreplace(hostpath, volume, adir);
        }

    }

    /* Convert path to Linux */
    strreplace(hostpath, "ROOT:","/");

    if (SB.sb_debugpath)
        printf("<<INFO>>: A2H [%s] %s -> %s\n", func, amigapath, hostpath);

    return 0;
}

/*****************************************************************************

    NAME */
        __attribute__((visibility("default")))

        LONG Path2Host(

/*  SYNOPSIS */
        CONST_STRPTR path,
        STRPTR buffer,
        LONG length)

/*  FUNCTION
        Converts a path that is either Amiga or Host to a Host path

    INPUTS
        path    - input path, either Amiga or Host
        buffer  - output buffer for converted path
        length  - size of output buffer

    RESULT
        Ignored. Always returns 0 (success).

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
    return __shims_amiga2host(__func__, path, buffer);
}
