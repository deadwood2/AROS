/*
    Copyright (C) 2012-2013, The AROS Development Team. All rights reserved.
*/

#include <aros/symbolsets.h>

#include <proto/exec.h>
#include <signal.h>
#include <setjmp.h>
#include <time.h>

const unsigned short int * const *__ctype_b_ptr = NULL;
const unsigned char * const *__ctype_toupper_ptr = NULL;
const unsigned char * const *__ctype_tolower_ptr = NULL;

void __assert(const char * expr, const char * file, unsigned int line) { asm("int3"); }
void exit(int code) { asm("int3"); }
void _Exit(int code) { asm("int3"); }
void _exit(int code) { asm("int3"); }
void free(void *memory) { asm("int3"); }
void *malloc(size_t size) { asm("int3");return NULL; }
void *malloc_align(size_t size, size_t alignment) { asm("int3");return NULL; }
void *realloc(void *oldmem, size_t newsize) { asm("int3");return NULL; }
void *realloc_nocopy(void *oldmem, size_t newsize) { asm("int3");return NULL; }
void *calloc(size_t count, size_t size) { asm("int3");return NULL; }
int rand(void) { asm("int3");return 0; }
int remove(const char *filename) { asm("int3");return 0; }
int rename(const char *old, const char *new) { asm("int3");return 0; }
int __stdc_gmtoffset(void) { asm("int3");return 0; }
void __stdc_program_startup(jmp_buf exitjmp, int *errorptr) { asm("int3"); }
int *__stdc_get_errorptr(void) { asm("int3");return NULL; }
int *__stdc_geterrnoptr(void) { asm("int3");return NULL; }
int *__stdc_set_errorptr(int *errorptr) { asm("int3");return NULL; }
void __stdc_set_exitjmp(jmp_buf exitjmp, jmp_buf previousjmp) { asm("int3"); }
char *__stdc_strerror (int n) { asm("int3");return NULL; }
char *strdup(const char *str) { asm("int3");return NULL; }
time_t time(time_t *t) { asm("int3");return *t; }
int __stdc_ioerr2errno(int ioerr) { asm("int3");return 0; }
void abort(void) { asm("int3"); }
__sighandler_t *signal(int a, __sighandler_t * h) { asm("int3");return NULL; }
clock_t clock(void) { clock_t c; asm("int3");return c; }
char *ctime(const time_t *t) { asm("int3");return NULL; }
void srand(unsigned int seed)  { asm("int3"); }
struct tm *gmtime(const time_t *t) { asm("int3");return NULL; }
char *strtok (char * str, const char * sep) { asm("int3");return NULL; }
struct tm *localtime(const time_t *t) { asm("int3");return NULL; }
int atexit(void (*func)(void)) { asm("int3");return 0; }

