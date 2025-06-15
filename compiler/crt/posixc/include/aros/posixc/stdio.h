#ifndef _POSIXC_STDIO_H_
#define _POSIXC_STDIO_H_

/*
    Copyright © 1995-2025, The AROS Development Team. All rights reserved.
    $Id$

    POSIX.1-2008 header file stdio.h
*/

#include <aros/features.h>
#include <aros/stdc/stdio.h>

#include <aros/types/off_t.h>
#include <aros/types/ssize_t.h>

#define L_ctermid	FILENAME_MAX
#define P_tmpdir	"T:"

__BEGIN_DECLS

int fileno(FILE *);
void flockfile(FILE *);
int getc_unlocked(FILE *);
void funlockfile(FILE *);

/* Extensions requiring feature guards */

/* GNU or XSI */
#if defined(__GNU_SOURCE) || defined(_XOPEN_SOURCE)
FILE *fdopen(int filedes, const char *mode);
int pclose(FILE *);
FILE *popen(const char *, const char *);
char *tempnam(const char *dir, const char *pfx);
#endif

/* GNU extensions */
#if defined(__GNU_SOURCE)
/* NOTIMPL char *ctermid(char *); */
/* NOTIMPL int dprintf(int, const char *restrict, ...); */
/* NOTIMPL FILE *fmemopen(void *restrict, size_t, const char *restrict); */
/* NOTIMPL int ftrylockfile(FILE *); */
/* NOTIMPL int getchar_unlocked(void); */
/* NOTIMPL ssize_t getdelim(char **restrict, size_t *restrict, int, FILE *restrict); */
/* NOTIMPL ssize_t getline(char **restrict, size_t *restrict, FILE *restrict); */
/* NOTIMPL FILE *open_memstream(char **, size_t *); */
/* NOTIMPL int putc_unlocked(int, FILE *); */
/* NOTIMPL int putchar_unlocked(int); */
/* NOTIMPL int renameat(int, const char *, int, const char *); */
/* NOTIMPL int vdprintf(int, const char *restrict, va_list); */
#endif

/* Large file support (non-standard) */
FILE *fopen64(const char * restrict filename, const char * restrict mode);
#if defined(__USE_FILE_OFFSET64)
static __inline__  FILE *fopen(const char * restrict filename, const char * restrict mode)
{
    return fopen64(filename, mode);
}
#else
FILE *fopen(const char * restrict filename, const char * restrict mode);
#endif

int fgetpos64(FILE * restrict stream, __fpos64_t * restrict pos);
int fsetpos64(FILE *stream, const __fpos64_t *pos);
#if defined(__USE_FILE_OFFSET64)
static __inline__  int fgetpos(FILE * restrict stream, fpos_t * restrict pos)
{
    return fgetpos64(stream, (__fpos64_t *)pos);
}
static __inline__  int fsetpos(FILE *stream, const fpos_t *pos)
{
    return fsetpos64(stream, (__fpos64_t *)pos);
}
#else
int fgetpos(FILE * restrict stream, fpos_t * restrict pos);
int fsetpos(FILE *stream, const fpos_t *pos);
#endif

#if defined(__off64_t_defined)
int fseeko64(FILE *stream, off64_t offset, int whence);
#endif
#if defined(__USE_FILE_OFFSET64)
static __inline__  int fseeko(FILE *stream, off_t offset, int whence)
{
    return fseeko64(stream, (__off64_t)offset, whence);
}
#else
int fseeko(FILE *stream, off_t offset, int whence);
#endif

#if defined(__off64_t_defined)
off64_t ftello64(FILE *stream);
#endif
#if defined(__USE_FILE_OFFSET64)
static __inline__  off_t ftello(FILE *stream)
{
    return (off_t)ftello64(stream);
}
#else
off_t ftello(FILE *stream);
#endif


/* Following function is here for clib2 compatibility and abc-shell
   Don't use in new code
*/
int __get_default_file(int file_descriptor, long *file_handle);

/* Deprecated POSIX functions */
static __inline__ int getw(FILE *stream)
{
    int word;
    return (fread(&word, sizeof(word), 1, stream) > 0) ? word : EOF;
}

static __inline__ int putw(int word, FILE *stream)
{
    return (fwrite(&word, sizeof(word), 1, stream) > 0) ? 0 : EOF;
}

__END_DECLS

#endif /* _POSIXC_STDIO_H_ */
