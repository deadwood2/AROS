#ifndef _POSIXC_STDLIB_H_
#define _POSIXC_STDLIB_H_

/*
    Copyright � 1995-2025, The AROS Development Team. All rights reserved.
    $Id$

    POSIX.1-2008 header file stdlib.h
*/

#include <aros/features.h>

/* C99 */
#include <aros/stdc/stdlib.h>

/* It seems that also stdlib.h defines alloca() */
#include <alloca.h>

__BEGIN_DECLS

/* GNU/SVID extensions */
#if defined(_GNU_SOURCE)
double drand48(void);
double erand48(unsigned short [3]);
char *initstate(unsigned, char *, int);
long int jrand48(unsigned short int [3]);
void lcong48(unsigned short int [7]);
long int lrand48(void);
long int mrand48(void);
long int nrand48(unsigned short int [3]);
unsigned short int *seed48(unsigned short int [3]);
void srand48(long int);
void srandom(unsigned);
#endif

/* POSIX.1-2001 */
#if defined(_GNU_SOURCE) || (_POSIX_C_SOURCE >= 200112L)
int setenv(const char *, const char *, int);
int unsetenv(const char *);
int posix_memalign(void **memptr, size_t alignment, size_t size);
char *realpath(const char * restrict, char * restrict);
#endif

/* XSI extensions (X/Open, e.g. XPG4, POSIX.1-2001) */
#if defined(_GNU_SOURCE) || defined(_XOPEN_SOURCE)
int mkstemp(char *);
#endif

/* BSD / GNU extensions */
#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
char *mktemp(char *);
int getloadavg(double loadavg[], int n);
#endif

/* Deprecated or unimplemented functions (preserved) */
/* NOTIMPL long a64l(const char *); */
/* NOTIMPL int getsubopt(char **, char *const *, char **); */
/* NOTIMPL int grantpt(int); */
/* NOTIMPL char *l64a(long); */
/* NOTIMPL char *mkdtemp(char *); */
/* NOTIMPL int posix_openpt(int); */
/* NOTIMPL char *ptsname(int); */
/* NOTIMPL int rand_r(unsigned int *); */
/* NOTIMPL void setkey(const char *); */
/* NOTIMPL int unlockpt(int); */

/* Always available */
int putenv(const char *);
long random(void);

/* Deprecated POSIX (still implemented) */
char *setstate(char *);

/* POSIX.1-2008 / XSI-7 / GNU extensions */
#if defined(_GNU_SOURCE) || (_XOPEN_SOURCE >= 700) || (_POSIX_C_SOURCE >= 200809L)
#include <aros/posixc/locale.h>

/* NOTIMPL long                strtol_l(const char * restrict nptr, char ** restrict endptr, int base, locale_t loc); */
/* NOTIMPL float               strtof_l(const char * restrict nptr, char ** restrict endptr, locale_t loc); */
/* NOTIMPL double              strtod_l(const char * restrict nptr, char ** restrict endptr, locale_t loc); */
/* NOTIMPL long double         strtold_l(const char * restrict nptr, char ** restrict endptr, locale_t loc); */
/* NOTIMPL unsigned long       strtoul_l(const char * restrict nptr, char ** restrict endptr, int base, locale_t loc); */
/* NOTIMPL long long           strtoll_l(const char * restrict nptr, char ** restrict endptr, int base, locale_t loc); */
/* NOTIMPL unsigned long long  strtoull_l(const char * restrict nptr, char ** restrict endptr, int base, locale_t loc); */
#endif

char *getenv(const char *name);
int system(const char *string);

__END_DECLS

#endif /* _POSIXC_STDLIB_H_ */
