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
#include <sys/resource.h>

void __assert(const char * expr, const char * file, unsigned int line) { asm("int3"); }
void *malloc_align(size_t size, size_t alignment) { asm("int3");return NULL; }
int rand(void) { asm("int3");return 0; }
void srand(unsigned int seed)  { asm("int3"); }
char *ctime(const time_t *t) { asm("int3");return NULL; }
struct tm *gmtime(const time_t *t) { asm("int3");return NULL; }
char *strtok (char * str, const char * sep) { asm("int3");return NULL; }
int scanf(const char *restrict c, ...) { asm("int3");return 0; }
double drand48(void) { asm("int3");return 0.0; }
void srand48(long a) { asm("int3"); }
int fchmod(int fildes, mode_t mode) { asm("int3");return 0; }
int fchown(int fd, uid_t owner, gid_t group) { asm("int3");return 0; }
void rewind(FILE *f) { asm("int3"); }
int utime(const char *c, const struct utimbuf *u) { asm("int3");return 0; }
int system(const char *string) { asm("int3");return 0; }
int rename(const char *old, const char *new) { asm("int3");return 0; }
int setvbuf(FILE *restrict f, char *restrict c, int a, size_t b) { asm("int3");return 0; }
char *gets(char *c) { asm("int3");return NULL; }
long random(void) { asm("int3");return 0; }
void srandom(unsigned a) { asm("int3"); }
int vprintf(const char *restrict c, va_list va) { asm("int3");return 0; }
double erand48(unsigned short a[3]) { asm("int3");return 0.0; }
int usleep(useconds_t microseconds) { asm("int3");return 0; }
void setbuf(FILE *restrict f, char *restrict c) { asm("int3"); }
FILE *freopen(const char *restrict c, const char *restrict d, FILE *restrict f) { asm("int3");return NULL; }
int fscanf(FILE *restrict f, const char *restrict c, ...) { asm("int3");return 0; }
pid_t getppid(void) { asm("int3");return 0; }
int kill(pid_t p, int a) { asm("int3");return 0; }
int pclose(FILE *f) { asm("int3");return 0; }
FILE *popen(const char *c, const char *d) { asm("int3");return NULL; }
int __posixc_fgetpos(FILE *restrictf , fpos_t *restrict a) { asm("int3");return 0; }
int __posixc_fsetpos(FILE *f , const fpos_t *a) { asm("int3");return 0; }
char *asctime(const struct tm *t) { asm("int3");return NULL; }
int raise(int a) { asm("int3");return 0; }
char *ctime_r(const time_t *t, char *a) { asm("int3");return NULL; }
FILE *tmpfile(void) { asm("int3");return NULL; }
int chdir(const char *path) { asm("int3");return 0; }
int __posixc_creat (const char * filename, int mode) { asm("int3");return 0; }
int chown(const char *path, uid_t owner, gid_t group) { asm("int3");return 0; }
int __progonly_execlp(const char *path, const char *arg, ...) { asm("int3");return 0; }
int __progonly_execve(const char *path, char *const argv[], char *const envp[]) { asm("int3");return 0; }
int fchdir(int fd) { asm("int3");return 0; }
int mkdir(const char *path, mode_t mode) { asm("int3");return 0; }
int mkstemp(char *c) { asm("int3");return 0; }
int flock(int a, int b) { asm("int3");return 0; }
char *mktemp(char *c) { asm("int3");return NULL; }
int getfsstat (struct statfs *a, long b, int c) { asm("int3");return 0; }
char *getpass(const char *c) { asm("int3");return NULL; }
unsigned sleep(unsigned a) { asm("int3");return 0; }
long int jrand48(unsigned short a[3]) { asm("int3");return 0; }
void lcong48(unsigned short a[7]) { asm("int3"); }
long int lrand48(void) { asm("int3");return 0; }
long int nrand48(unsigned short a[3]) { asm("int3");return 0; }
unsigned short int *seed48(unsigned short a[3]) { asm("int3");return NULL; }
int statfs(const char *a, struct statfs *b) { asm("int3");return 0; }
int uname(struct utsname *a) { asm("int3");return 0; }
pid_t wait(int *a) { asm("int3");return 0; }
int posix_memalign(void **a, size_t b, size_t c) { asm("int3");return 0; }
int __posixc_alphasort(const struct dirent **a, const struct dirent **b) { asm("int3");return 0; }
int __posixc_scandir (const char *dir, struct dirent ***namelist, int (*select)(const struct dirent *), int (*compar)(const struct dirent **, const struct dirent **)) { asm("int3");return 0; }
int regcomp(regex_t *restrict a, const char *restrict b, int c) { asm("int3");return 0; }
size_t regerror(int a, const regex_t *restrict b, char *restrict c, size_t d) { asm("int3");return 0; }
int regexec(const regex_t *restrict a, const char *restrict b, size_t c, regmatch_t d[], int e) { asm("int3");return 0; }
void regfree(regex_t *a) { asm("int3"); }
char *strndup(const char *a , size_t b) { asm("int3");return 0; }
mode_t umask(mode_t numask) { asm("int3");return 0; }
void clearerr(FILE *f) { asm("int3"); }
int rmdir(const char *path) { asm("int3");return 0; }
char *tempnam(const char *s, const char *ss) { asm("int3");return NULL; }
ssize_t readlink(const char * restrict path, char * restrict buf, size_t bufsize) { asm("int3");return 0; }
off_t __posixc_ftello(FILE *f) { asm("int3");return 0; }
int __posixc_fseeko(FILE *f, off_t a, int b) { asm("int3");return 0; }
int dirfd(DIR *dir) { asm("int3");return 0; }
struct group *getgrgid(gid_t a) { asm("int3");return NULL; }
struct group *getgrnam(const char *a) { asm("int3");return NULL; }
struct passwd *getpwnam(const char *a) { asm("int3");return NULL; }
struct passwd *getpwuid(uid_t a) { asm("int3");return NULL; }
int link(const char *name1, const char *name2) { asm("int3");return 0; }
int mknod(const char *path, mode_t mode, dev_t dev) { asm("int3");return 0; }
int symlink(const char *name1, const char *name2) { asm("int3");return 0; }
int utimes(const char *a, const struct timeval b[2]) { asm("int3");return 0; }
int fsync(int fd) { asm("int3");return 0; }
int sigaction(int a, const struct sigaction *b, struct sigaction *c) { asm("int3");return 0; }
int sigaddset(sigset_t *a, int b) { asm("int3");return 0; }
int sigemptyset(sigset_t *a) { asm("int3");return 0; }
int sigismember(const sigset_t *a, int b) { asm("int3");return 0; }
int sigprocmask(int a, const sigset_t *b, sigset_t *c) { asm("int3");return 0; }
int getc_unlocked(FILE *f) { asm("int3");return 0; }
void rewinddir(DIR *dir) { asm("int3"); }
void setgrent(void) { asm("int3"); }
void setpwent(void) { asm("int3"); }
int __stdc_mb_cur_max(void) { asm("int3");return 0; }
char *getlogin(void) { asm("int3");return NULL; }
char *ttyname(int fd) { asm("int3");return NULL; }
int setgid(gid_t gid) { asm("int3");return 0; }
int setuid(uid_t uid) { asm("int3");return 0; }
