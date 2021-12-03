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
#include <sys/mount.h>
#include <sys/utsname.h>
#include <regex.h>
#include <setjmp.h>

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
int __posixc_set_environptr(char ***environptr) { asm("int3");return 0; }
char *asctime(const struct tm *t) { asm("int3");return NULL; }
int raise(int a) { asm("int3");return 0; }
char *ctime_r(const time_t *t, char *a) { asm("int3");return NULL; }
struct tm *gmtime_r(const time_t *t, struct tm *a) { asm("int3");return NULL; }
struct tm *localtime_r(const time_t *t, struct tm *a) { asm("int3");return NULL; }
FILE *tmpfile(void) { asm("int3");return NULL; }
int chdir(const char *path) { asm("int3");return 0; }
int __posixc_creat (const char * filename, int mode) { asm("int3");return 0; }
int chown(const char *path, uid_t owner, gid_t group) { asm("int3");return 0; }
int execl(const char *path, const char *arg, ...) { asm("int3");return 0; }
int execlp(const char *path, const char *arg, ...) { asm("int3");return 0; }
int execv(const char *path, char *const argv[]) { asm("int3");return 0; }
int execve(const char *path, char *const argv[], char *const envp[]) { asm("int3");return 0; }
int execvp(const char *path, char *const argv[]) { asm("int3");return 0; }
pid_t vfork(void) { asm("int3");return 0; }
int fchdir(int fd) { asm("int3");return 0; }
int mkdir(const char *path, mode_t mode) { asm("int3");return 0; }
int __posixc_fstat(int fd, struct stat *sb) { asm("int3");return 0; }
int dup(int oldfd) { asm("int3");return 0; }
int fcntl (int fd, int cmd, ...) { asm("int3");return 0; }
int mkstemp(char *c) { asm("int3");return 0; }
int flock(int a, int b) { asm("int3");return 0; }
char *mktemp(char *c) { asm("int3");return NULL; }
int getfsstat (struct statfs *a, long b, int c) { asm("int3");return 0; }
char *getpass(const char *c) { asm("int3");return NULL; }
int pipe(int filedes[2]) { asm("int3");return 0; }
unsigned sleep(unsigned a) { asm("int3");return 0; }
long int jrand48(unsigned short a[3]) { asm("int3");return 0; }
void lcong48(unsigned short a[7]) { asm("int3"); }
long int lrand48(void) { asm("int3");return 0; }
long int nrand48(unsigned short a[3]) { asm("int3");return 0; }
unsigned short int *seed48(unsigned short a[3]) { asm("int3");return NULL; }
int statfs(const char *a, struct statfs *b) { asm("int3");return 0; }
int uname(struct utsname *a) { asm("int3");return 0; }
pid_t waitpid(pid_t a, int *b, int c) { asm("int3");return 0; }
pid_t wait(int *a) { asm("int3");return 0; }
int posix_memalign(void **a, size_t b, size_t c) { asm("int3");return 0; }
int __posixc_alphasort(const struct dirent **a, const struct dirent **b) { asm("int3");return 0; }
int __posixc_scandir (const char *dir, struct dirent ***namelist, int (*select)(const struct dirent *), int (*compar)(const struct dirent **, const struct dirent **)) { asm("int3");return 0; }
int regcomp(regex_t *restrict a, const char *restrict b, int c) { asm("int3");return 0; }
size_t regerror(int a, const regex_t *restrict b, char *restrict c, size_t d) { asm("int3");return 0; }
int regexec(const regex_t *restrict a, const char *restrict b, size_t c, regmatch_t d[], int e) { asm("int3");return 0; }
void regfree(regex_t *a) { asm("int3"); }
char *strndup(const char *a , size_t b) { asm("int3");return 0; }
int __posixc_nixmain(int (*main)(int argc, char *argv[]), int argc, char *argv[]) { asm("int3");return 0; }
mode_t umask(mode_t numask) { asm("int3");return 0; }
void siglongjmp(sigjmp_buf a, int b) { asm("int3"); }
int sigsetjmp(sigjmp_buf a, int b) { asm("int3");return 0; }
void clearerr(FILE *f) { asm("int3"); }
int rmdir(const char *path) { asm("int3");return 0; }
char *tempnam(const char *s, const char *ss) { asm("int3");return NULL; }
int ioctl(int a, int b, ...) { asm("int3");return 0; }
int dup2(int oldfd, int newfd) { asm("int3");return 0; }
