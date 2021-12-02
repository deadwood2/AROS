/*
    Copyright (C) 2012-2013, The AROS Development Team. All rights reserved.
*/

#include <aros/symbolsets.h>

#include <proto/exec.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <utime.h>
#include <dirent.h>

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
void srand(unsigned int seed)  { asm("int3"); }
time_t time(time_t *t) { asm("int3");return *t; }
char *ctime(const time_t *t) { asm("int3");return NULL; }
struct tm *gmtime(const time_t *t) { asm("int3");return NULL; }
struct tm *localtime(const time_t *t) { asm("int3");return NULL; }
clock_t clock(void) { clock_t c; asm("int3");return c; }
char *strdup(const char *str) { asm("int3");return NULL; }
int atexit(void (*func)(void)) { asm("int3");return 0; }
void abort(void) { asm("int3"); }
__sighandler_t *signal(int a, __sighandler_t * h) { asm("int3");return NULL; }
char *strtok (char * str, const char * sep) { asm("int3");return NULL; }
int gettimeofday(struct timeval * restrict tv, struct timezone * restrict tz) { asm("int3");return 0; }
int printf(const char *restrict c, ...) { asm("int3");return 0; }
int puts(const char *c) { asm("int3");return 0; }
int putchar(int c) { asm("int3");return 0; }
int fflush(FILE *f) { asm("int3");return 0; }
int scanf(const char *restrict c, ...) { asm("int3");return 0; }
FILE * __stdio_getstdout(void) { asm("int3");return NULL; }
FILE * __stdio_getstderr(void) { asm("int3");return NULL; }
int fprintf(FILE *restrict f, const char *restrict c, ...) { asm("int3");return 0; }
size_t fwrite(const void *restrict c, size_t b, size_t a, FILE *restrict f) { asm("int3");return 0; }
int remove(const char *filename) { asm("int3");return 0; }
FILE *__posixc_fopen(const char *restrict c, const char *restrict d) { asm("int3");return NULL; }
int fclose(FILE *f) { asm("int3");return 0; }
size_t fread(void *restrict a, size_t b, size_t c, FILE *restrict f)  { asm("int3");return 0; }
int fseek(FILE * f, long a, int b) { asm("int3");return 0; }
double drand48(void) { asm("int3");return 0.0; }
void srand48(long a) { asm("int3"); }
FILE *fdopen(int a, const char * b) { asm("int3");return NULL; }
int ferror(FILE *f) { asm("int3");return 0; }
int fgetc(FILE *f) { asm("int3");return 0; }
int fputc(int a, FILE *f) { asm("int3");return 0; }
FILE * __stdio_getstdin(void) { asm("int3");return NULL; }
int ungetc(int a, FILE *f) { asm("int3");return 0; }
void perror(const char *c) { asm("int3"); }
int *__stdc_geterrnoptr(void) { asm("int3");return NULL; }
int fchmod(int fildes, mode_t mode) { asm("int3");return 0; }
int fchown(int fd, uid_t owner, gid_t group) { asm("int3");return 0; }
int fileno(FILE *f) { asm("int3");return 0; }
char *getenv(const char *name) { asm("int3");return NULL; }
int isatty(int fd) { asm("int3");return 0; }
int __posixc_lstat(const char * restrict path, struct stat * restrict sb) { asm("int3");return 0; }
int __posixc_stat(const char * restrict path, struct stat * restrict sb) { asm("int3");return 0; }
void rewind(FILE *f) { asm("int3"); }
char *strerror(int a) { asm("int3");return NULL; }
int utime(const char *c, const struct utimbuf *u) { asm("int3");return 0; }
int system(const char *string) { asm("int3");return 0; }
int rename(const char *old, const char *new) { asm("int3");return 0; }
char *fgets(char *restrict c, int a, FILE *restrict f) { asm("int3");return NULL; }
int fputs(const char *restrict a, FILE *restrict f) { asm("int3");return 0; }
int setvbuf(FILE *restrict f, char *restrict c, int a, size_t b) { asm("int3");return 0; }
int putenv(const char *c) { asm("int3");return 0; }
char *gets(char *c) { asm("int3");return NULL; }
long random(void) { asm("int3");return 0; }
void srandom(unsigned a) { asm("int3"); }
long ftell(FILE *f) { asm("int3");return 0; }
int vfprintf(FILE *restrict f, const char *restrict c, va_list va) { asm("int3");return 0; }
int vprintf(const char *restrict c, va_list va) { asm("int3");return 0; }
double erand48(unsigned short a[3]) { asm("int3");return 0.0; }
off_t __posixc_lseek(int filedes, off_t offset, int whence) { asm("int3");return 0; }
ssize_t read(int d, void *buf, size_t nbytes) { asm("int3");return 0; }
ssize_t write(int fd, const void *buf, size_t nbytes) { asm("int3");return 0; }
int usleep(useconds_t microseconds) { asm("int3");return 0; }
int feof(FILE *f) { asm("int3");return 0; }
pid_t getpid(void) { asm("int3");return 0; }
void setbuf(FILE *restrict f, char *restrict c) { asm("int3"); }
FILE *freopen(const char *restrict c, const char *restrict d, FILE *restrict f) { asm("int3");return NULL; }
int access(const char *path, int mode) { asm("int3");return 0; }
int chmod(const char *path, mode_t mode) { asm("int3");return 0; }
int closedir(DIR *dir) { asm("int3");return 0; }
int fscanf(FILE *restrict f, const char *restrict c, ...) { asm("int3");return 0; }
pid_t getppid(void) { asm("int3");return 0; }
int kill(pid_t p, int a) { asm("int3");return 0; }
DIR *opendir(const char *filename) { asm("int3");return NULL; }
int pclose(FILE *f) { asm("int3");return 0; }
FILE *popen(const char *c, const char *d) { asm("int3");return NULL; }
int __posixc_fgetpos(FILE *restrictf , fpos_t *restrict a) { asm("int3");return 0; }
int __posixc_fsetpos(FILE *f , const fpos_t *a) { asm("int3");return 0; }
struct dirent *__posixc_readdir(DIR *dir) { asm("int3");return NULL; }
char *getcwd(char *buf, size_t size) { asm("int3");return NULL; }


int __stdc_gmtoffset(void) { asm("int3");return 0; }
void __stdc_program_startup(jmp_buf exitjmp, int *errorptr) { asm("int3"); }
int *__stdc_get_errorptr(void) { asm("int3");return NULL; }
int *__stdc_set_errorptr(int *errorptr) { asm("int3");return NULL; }
void __stdc_set_exitjmp(jmp_buf exitjmp, jmp_buf previousjmp) { asm("int3"); }
char *__stdc_strerror (int n) { asm("int3");return NULL; }

int __stdc_ioerr2errno(int ioerr) { asm("int3");return 0; }

