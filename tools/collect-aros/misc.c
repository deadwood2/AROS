/*
    Copyright (C) 1995-2014, The AROS Development Team. All rights reserved.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/param.h>
#include <string.h>

#include "misc.h"
#include <stdarg.h>
#include <stdarg.h>

#ifdef _WIN32
#define PATH_SEPARATOR ';'

/* If we're running on MinGW, PATH is in native Windows form while
   COMPILER_PATH has ';' as entries separator but still has '/' as directory
   separator, so we have to convert it. This is what this magic for. */

void copy_path(char *to, char *from)
{
    do {
        if (*from == '/')
            *to = '\\';
        else
            *to = *from;
        to++;
    } while (*from++);
}
#else
#define PATH_SEPARATOR ':'
#define copy_path strcpy
#endif

char *program_name;
void nonfatal(const char *msg, const char *errorstr)
{
    if (msg != NULL)
        fprintf(stderr, "%s: %s: %s\n" , program_name, msg, errorstr);
    else
        fprintf(stderr, "%s: %s\n" , program_name, errorstr);
}

void fatal(const char *msg, const char *errorstr)
{
    nonfatal(msg, errorstr);
    exit(EXIT_FAILURE);
}

void diag_printf(const char *fmt, ...)
{
    va_list ap;
    printf("DIAG: ");
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
}

#ifdef __linux__
static const char *col_temp_dir(void) { const char *t = getenv("TMPDIR"); return t ? t : "/tmp"; }
#else
static const char *col_temp_dir(void) { return "T:"; }
#endif


void set_compiler_path(void)
{
    static int path_set = 0;

    if (!path_set)
    {
        char *compiler_path = getenv("COMPILER_PATH");
        char *path          = getenv("PATH");

        if (compiler_path && path)
        {
            char *new_path;
            size_t compiler_path_len = strlen(compiler_path);
            size_t path_len          = strlen(path);

            new_path = malloc(5 + compiler_path_len + 1 + path_len + 1);
            if (new_path)
            {
                strcpy(new_path, "PATH=");
                copy_path(new_path + 5, compiler_path);
                new_path[5 + compiler_path_len] = PATH_SEPARATOR;
                strcpy(new_path + 5 + compiler_path_len + 1, path);

                if (putenv(new_path) == 0)
                    path_set = 1;
            }
        }
    }
}

#ifndef _HAVE_LIBIBERTY_

void *xmalloc(size_t size)
{
    void *ret = malloc(size);
    if (ret == NULL)
    {
        fatal("xmalloc", strerror(errno));
    }

    return ret;
}

char *make_temp_file(char *suffix __attribute__((unused)))
{
    int fd;
    /* If you're unlucky enough to not have libiberty available, you'll have
       to live with temporary files in /tmp and no suffix; it's ok for our own
       purposes,  */
    static int __catmp_ctr = 0;
    char template[64];
    {
        const char *td = col_temp_dir();
        size_t tdn = strlen(td);
        /* On AROS the temp dir is a volume ("T:"); a "/" right after the colon
           is parent-of-root navigation resolved against the per-process current
           dir, so it points at different places in the collect-aros parent and
           its vfork children. Use a volume-absolute "T:catmp..." (no slash). */
        const char *sep = (tdn && (td[tdn-1] == ":" [0] || td[tdn-1] == "/" [0])) ? "" : "/";
        snprintf(template, sizeof(template), "%s%scatmp%d_%dXXXXXX", td, sep, (int)getpid(), __catmp_ctr++);
    }

    fd = mkstemp(template);
    if (fd == -1)
        return NULL;

    if (close(fd) != 0)
        fatal("make_temp_file()/close()", strerror(errno));

    return strdup(template);
}

#endif
