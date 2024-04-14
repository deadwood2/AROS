/*
    Copyright (C) 1995-2023, The AROS Development Team. All rights reserved.
*/
/* For comments and explanation of generated code look in writestart.c source code
   of the genmodule program */
#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/resident.h>
#include <aros/libcall.h>
#include <aros/asmcall.h>
#include <aros/symbolsets.h>
#include <aros/genmodule.h>
#include <dos/dos.h>

#include "internal/arosc_libdefs.h"


#undef SysBase
#undef OOPBase
#undef UtilityBase

#include <proto/exec.h>
#include <proto/alib.h>

//

static inline IPTR __inline_Exec_GetParentTaskStorageSlot(LONG __arg1, APTR __SysBase)
{
    AROS_LIBREQ(SysBase, 51)
    return AROS_LC1(IPTR, GetParentTaskStorageSlot,
        AROS_LCA(LONG,(__arg1),D0),
        struct ExecBase *, (__SysBase), 186, Exec    );
}

#define GetParentTaskStorageSlot(arg1) \
    __inline_Exec_GetParentTaskStorageSlot((arg1), __aros_getbase_SysBase())

//

struct aroscbase;
struct arosc_userdata;
struct arosc_ctype;
#define _STDIO_H_NOMACRO
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>
#include <dirent.h>
#include <wchar.h>
#include <regex.h>
#include <setjmp.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/utsname.h>
#include <sys/resource.h>

#if !defined(__off64_t_defined)
#define off64_t UQUAD
#endif
#ifndef GM_SEGLIST_FIELD
static BPTR __attribute__((unused)) GM_UNIQUENAME(seglist);
#define GM_SEGLIST_FIELD(LIBBASE) (GM_UNIQUENAME(seglist))
#endif
/* Required for TaskStorage manipulation */
extern struct ExecBase *SysBase;
#ifndef GM_ROOTBASE_FIELD
static LIBBASETYPEPTR GM_UNIQUENAME(rootbase);
#define GM_ROOTBASE_FIELD(LIBBASE) (GM_UNIQUENAME(rootbase))
#endif
struct __GM_DupBase {
    LIBBASETYPE base;
    LIBBASETYPEPTR oldbase;
    ULONG taskopencount;
    struct Task *task;
    APTR retaddr;
    LIBBASETYPEPTR oldpertaskbase;
};
static LONG __pertaskslot;
LIBBASETYPEPTR __GM_GetBaseParent(LIBBASETYPEPTR base)
{
    return ((struct __GM_DupBase *)base)->oldpertaskbase;
}
static inline LIBBASETYPEPTR __GM_GetPerTaskBase(void)
{
    return (LIBBASETYPEPTR)GetTaskStorageSlot(__pertaskslot);
}
static inline LIBBASETYPEPTR __GM_GetParentPerTaskBase(void)
{
    return (LIBBASETYPEPTR)GetParentTaskStorageSlot(__pertaskslot);
}
static inline void __GM_SetPerTaskBase(LIBBASETYPEPTR base)
{
    SetTaskStorageSlot(__pertaskslot, (IPTR)base);
}
#define LIBBASESIZE (sizeof(struct __GM_DupBase) + sizeof(struct Library *)*0)
static LONG __GM_BaseSlot;
char *__aros_getoffsettable(void)
{
    return (char *)GetTaskStorageSlot(__GM_BaseSlot);
}
void __aros_setoffsettable(void *base)
{
    SetTaskStorageSlot(__GM_BaseSlot, (IPTR)base);
}
struct aroscbase *__aros_getbase_aroscbase(void)
{
    return (struct aroscbase *)__aros_getoffsettable();
}
FILE * fopen(const char * restrict filename, const char * restrict mode);
FILE * fdopen(int filedes, const char *mode);
int fclose(FILE *stream);
int printf(const char * restrict format, ...);
int vprintf(const char * restrict format, va_list arg);
int fprintf(FILE * restrict stream, const char * restrict format, ...);
int vfprintf(FILE * restrict stream, const char * restrict format, va_list arg);
int fputc(int c, FILE *stream);
int fputs(const char * restrict s, FILE * restrict stream);
int puts(const char *s);
int fflush(FILE *stream);
int fgetc(FILE *stream);
int ungetc(int c, FILE *stream);
char * fgets(char * restrict s, int n, FILE * restrict stream);
int feof(FILE *stream);
int ferror(FILE *stream);
int fileno(FILE *);
FILE * freopen(const char * restrict filename, const char * restrict mode, FILE * restrict stream);
void clearerr(FILE *stream);
size_t fread(void * restrict ptr, size_t size, size_t nmemb, FILE * restrict stream);
size_t fwrite(const void * restrict ptr, size_t size, size_t nmemb, FILE * restrict stream);
int rename(const char *from, const char *to);
int sprintf(char * restrict s, const char * restrict format, ...);
int vsprintf(char * restrict s, const char * restrict format, va_list arg);
int snprintf(char * restrict s, size_t n, const char * restrict format, ...);
int vsnprintf(char * restrict s, size_t n, const char * restrict format, va_list arg);
int scanf(const char * restrict format, ...);
int vscanf(const char * restrict format, va_list arg);
int fscanf(FILE * restrict stream, const char * restrict format, ...);
int vfscanf(FILE * restrict stream, const char * restrict format, va_list arg);
int sscanf(const char * restrict s, const char * restrict format, ...);
int vsscanf(const char * restrict s, const char * restrict format, va_list arg);
int fseek(FILE *stream, long int offset, int whence);
long int ftell(FILE *stream);
void rewind(FILE *stream);
int fgetpos(FILE * restrict stream, fpos_t * restrict pos);
int fsetpos(FILE *stream, const fpos_t *pos);
int remove(const char *filename);
void setbuf(FILE * restrict stream, char * restrict buf);
void setlinebuf(FILE *stream);
int setvbuf(FILE * restrict stream, char * restrict buf, int mode, size_t size);
int __vcformat(void * data, int (*outc)(int, void *), const char * format, va_list args);
int __vcscan(void * data, int (*getc)(void *), int (*ungetc)(int, void *), const char * format, va_list args);
int access(const char *path, int mode);
int close(int fd);
int dup(int oldfd);
int dup2(int oldfd, int newfd);
char * getcwd(char *buf, size_t size);
int isatty(int fd);
off_t lseek(int filedes, off_t offset, int whence);
ssize_t read(int d, void *buf, size_t nbytes);
int truncate(const char *path, off_t length);
int unlink(const char *path);
ssize_t write(int fd, const void *buf, size_t nbytes);
int open(const char * filename, int flags, ...);
int creat(const char * filename, int mode);
int utime(const char *filename, const struct utimbuf *buf);
int abs(int j);
long labs(long j);
double atof(const char *nptr);
int atoi(const char *nptr);
long int atol(const char *nptr);
long int strtol(const char * restrict nptr, char ** restrict endptr, int base);
unsigned long int strtoul(const char * restrict nptr, char ** restrict endptr, int base);
double strtod(const char * restrict nptr, char ** restrict endptr);
int rand(void);
void srand(unsigned int seed);
double drand48(void);
double erand48(unsigned short int xsubi[3]);
long int lrand48(void);
long int nrand48(unsigned short int xsubi[3]);
long int mrand48(void);
long int jrand48(unsigned short int xsubi[3]);
void srand48(long int seedval);
unsigned short int * seed48(unsigned short int seed16v[3]);
void lcong48(unsigned short int param[7]);
long random(void);
void srandom(unsigned seed);
char * initstate(unsigned seed, char *state, int n);
char * setstate(char *state);
void qsort(void * array, size_t count, size_t elementsize, int (*comparefunction)(const void * element1, const void * element2));
void * bsearch(const void * key, const void * base, size_t count, size_t size, int (*comparefunction)(const void *, const void *));
void * malloc(size_t size);
void * calloc(size_t count, size_t size);
void * realloc(void *oldmem, size_t newsize);
void free(void *memory);
char * getenv(const char *name);
int setenv(const char *name, const char *value, int overwrite);
int putenv(const char *string);
char * mktemp(char *);
int system(const char *string);
char * gcvt(double number, int ndigit, char *buf);
int mkdir(const char *path, mode_t mode);
int stat(const char * restrict path, struct stat * restrict sb);
int fstat(int fd, struct stat *sb);
DIR * opendir(const char *filename);
struct dirent * readdir(DIR *dir);
void rewinddir(DIR *dir);
int closedir(DIR *dir);
long telldir(DIR *dir);
void seekdir(DIR *dir, off_t loc);
int gettimeofday(struct timeval * tv, struct timezone * tz);
int utimes(const char *file, const struct timeval tvp[2]);
size_t strftime(char *, size_t, const char *, const struct tm *);
char * strerror(int n);
void exit(int code);
void abort(void);
int chmod(const char *path, mode_t mode);
struct tm * localtime(const time_t *);
time_t time(time_t *);
struct tm * gmtime(const time_t *);
void perror(const char *s);
char * ctime(const time_t *);
clock_t clock(void);
int __progonly_atexit(void (*func)(void));
int pipe(int filedes[2]);
int fcntl(int fd, int cmd, ...);
double difftime(time_t, time_t);
int ftruncate(int fd, off_t length);
mode_t umask(mode_t numask);
int unsetenv(const char *name);
char * asctime(const struct tm *);
unsigned sleep(unsigned);
int chdir(const char *path);
int __stdc_ioerr2errno(int ioerr);
time_t mktime(struct tm *);
div_t div(int numer, int denom);
ldiv_t ldiv(long int numer, long int denom);
int sigaction(int, const struct sigaction *, struct sigaction *);
int sigaddset(sigset_t *, int);
int sigemptyset(sigset_t *);
int fchmod(int fildes, mode_t mode);
int sigdelset(sigset_t *, int);
int sigfillset(sigset_t *);
int sigismember(const sigset_t *, int);
int sigpending(sigset_t *);
int sigprocmask(int, const sigset_t *, sigset_t *);
int sigsuspend(const sigset_t *);
char * tmpnam(char *s);
void updatestdio(void);
void __assert(const char *, const char *, unsigned int);
lldiv_t lldiv(long long int numer, long long int denom);
long long int strtoll(const char * restrict nptr, char ** restrict endptr, int base);
unsigned long long int strtoull(const char * restrict nptr, char ** restrict endptr, int base);
int __progonly_on_exit(void (*func)(int, void *), void *);
pid_t getpid(void);
FILE * popen(const char *, const char *);
int stcd_l(const char * in, long * lvalue);
int stch_l(const char * in, long * lvalue);
int stcl_d(char * out, long lvalue);
int stcl_h(char * out, long lvalue);
int stcl_o(char * out, long lvalue);
int stco_l(const char * in, long * lvalue);
void * realloc_nocopy(void *oldmem, size_t newsize);
int getfsstat(struct statfs *, long, int);
__sighandler_t * signal(int, __sighandler_t *);
void _exit(int);
int kill(pid_t, int);
char * setlocale(int category, const char *locale);
int pclose(FILE *);
int mkstemp(char *);
int execvp(const char *path, char *const argv[]);
int rmdir(const char *path);
uid_t getuid(void);
uid_t geteuid(void);
struct arosc_userdata * __get_arosc_userdata(void);
AROS_LD3(int, __arosc_nixmain_abiv0,
         AROS_LDA(void *, main, A0),
         AROS_LDA(int, argc, D0),
         AROS_LDA(char **, argv, A1),
         LIBBASETYPEPTR, aroscbase, 173, Arosc
);
int spawnv(int mode, const char *path, char *const argv[]);
int chown(const char *path, uid_t owner, gid_t group);
int spawnvp(int mode, const char *command, char *const argv[]);
ssize_t readlink(const char * restrict path, char * restrict buf, size_t bufsize);
struct passwd * getpwuid(uid_t uid);
struct group * getgrgid(gid_t gid);
gid_t getgid(void);
gid_t getegid(void);
struct passwd * getpwnam(const char *name);
void setpwent(void);
struct passwd * getpwent(void);
void endpwent(void);
int getgroups(int gidsetlen, gid_t *gidset);
struct group * getgrnam(const char *name);
void endgrent(void);
struct group * getgrent(void);
void setgrent(void);
int fchown(int fd, uid_t owner, gid_t group);
int fsync(int fd);
int link(const char *name1, const char *name2);
int symlink(const char *name1, const char *name2);
int mknod(const char *path, mode_t mode, dev_t dev);
pid_t getppid(void);
int execl(const char *path, const char *arg, ...);
pid_t getpgrp(void);
int ioctl(int fd, int request, ...);
pid_t wait(int *status);
int setuid(uid_t uid);
int setgid(gid_t gid);
int getw_abiv0(FILE *stream);
int putw_abiv0(int word, FILE *stream);
int getloadavg(double loadavg[], int n);
int raise(int);
clock_t times(struct tms *buffer);
int ftime(struct timeb *tb);
char * strsep(char **, const char *);
char * tempnam(const char *dir, const char *pfx);
FILE * tmpfile(void);
int posix_memalign(void **memptr, size_t alignment, size_t size);
struct tm * localtime_r(const time_t *, struct tm *);
struct tm * gmtime_r(const time_t *, struct tm *);
char * ctime_r(const time_t *, char *);
char * asctime_r(const struct tm *, char *);
char * basename(char *);
int dirfd(DIR *dir);
char * dirname(char *);
struct lconv * localeconv(void);
size_t mbrtowc(int * restrict pwc, const char * restrict s, size_t n, mbstate_t * restrict ps);
int regcomp(regex_t * __restrict, const char * __restrict, int);
size_t regerror(int, const regex_t * __restrict, char * __restrict, size_t);
int regexec(const regex_t * __restrict, const char * __restrict, size_t, regmatch_t * __restrict, int);
void regfree(regex_t *);
int putc(int c, FILE *stream);
int getc(FILE * stream);
int getchar(void);
char * gets(char *s);
int __get_default_file(int file_descriptor, long* file_handle);
int statfs(const char *path, struct statfs *buf);
int usleep(useconds_t usec);
char * ttyname(int fd);
char * getlogin(void);
int fchdir(int fd);
int uname(struct utsname *name);
int __env_get_environ(char **environ, int size);
int execve(const char *filename, char *const argv[], char *const envp[]);
char * strptime(const char *s, const char *format, struct tm *tm);
pid_t vfork(void);
pid_t waitpid(pid_t pid, int *status, int options);
int execv(const char *path, char *const argv[]);
int execlp(const char *file, const char *arg, ...);
int flock(int fd, int operation);
intmax_t strtoimax(const char *nptr, char **endptr, int base);
uintmax_t strtoumax(const char *nptr, char **endptr, int base);
void sharecontextwithchild(int share);
long sysconf(int name);
int lstat(const char *path, struct stat *sb);
void __arosc_program_startup_abiv0(void);
void __arosc_program_end(void);
int nanosleep(const struct timespec *req, struct timespec *rem);
int fseeko(FILE *stream, off_t offset, int whence);
off_t ftello(FILE *stream);
long pathconf(const char *path, int name);
char * realpath(const char *path, char *resolved_path);
void sync();
int tcgetattr(int __fd, struct termios *__termios_p);
int tcsetattr(int __fd, int __optional_actions, const struct termios *__termios_p);
speed_t cfgetispeed(const struct termios *__termios_p);
speed_t cfgetospeed(const struct termios *__termios_p);
int cfsetispeed(struct termios *__termios_p, speed_t speed);
int cfsetospeed(struct termios *__termios_p, speed_t speed);
const struct arosc_ctype * __get_arosc_ctype(void);
int getrlimit(int resource, struct rlimit *rlp);
int setrlimit(int resource, const struct rlimit *rlp);
int __posixc_set_environptr(char ***environptr);
int ___gmtoffset(void);
int * __arosc_set_errorptr(int *errorptr);
void __arosc_set_exitjmp(jmp_buf exitjmp, jmp_buf previousjmp);
void __arosc_jmp2exit(int normal, int retcode);
int * __arosc_get_errorptr(void);
char *** __posixc_get_environptr(void);
void __arosc_program_startup(jmp_buf exitjmp, int *error_ptr);
int __posixc_nixmain(int (*main)(int argc, char *argv[]), int argc, char *argv[]);
int clock_gettime(clockid_t, struct timespec *);
FILE * fopen64(const char *restrict, const char *restrict);
int fseeko64(FILE *, off64_t, int);
off64_t ftello64(FILE *);
int fstat64(int fd, struct stat64 *sb);
off64_t lseek64(int filedes, off64_t offset, int whence);
AROS_GM_STACKCALL(fopen,Arosc,5);
AROS_GM_STACKCALL(fdopen,Arosc,6);
AROS_GM_STACKCALL(fclose,Arosc,7);
AROS_GM_STACKCALL(printf,Arosc,8);
AROS_GM_STACKCALL(vprintf,Arosc,9);
AROS_GM_STACKCALL(fprintf,Arosc,10);
AROS_GM_STACKCALL(vfprintf,Arosc,11);
AROS_GM_STACKCALL(fputc,Arosc,12);
AROS_GM_STACKCALL(fputs,Arosc,13);
AROS_GM_STACKCALL(puts,Arosc,14);
AROS_GM_STACKCALL(fflush,Arosc,15);
AROS_GM_STACKCALL(fgetc,Arosc,16);
AROS_GM_STACKCALL(ungetc,Arosc,17);
AROS_GM_STACKCALL(fgets,Arosc,18);
AROS_GM_STACKCALL(feof,Arosc,19);
AROS_GM_STACKCALL(ferror,Arosc,20);
AROS_GM_STACKCALL(fileno,Arosc,21);
AROS_GM_STACKCALL(freopen,Arosc,22);
AROS_GM_STACKCALL(clearerr,Arosc,23);
AROS_GM_STACKCALL(fread,Arosc,24);
AROS_GM_STACKCALL(fwrite,Arosc,25);
AROS_GM_STACKCALL(rename,Arosc,26);
AROS_GM_STACKCALL(sprintf,Arosc,27);
AROS_GM_STACKCALL(vsprintf,Arosc,28);
AROS_GM_STACKCALL(snprintf,Arosc,29);
AROS_GM_STACKCALL(vsnprintf,Arosc,30);
AROS_GM_STACKCALL(scanf,Arosc,31);
AROS_GM_STACKCALL(vscanf,Arosc,32);
AROS_GM_STACKCALL(fscanf,Arosc,33);
AROS_GM_STACKCALL(vfscanf,Arosc,34);
AROS_GM_STACKCALL(sscanf,Arosc,35);
AROS_GM_STACKCALL(vsscanf,Arosc,36);
AROS_GM_STACKCALL(fseek,Arosc,37);
AROS_GM_STACKCALL(ftell,Arosc,38);
AROS_GM_STACKCALL(rewind,Arosc,39);
AROS_GM_STACKCALL(fgetpos,Arosc,40);
AROS_GM_STACKCALL(fsetpos,Arosc,41);
AROS_GM_STACKCALL(remove,Arosc,42);
AROS_GM_STACKCALL(setbuf,Arosc,43);
AROS_GM_STACKCALL(setlinebuf,Arosc,44);
AROS_GM_STACKCALL(setvbuf,Arosc,45);
AROS_GM_STACKCALL(__vcformat,Arosc,46);
AROS_GM_STACKCALL(__vcscan,Arosc,47);
AROS_GM_STACKCALL(access,Arosc,48);
AROS_GM_STACKCALL(close,Arosc,49);
AROS_GM_STACKCALL(dup,Arosc,50);
AROS_GM_STACKCALL(dup2,Arosc,51);
AROS_GM_STACKCALL(getcwd,Arosc,52);
AROS_GM_STACKCALL(isatty,Arosc,53);
AROS_GM_STACKCALL(lseek,Arosc,54);
AROS_GM_STACKCALL(read,Arosc,55);
AROS_GM_STACKCALL(truncate,Arosc,56);
AROS_GM_STACKCALL(unlink,Arosc,57);
AROS_GM_STACKCALL(write,Arosc,58);
AROS_GM_STACKCALL(open,Arosc,59);
AROS_GM_STACKCALL(creat,Arosc,60);
AROS_GM_STACKCALL(utime,Arosc,61);
AROS_GM_STACKCALL(abs,Arosc,62);
AROS_GM_STACKCALL(labs,Arosc,63);
AROS_GM_STACKCALL(atof,Arosc,64);
AROS_GM_STACKCALL(atoi,Arosc,65);
AROS_GM_STACKCALL(atol,Arosc,66);
AROS_GM_STACKCALL(strtol,Arosc,67);
AROS_GM_STACKCALL(strtoul,Arosc,68);
AROS_GM_STACKCALL(strtod,Arosc,69);
AROS_GM_STACKCALL(rand,Arosc,70);
AROS_GM_STACKCALL(srand,Arosc,71);
AROS_GM_STACKCALL(drand48,Arosc,72);
AROS_GM_STACKCALL(erand48,Arosc,73);
AROS_GM_STACKCALL(lrand48,Arosc,74);
AROS_GM_STACKCALL(nrand48,Arosc,75);
AROS_GM_STACKCALL(mrand48,Arosc,76);
AROS_GM_STACKCALL(jrand48,Arosc,77);
AROS_GM_STACKCALL(srand48,Arosc,78);
AROS_GM_STACKCALL(seed48,Arosc,79);
AROS_GM_STACKCALL(lcong48,Arosc,80);
AROS_GM_STACKCALL(random,Arosc,81);
AROS_GM_STACKCALL(srandom,Arosc,82);
AROS_GM_STACKCALL(initstate,Arosc,83);
AROS_GM_STACKCALL(setstate,Arosc,84);
AROS_GM_STACKCALL(qsort,Arosc,85);
AROS_GM_STACKCALL(bsearch,Arosc,86);
AROS_GM_STACKCALL(malloc,Arosc,87);
AROS_GM_STACKCALL(calloc,Arosc,88);
AROS_GM_STACKCALL(realloc,Arosc,89);
AROS_GM_STACKCALL(free,Arosc,90);
AROS_GM_STACKCALL(getenv,Arosc,91);
AROS_GM_STACKCALL(setenv,Arosc,92);
AROS_GM_STACKCALL(putenv,Arosc,93);
AROS_GM_STACKCALL(mktemp,Arosc,94);
AROS_GM_STACKCALL(system,Arosc,95);
AROS_GM_STACKCALL(gcvt,Arosc,96);
AROS_GM_STACKCALL(mkdir,Arosc,97);
AROS_GM_STACKCALL(stat,Arosc,98);
AROS_GM_STACKCALL(fstat,Arosc,99);
AROS_GM_STACKCALL(opendir,Arosc,100);
AROS_GM_STACKCALL(readdir,Arosc,101);
AROS_GM_STACKCALL(rewinddir,Arosc,102);
AROS_GM_STACKCALL(closedir,Arosc,103);
AROS_GM_STACKCALL(telldir,Arosc,104);
AROS_GM_STACKCALL(seekdir,Arosc,105);
AROS_GM_STACKCALL(gettimeofday,Arosc,106);
AROS_GM_STACKCALL(utimes,Arosc,107);
AROS_GM_STACKCALL(strftime,Arosc,110);
AROS_GM_STACKCALL(strerror,Arosc,111);
AROS_GM_STACKCALL(exit,Arosc,112);
AROS_GM_STACKCALL(abort,Arosc,113);
AROS_GM_STACKCALL(chmod,Arosc,114);
AROS_GM_STACKCALL(localtime,Arosc,115);
AROS_GM_STACKCALL(time,Arosc,116);
AROS_GM_STACKCALL(gmtime,Arosc,117);
AROS_GM_STACKCALL(perror,Arosc,118);
AROS_GM_STACKCALL(ctime,Arosc,119);
AROS_GM_STACKCALL(clock,Arosc,120);
AROS_GM_STACKCALL(__progonly_atexit,Arosc,121);
AROS_GM_STACKCALL(pipe,Arosc,122);
AROS_GM_STACKCALL(fcntl,Arosc,123);
AROS_GM_STACKCALL(difftime,Arosc,124);
AROS_GM_STACKCALL(ftruncate,Arosc,125);
AROS_GM_STACKCALL(umask,Arosc,126);
AROS_GM_STACKCALL(unsetenv,Arosc,127);
AROS_GM_STACKCALL(asctime,Arosc,128);
AROS_GM_STACKCALL(sleep,Arosc,129);
AROS_GM_STACKCALL(chdir,Arosc,130);
AROS_GM_STACKCALL(__stdc_ioerr2errno,Arosc,131);
AROS_GM_STACKCALL(mktime,Arosc,132);
AROS_GM_STACKCALL(div,Arosc,133);
AROS_GM_STACKCALL(ldiv,Arosc,134);
AROS_GM_STACKCALL(sigaction,Arosc,135);
AROS_GM_STACKCALL(sigaddset,Arosc,136);
AROS_GM_STACKCALL(sigemptyset,Arosc,137);
AROS_GM_STACKCALL(fchmod,Arosc,138);
AROS_GM_STACKCALL(sigdelset,Arosc,139);
AROS_GM_STACKCALL(sigfillset,Arosc,140);
AROS_GM_STACKCALL(sigismember,Arosc,141);
AROS_GM_STACKCALL(sigpending,Arosc,142);
AROS_GM_STACKCALL(sigprocmask,Arosc,143);
AROS_GM_STACKCALL(sigsuspend,Arosc,144);
AROS_GM_STACKCALL(tmpnam,Arosc,145);
AROS_GM_STACKCALL(updatestdio,Arosc,146);
AROS_GM_STACKCALL(__assert,Arosc,147);
AROS_GM_STACKCALL(lldiv,Arosc,148);
AROS_GM_STACKCALL(strtoll,Arosc,149);
AROS_GM_STACKCALL(strtoull,Arosc,150);
AROS_GM_STACKCALL(__progonly_on_exit,Arosc,151);
AROS_GM_STACKCALL(getpid,Arosc,152);
AROS_GM_STACKCALL(popen,Arosc,153);
AROS_GM_STACKCALL(stcd_l,Arosc,154);
AROS_GM_STACKCALL(stch_l,Arosc,155);
AROS_GM_STACKCALL(stcl_d,Arosc,156);
AROS_GM_STACKCALL(stcl_h,Arosc,157);
AROS_GM_STACKCALL(stcl_o,Arosc,158);
AROS_GM_STACKCALL(stco_l,Arosc,159);
AROS_GM_STACKCALL(realloc_nocopy,Arosc,160);
AROS_GM_STACKCALL(getfsstat,Arosc,161);
AROS_GM_STACKCALL(signal,Arosc,162);
AROS_GM_STACKCALL(_exit,Arosc,163);
AROS_GM_STACKCALL(kill,Arosc,164);
AROS_GM_STACKCALL(setlocale,Arosc,165);
AROS_GM_STACKCALL(pclose,Arosc,166);
AROS_GM_STACKCALL(mkstemp,Arosc,167);
AROS_GM_STACKCALL(execvp,Arosc,168);
AROS_GM_STACKCALL(rmdir,Arosc,169);
AROS_GM_STACKCALL(getuid,Arosc,170);
AROS_GM_STACKCALL(geteuid,Arosc,171);
AROS_GM_STACKCALL(__get_arosc_userdata,Arosc,172);
AROS_GM_STACKCALL(spawnv,Arosc,174);
AROS_GM_STACKCALL(chown,Arosc,175);
AROS_GM_STACKCALL(spawnvp,Arosc,176);
AROS_GM_STACKCALL(readlink,Arosc,177);
AROS_GM_STACKCALL(getpwuid,Arosc,178);
AROS_GM_STACKCALL(getgrgid,Arosc,179);
AROS_GM_STACKCALL(getgid,Arosc,180);
AROS_GM_STACKCALL(getegid,Arosc,181);
AROS_GM_STACKCALL(getpwnam,Arosc,182);
AROS_GM_STACKCALL(setpwent,Arosc,183);
AROS_GM_STACKCALL(getpwent,Arosc,184);
AROS_GM_STACKCALL(endpwent,Arosc,185);
AROS_GM_STACKCALL(getgroups,Arosc,186);
AROS_GM_STACKCALL(getgrnam,Arosc,187);
AROS_GM_STACKCALL(endgrent,Arosc,188);
AROS_GM_STACKCALL(getgrent,Arosc,189);
AROS_GM_STACKCALL(setgrent,Arosc,190);
AROS_GM_STACKCALL(fchown,Arosc,191);
AROS_GM_STACKCALL(fsync,Arosc,192);
AROS_GM_STACKCALL(link,Arosc,193);
AROS_GM_STACKCALL(symlink,Arosc,194);
AROS_GM_STACKCALL(mknod,Arosc,195);
AROS_GM_STACKCALL(getppid,Arosc,196);
AROS_GM_STACKCALL(execl,Arosc,197);
AROS_GM_STACKCALL(getpgrp,Arosc,198);
AROS_GM_STACKCALL(ioctl,Arosc,199);
AROS_GM_STACKCALL(wait,Arosc,200);
AROS_GM_STACKCALL(setuid,Arosc,201);
AROS_GM_STACKCALL(setgid,Arosc,202);
AROS_GM_STACKCALL(getw_abiv0,Arosc,203);
AROS_GM_STACKCALL(putw_abiv0,Arosc,204);
AROS_GM_STACKCALL(getloadavg,Arosc,205);
AROS_GM_STACKCALL(raise,Arosc,206);
AROS_GM_STACKCALL(times,Arosc,207);
AROS_GM_STACKCALL(ftime,Arosc,208);
AROS_GM_STACKCALL(strsep,Arosc,209);
AROS_GM_STACKCALL(tempnam,Arosc,210);
AROS_GM_STACKCALL(tmpfile,Arosc,211);
AROS_GM_STACKCALL(posix_memalign,Arosc,212);
AROS_GM_STACKCALL(localtime_r,Arosc,213);
AROS_GM_STACKCALL(gmtime_r,Arosc,214);
AROS_GM_STACKCALL(ctime_r,Arosc,215);
AROS_GM_STACKCALL(asctime_r,Arosc,216);
AROS_GM_STACKCALL(basename,Arosc,217);
AROS_GM_STACKCALL(dirfd,Arosc,218);
AROS_GM_STACKCALL(dirname,Arosc,219);
AROS_GM_STACKCALL(localeconv,Arosc,220);
AROS_GM_STACKCALL(mbrtowc,Arosc,238);
AROS_GM_STACKCALL(regcomp,Arosc,240);
AROS_GM_STACKCALL(regerror,Arosc,241);
AROS_GM_STACKCALL(regexec,Arosc,242);
AROS_GM_STACKCALL(regfree,Arosc,243);
AROS_GM_STACKCALL(putc,Arosc,244);
AROS_GM_STACKCALL(getc,Arosc,245);
AROS_GM_STACKCALL(getchar,Arosc,246);
AROS_GM_STACKCALL(gets,Arosc,247);
AROS_GM_STACKCALL(__get_default_file,Arosc,250);
AROS_GM_STACKCALL(statfs,Arosc,251);
AROS_GM_STACKCALL(usleep,Arosc,252);
AROS_GM_STACKCALL(ttyname,Arosc,253);
AROS_GM_STACKCALL(getlogin,Arosc,254);
AROS_GM_STACKCALL(fchdir,Arosc,255);
AROS_GM_STACKCALL(uname,Arosc,256);
AROS_GM_STACKCALL(__env_get_environ,Arosc,257);
AROS_GM_STACKCALL(execve,Arosc,258);
AROS_GM_STACKCALL(strptime,Arosc,259);
AROS_GM_STACKCALL(vfork,Arosc,260);
AROS_GM_STACKCALL(waitpid,Arosc,261);
AROS_GM_STACKCALL(execv,Arosc,262);
AROS_GM_STACKCALL(execlp,Arosc,263);
AROS_GM_STACKCALL(flock,Arosc,264);
AROS_GM_STACKCALL(strtoimax,Arosc,265);
AROS_GM_STACKCALL(strtoumax,Arosc,266);
AROS_GM_STACKCALL(sharecontextwithchild,Arosc,267);
AROS_GM_STACKCALL(sysconf,Arosc,268);
AROS_GM_STACKCALL(lstat,Arosc,269);
AROS_GM_STACKCALL(__arosc_program_startup_abiv0,Arosc,270);
AROS_GM_STACKCALL(__arosc_program_end,Arosc,271);
AROS_GM_STACKCALL(nanosleep,Arosc,272);
AROS_GM_STACKCALL(fseeko,Arosc,273);
AROS_GM_STACKCALL(ftello,Arosc,274);
AROS_GM_STACKCALL(pathconf,Arosc,275);
AROS_GM_STACKCALL(realpath,Arosc,276);
AROS_GM_STACKCALL(sync,Arosc,277);
AROS_GM_STACKCALL(tcgetattr,Arosc,278);
AROS_GM_STACKCALL(tcsetattr,Arosc,279);
AROS_GM_STACKCALL(cfgetispeed,Arosc,280);
AROS_GM_STACKCALL(cfgetospeed,Arosc,281);
AROS_GM_STACKCALL(cfsetispeed,Arosc,282);
AROS_GM_STACKCALL(cfsetospeed,Arosc,283);
AROS_GM_STACKCALL(__get_arosc_ctype,Arosc,284);
AROS_GM_STACKCALL(getrlimit,Arosc,285);
AROS_GM_STACKCALL(setrlimit,Arosc,286);
AROS_GM_STACKCALL(__posixc_set_environptr,Arosc,287);
AROS_GM_STACKCALL(___gmtoffset,Arosc,288);
AROS_GM_STACKCALL(__arosc_set_errorptr,Arosc,289);
AROS_GM_STACKCALL(__arosc_set_exitjmp,Arosc,290);
AROS_GM_STACKCALL(__arosc_jmp2exit,Arosc,291);
AROS_GM_STACKCALL(__arosc_get_errorptr,Arosc,292);
AROS_GM_STACKCALL(__posixc_get_environptr,Arosc,293);
AROS_GM_STACKCALL(__arosc_program_startup,Arosc,294);
AROS_GM_STACKCALL(__posixc_nixmain,Arosc,295);
AROS_GM_STACKCALL(clock_gettime,Arosc,296);
AROS_GM_STACKCALL(fopen64,Arosc,303);
AROS_GM_STACKCALL(fseeko64,Arosc,304);
AROS_GM_STACKCALL(ftello64,Arosc,306);
AROS_GM_STACKCALL(fstat64,Arosc,307);
AROS_GM_STACKCALL(lseek64,Arosc,310);

extern int GM_UNIQUENAME(End)(void);
extern const APTR GM_UNIQUENAME(FuncTable)[];
struct InitTable
{
    IPTR              Size;
    const APTR       *FuncTable;
    struct DataTable *DataTable;
    APTR              InitLibTable;
};
static const struct InitTable GM_UNIQUENAME(InitTable);

extern const char GM_UNIQUENAME(LibName)[];
extern const char GM_UNIQUENAME(LibID)[];
extern const char GM_UNIQUENAME(Copyright)[];

#define __freebase(LIBBASE)\
do {\
    UWORD negsize, possize;\
    UBYTE *negptr = (UBYTE *)LIBBASE;\
    negsize = ((struct Library *)LIBBASE)->lib_NegSize;\
    negptr -= negsize;\
    possize = ((struct Library *)LIBBASE)->lib_PosSize;\
    FreeMem (negptr, negsize+possize);\
} while(0)

AROS_UFP3 (LIBBASETYPEPTR, GM_UNIQUENAME(InitLib),
    AROS_UFPA(LIBBASETYPEPTR, LIBBASE, D0),
    AROS_UFPA(BPTR, segList, A0),
    AROS_UFPA(struct ExecBase *, sysBase, A6)
);
AROS_LD1(BPTR, GM_UNIQUENAME(ExpungeLib),
    AROS_LDA(LIBBASETYPEPTR, extralh, D0),
    LIBBASETYPEPTR, LIBBASE, 3, Arosc
);

__section(".text.romtag") struct Resident const GM_UNIQUENAME(ROMTag) =
{
    RTC_MATCHWORD,
    (struct Resident *)&GM_UNIQUENAME(ROMTag),
    (APTR)&GM_UNIQUENAME(End),
    RESIDENTFLAGS,
    VERSION_NUMBER,
    NT_LIBRARY,
    RESIDENTPRI,
    (CONST_STRPTR)&GM_UNIQUENAME(LibName)[0],
    (CONST_STRPTR)&GM_UNIQUENAME(LibID)[6],
    (APTR)&GM_UNIQUENAME(InitTable)
};

__section(".text.romtag") static struct InitTable const GM_UNIQUENAME(InitTable) =
{
    LIBBASESIZE,
    &GM_UNIQUENAME(FuncTable)[0],
    NULL,
    (APTR)GM_UNIQUENAME(InitLib)
};

#if defined(MOD_NAME_STRING)
__section(".text.romtag") const char GM_UNIQUENAME(LibName)[] = MOD_NAME_STRING;
#endif
#if defined(VERSION_STRING)
__section(".text.romtag") const char GM_UNIQUENAME(LibID)[] = VERSION_STRING;
#endif
#if defined(COPYRIGHT_STRING)
__section(".text.romtag") const char GM_UNIQUENAME(Copyright)[] = COPYRIGHT_STRING;
#endif

THIS_PROGRAM_HANDLES_SYMBOLSET(INIT)
THIS_PROGRAM_HANDLES_SYMBOLSET(EXIT)
DECLARESET(INIT)
DECLARESET(EXIT)
THIS_PROGRAM_HANDLES_SYMBOLSET(PROGRAM_ENTRIES)
DECLARESET(PROGRAM_ENTRIES)
THIS_PROGRAM_HANDLES_SYMBOLSET(CTORS)
THIS_PROGRAM_HANDLES_SYMBOLSET(DTORS)
DECLARESET(CTORS)
DECLARESET(DTORS)
THIS_PROGRAM_HANDLES_SYMBOLSET(INIT_ARRAY)
THIS_PROGRAM_HANDLES_SYMBOLSET(FINI_ARRAY)
DECLARESET(INIT_ARRAY)
DECLARESET(FINI_ARRAY)
THIS_PROGRAM_HANDLES_SYMBOLSET(INITLIB)
THIS_PROGRAM_HANDLES_SYMBOLSET(EXPUNGELIB)
DECLARESET(INITLIB)
DECLARESET(EXPUNGELIB)
THIS_PROGRAM_HANDLES_SYMBOLSET(LIBS)
DECLARESET(LIBS)
THIS_PROGRAM_HANDLES_SYMBOLSET(OPENLIB)
THIS_PROGRAM_HANDLES_SYMBOLSET(CLOSELIB)
DECLARESET(OPENLIB)
DECLARESET(CLOSELIB)

extern const LONG __aros_libreq_SysBase __attribute__((weak));

AROS_UFH3 (LIBBASETYPEPTR, GM_UNIQUENAME(InitLib),
    AROS_UFHA(LIBBASETYPEPTR, LIBBASE, D0),
    AROS_UFHA(BPTR, segList, A0),
    AROS_UFHA(struct ExecBase *, sysBase, A6)
)
{
    AROS_USERFUNC_INIT

    int ok;
    int initcalled = 0;
    SysBase = sysBase;

#ifdef GM_SYSBASE_FIELD
    GM_SYSBASE_FIELD(LIBBASE) = (APTR)SysBase;
#endif
    if (!SysBase || SysBase->LibNode.lib_Version < __aros_libreq_SysBase)
        return NULL;

#ifdef GM_OOPBASE_FIELD
    GM_OOPBASE_FIELD(LIBBASE) = OpenLibrary("oop.library",0);
    if (GM_OOPBASE_FIELD(LIBBASE) == NULL)
        return NULL;
#endif
#if defined(REVISION_NUMBER)
    ((struct Library *)LIBBASE)->lib_Revision = REVISION_NUMBER;
#endif
    __GM_BaseSlot = AllocTaskStorageSlot();
    if (!SetTaskStorageSlot(__GM_BaseSlot, (IPTR)LIBBASE)) {
        FreeTaskStorageSlot(__GM_BaseSlot);
        return NULL;
    }
    __pertaskslot = AllocTaskStorageSlot();
    GM_SEGLIST_FIELD(LIBBASE) = segList;
    GM_ROOTBASE_FIELD(LIBBASE) = (LIBBASETYPEPTR)LIBBASE;
    if (set_open_libraries() && set_call_funcs(SETNAME(INIT), 1, 1) &&1)
    {
        set_call_funcs(SETNAME(CTORS), -1, 0);
        set_call_funcs(SETNAME(INIT_ARRAY), 1, 0);

        initcalled = 1;
        ok = set_call_libfuncs(SETNAME(INITLIB), 1, 1, LIBBASE);
    }
    else
        ok = 0;

    if (!ok)
    {
        if (initcalled)
            set_call_libfuncs(SETNAME(EXPUNGELIB), -1, 0, LIBBASE);
        set_call_funcs(SETNAME(FINI_ARRAY), -1, 0);
        set_call_funcs(SETNAME(DTORS), 1, 0);
        set_call_funcs(SETNAME(EXIT), -1, 0);
        set_close_libraries();

        __freebase(LIBBASE);
        return NULL;
    }
    else
    {
        return  LIBBASE;
    }

    AROS_USERFUNC_EXIT
}

AROS_LH1 (LIBBASETYPEPTR, GM_UNIQUENAME(OpenLib),
    AROS_LHA (ULONG, version, D0),
    LIBBASETYPEPTR, LIBBASE, 1, Arosc
)
{
    AROS_LIBFUNC_INIT

    struct Library *newlib = NULL;
    UWORD possize = ((struct Library *)LIBBASE)->lib_PosSize;
    LIBBASETYPEPTR oldbase = (LIBBASETYPEPTR)__aros_getbase_aroscbase();
    struct Task *thistask = FindTask(NULL);
    LIBBASETYPEPTR oldpertaskbase = __GM_GetPerTaskBase();
    if (!oldpertaskbase)
       oldpertaskbase = __GM_GetParentPerTaskBase();
    newlib = (struct Library *)oldpertaskbase;
    if (newlib)
    {
        struct __GM_DupBase *dupbase = (struct __GM_DupBase *)newlib;
        if (dupbase->task != thistask)
            newlib = NULL;
        else if (thistask->tc_Node.ln_Type == NT_PROCESS
                 && dupbase->retaddr != ((struct Process *)thistask)->pr_ReturnAddr
        )
            newlib = NULL;
        else
            dupbase->taskopencount++;
    }

    if (newlib == NULL)
    {
        newlib = MakeLibrary(GM_UNIQUENAME(InitTable).FuncTable,
                             GM_UNIQUENAME(InitTable).DataTable,
                             NULL,
                             GM_UNIQUENAME(InitTable).Size,
                             (BPTR)NULL
        );
        if (newlib == NULL)
            return NULL;

        CopyMem(LIBBASE, newlib, possize);
        struct __GM_DupBase *dupbase = (struct __GM_DupBase *)newlib;
        dupbase->oldbase = oldbase;
        __aros_setoffsettable((char *)newlib);
        dupbase->task = thistask;
        if (thistask->tc_Node.ln_Type == NT_PROCESS)
             dupbase->retaddr = ((struct Process *)thistask)->pr_ReturnAddr;
        dupbase->oldpertaskbase = oldpertaskbase;
        dupbase->taskopencount = 1;
        __GM_SetPerTaskBase((LIBBASETYPEPTR)newlib);

        if (!(set_open_rellibraries(newlib)
              && set_call_libfuncs(SETNAME(OPENLIB), 1, 1, newlib)
             )
        )
        {
            __GM_SetPerTaskBase(oldpertaskbase);
            __freebase(newlib);
            return NULL;
        }

        ((struct Library *)LIBBASE)->lib_OpenCnt++;
        ((struct Library *)LIBBASE)->lib_Flags &= ~LIBF_DELEXP;
    }

    return (LIBBASETYPEPTR)newlib;

    AROS_LIBFUNC_EXIT
}

AROS_LH0 (BPTR, GM_UNIQUENAME(CloseLib),
    LIBBASETYPEPTR, LIBBASE, 2, Arosc
)
{
    AROS_LIBFUNC_INIT

    LIBBASETYPEPTR rootbase = GM_ROOTBASE_FIELD(LIBBASE);
    struct __GM_DupBase *dupbase = (struct __GM_DupBase *)LIBBASE;
    __aros_setoffsettable(LIBBASE);
    dupbase->taskopencount--;
    if (dupbase->taskopencount != 0)
        return BNULL;

    set_call_libfuncs(SETNAME(CLOSELIB), -1, 0, LIBBASE);
    set_close_rellibraries(LIBBASE);
    __aros_setoffsettable((char *)dupbase->oldbase);
    __GM_SetPerTaskBase(((struct __GM_DupBase *)LIBBASE)->oldpertaskbase);
    __freebase(LIBBASE);
    LIBBASE = rootbase;
    ((struct Library *)LIBBASE)->lib_OpenCnt--;

    if
    (
        (((struct Library *)LIBBASE)->lib_OpenCnt == 0)
        && (((struct Library *)LIBBASE)->lib_Flags & LIBF_DELEXP)
    )
    {
        return AROS_LC1(BPTR, GM_UNIQUENAME(ExpungeLib),
                   AROS_LCA(LIBBASETYPEPTR, LIBBASE, D0),
                   LIBBASETYPEPTR, LIBBASE, 3, Arosc
        );
    }

    return BNULL;

    AROS_LIBFUNC_EXIT
}

AROS_LH1 (BPTR, GM_UNIQUENAME(ExpungeLib),
    AROS_LHA(LIBBASETYPEPTR, extralh, D0),
    LIBBASETYPEPTR, LIBBASE, 3, Arosc
)
{
    AROS_LIBFUNC_INIT

#ifdef GM_SYSBASE_FIELD
    struct ExecBase *SysBase = (struct ExecBase *)GM_SYSBASE_FIELD(LIBBASE);
#endif
    __aros_setoffsettable(LIBBASE);

    if ( ((struct Library *)LIBBASE)->lib_OpenCnt == 0 )
    {
        BPTR seglist = GM_SEGLIST_FIELD(LIBBASE);

        if(!set_call_libfuncs(SETNAME(EXPUNGELIB), -1, 1, LIBBASE))
        {
            ((struct Library *)LIBBASE)->lib_Flags |= LIBF_DELEXP;
            return BNULL;
        }

        Remove((struct Node *)LIBBASE);

        set_call_funcs(SETNAME(FINI_ARRAY), -1, 0);
        set_call_funcs(SETNAME(DTORS), 1, 0);
        set_call_funcs(SETNAME(EXIT), -1, 0);
        set_close_libraries();
#ifdef GM_OOPBASE_FIELD
        CloseLibrary((struct Library *)GM_OOPBASE_FIELD(LIBBASE));
#endif
        FreeTaskStorageSlot(__pertaskslot);
        __pertaskslot = 0;

        __freebase(LIBBASE);

        return seglist;
    }

    ((struct Library *)LIBBASE)->lib_Flags |= LIBF_DELEXP;

    return BNULL;

    AROS_LIBFUNC_EXIT
}

AROS_LH0 (LIBBASETYPEPTR, GM_UNIQUENAME(ExtFuncLib),
    LIBBASETYPEPTR, LIBBASE, 4, Arosc
)
{
    AROS_LIBFUNC_INIT
    return NULL;
    AROS_LIBFUNC_EXIT
}

DEFINESET(INIT)
DEFINESET(EXIT)
DEFINESET(CTORS)
DEFINESET(DTORS)
DEFINESET(INIT_ARRAY)
DEFINESET(FINI_ARRAY)
DEFINESET(INITLIB)
DEFINESET(EXPUNGELIB)
DEFINESET(OPENLIB)
DEFINESET(CLOSELIB)


__section(".text.romtag") const APTR GM_UNIQUENAME(FuncTable)[]=
{
    &AROS_SLIB_ENTRY(GM_UNIQUENAME(OpenLib),Arosc,1),
    &AROS_SLIB_ENTRY(GM_UNIQUENAME(CloseLib),Arosc,2),
    &AROS_SLIB_ENTRY(GM_UNIQUENAME(ExpungeLib),Arosc,3),
    &AROS_SLIB_ENTRY(GM_UNIQUENAME(ExtFuncLib),Arosc,4),
    &AROS_SLIB_ENTRY(fopen,Arosc,5),
    &AROS_SLIB_ENTRY(fdopen,Arosc,6),
    &AROS_SLIB_ENTRY(fclose,Arosc,7),
    &AROS_SLIB_ENTRY(printf,Arosc,8),
    &AROS_SLIB_ENTRY(vprintf,Arosc,9),
    &AROS_SLIB_ENTRY(fprintf,Arosc,10),
    &AROS_SLIB_ENTRY(vfprintf,Arosc,11),
    &AROS_SLIB_ENTRY(fputc,Arosc,12),
    &AROS_SLIB_ENTRY(fputs,Arosc,13),
    &AROS_SLIB_ENTRY(puts,Arosc,14),
    &AROS_SLIB_ENTRY(fflush,Arosc,15),
    &AROS_SLIB_ENTRY(fgetc,Arosc,16),
    &AROS_SLIB_ENTRY(ungetc,Arosc,17),
    &AROS_SLIB_ENTRY(fgets,Arosc,18),
    &AROS_SLIB_ENTRY(feof,Arosc,19),
    &AROS_SLIB_ENTRY(ferror,Arosc,20),
    &AROS_SLIB_ENTRY(fileno,Arosc,21),
    &AROS_SLIB_ENTRY(freopen,Arosc,22),
    &AROS_SLIB_ENTRY(clearerr,Arosc,23),
    &AROS_SLIB_ENTRY(fread,Arosc,24),
    &AROS_SLIB_ENTRY(fwrite,Arosc,25),
    &AROS_SLIB_ENTRY(rename,Arosc,26),
    &AROS_SLIB_ENTRY(sprintf,Arosc,27),
    &AROS_SLIB_ENTRY(vsprintf,Arosc,28),
    &AROS_SLIB_ENTRY(snprintf,Arosc,29),
    &AROS_SLIB_ENTRY(vsnprintf,Arosc,30),
    &AROS_SLIB_ENTRY(scanf,Arosc,31),
    &AROS_SLIB_ENTRY(vscanf,Arosc,32),
    &AROS_SLIB_ENTRY(fscanf,Arosc,33),
    &AROS_SLIB_ENTRY(vfscanf,Arosc,34),
    &AROS_SLIB_ENTRY(sscanf,Arosc,35),
    &AROS_SLIB_ENTRY(vsscanf,Arosc,36),
    &AROS_SLIB_ENTRY(fseek,Arosc,37),
    &AROS_SLIB_ENTRY(ftell,Arosc,38),
    &AROS_SLIB_ENTRY(rewind,Arosc,39),
    &AROS_SLIB_ENTRY(fgetpos,Arosc,40),
    &AROS_SLIB_ENTRY(fsetpos,Arosc,41),
    &AROS_SLIB_ENTRY(remove,Arosc,42),
    &AROS_SLIB_ENTRY(setbuf,Arosc,43),
    &AROS_SLIB_ENTRY(setlinebuf,Arosc,44),
    &AROS_SLIB_ENTRY(setvbuf,Arosc,45),
    &AROS_SLIB_ENTRY(__vcformat,Arosc,46),
    &AROS_SLIB_ENTRY(__vcscan,Arosc,47),
    &AROS_SLIB_ENTRY(access,Arosc,48),
    &AROS_SLIB_ENTRY(close,Arosc,49),
    &AROS_SLIB_ENTRY(dup,Arosc,50),
    &AROS_SLIB_ENTRY(dup2,Arosc,51),
    &AROS_SLIB_ENTRY(getcwd,Arosc,52),
    &AROS_SLIB_ENTRY(isatty,Arosc,53),
    &AROS_SLIB_ENTRY(lseek,Arosc,54),
    &AROS_SLIB_ENTRY(read,Arosc,55),
    &AROS_SLIB_ENTRY(truncate,Arosc,56),
    &AROS_SLIB_ENTRY(unlink,Arosc,57),
    &AROS_SLIB_ENTRY(write,Arosc,58),
    &AROS_SLIB_ENTRY(open,Arosc,59),
    &AROS_SLIB_ENTRY(creat,Arosc,60),
    &AROS_SLIB_ENTRY(utime,Arosc,61),
    &AROS_SLIB_ENTRY(abs,Arosc,62),
    &AROS_SLIB_ENTRY(labs,Arosc,63),
    &AROS_SLIB_ENTRY(atof,Arosc,64),
    &AROS_SLIB_ENTRY(atoi,Arosc,65),
    &AROS_SLIB_ENTRY(atol,Arosc,66),
    &AROS_SLIB_ENTRY(strtol,Arosc,67),
    &AROS_SLIB_ENTRY(strtoul,Arosc,68),
    &AROS_SLIB_ENTRY(strtod,Arosc,69),
    &AROS_SLIB_ENTRY(rand,Arosc,70),
    &AROS_SLIB_ENTRY(srand,Arosc,71),
    &AROS_SLIB_ENTRY(drand48,Arosc,72),
    &AROS_SLIB_ENTRY(erand48,Arosc,73),
    &AROS_SLIB_ENTRY(lrand48,Arosc,74),
    &AROS_SLIB_ENTRY(nrand48,Arosc,75),
    &AROS_SLIB_ENTRY(mrand48,Arosc,76),
    &AROS_SLIB_ENTRY(jrand48,Arosc,77),
    &AROS_SLIB_ENTRY(srand48,Arosc,78),
    &AROS_SLIB_ENTRY(seed48,Arosc,79),
    &AROS_SLIB_ENTRY(lcong48,Arosc,80),
    &AROS_SLIB_ENTRY(random,Arosc,81),
    &AROS_SLIB_ENTRY(srandom,Arosc,82),
    &AROS_SLIB_ENTRY(initstate,Arosc,83),
    &AROS_SLIB_ENTRY(setstate,Arosc,84),
    &AROS_SLIB_ENTRY(qsort,Arosc,85),
    &AROS_SLIB_ENTRY(bsearch,Arosc,86),
    &AROS_SLIB_ENTRY(malloc,Arosc,87),
    &AROS_SLIB_ENTRY(calloc,Arosc,88),
    &AROS_SLIB_ENTRY(realloc,Arosc,89),
    &AROS_SLIB_ENTRY(free,Arosc,90),
    &AROS_SLIB_ENTRY(getenv,Arosc,91),
    &AROS_SLIB_ENTRY(setenv,Arosc,92),
    &AROS_SLIB_ENTRY(putenv,Arosc,93),
    &AROS_SLIB_ENTRY(mktemp,Arosc,94),
    &AROS_SLIB_ENTRY(system,Arosc,95),
    &AROS_SLIB_ENTRY(gcvt,Arosc,96),
    &AROS_SLIB_ENTRY(mkdir,Arosc,97),
    &AROS_SLIB_ENTRY(stat,Arosc,98),
    &AROS_SLIB_ENTRY(fstat,Arosc,99),
    &AROS_SLIB_ENTRY(opendir,Arosc,100),
    &AROS_SLIB_ENTRY(readdir,Arosc,101),
    &AROS_SLIB_ENTRY(rewinddir,Arosc,102),
    &AROS_SLIB_ENTRY(closedir,Arosc,103),
    &AROS_SLIB_ENTRY(telldir,Arosc,104),
    &AROS_SLIB_ENTRY(seekdir,Arosc,105),
    &AROS_SLIB_ENTRY(gettimeofday,Arosc,106),
    &AROS_SLIB_ENTRY(utimes,Arosc,107),
    NULL,
    NULL,
    &AROS_SLIB_ENTRY(strftime,Arosc,110),
    &AROS_SLIB_ENTRY(strerror,Arosc,111),
    &AROS_SLIB_ENTRY(exit,Arosc,112),
    &AROS_SLIB_ENTRY(abort,Arosc,113),
    &AROS_SLIB_ENTRY(chmod,Arosc,114),
    &AROS_SLIB_ENTRY(localtime,Arosc,115),
    &AROS_SLIB_ENTRY(time,Arosc,116),
    &AROS_SLIB_ENTRY(gmtime,Arosc,117),
    &AROS_SLIB_ENTRY(perror,Arosc,118),
    &AROS_SLIB_ENTRY(ctime,Arosc,119),
    &AROS_SLIB_ENTRY(clock,Arosc,120),
    &AROS_SLIB_ENTRY(__progonly_atexit,Arosc,121),
    &AROS_SLIB_ENTRY(pipe,Arosc,122),
    &AROS_SLIB_ENTRY(fcntl,Arosc,123),
    &AROS_SLIB_ENTRY(difftime,Arosc,124),
    &AROS_SLIB_ENTRY(ftruncate,Arosc,125),
    &AROS_SLIB_ENTRY(umask,Arosc,126),
    &AROS_SLIB_ENTRY(unsetenv,Arosc,127),
    &AROS_SLIB_ENTRY(asctime,Arosc,128),
    &AROS_SLIB_ENTRY(sleep,Arosc,129),
    &AROS_SLIB_ENTRY(chdir,Arosc,130),
    &AROS_SLIB_ENTRY(__stdc_ioerr2errno,Arosc,131),
    &AROS_SLIB_ENTRY(mktime,Arosc,132),
    &AROS_SLIB_ENTRY(div,Arosc,133),
    &AROS_SLIB_ENTRY(ldiv,Arosc,134),
    &AROS_SLIB_ENTRY(sigaction,Arosc,135),
    &AROS_SLIB_ENTRY(sigaddset,Arosc,136),
    &AROS_SLIB_ENTRY(sigemptyset,Arosc,137),
    &AROS_SLIB_ENTRY(fchmod,Arosc,138),
    &AROS_SLIB_ENTRY(sigdelset,Arosc,139),
    &AROS_SLIB_ENTRY(sigfillset,Arosc,140),
    &AROS_SLIB_ENTRY(sigismember,Arosc,141),
    &AROS_SLIB_ENTRY(sigpending,Arosc,142),
    &AROS_SLIB_ENTRY(sigprocmask,Arosc,143),
    &AROS_SLIB_ENTRY(sigsuspend,Arosc,144),
    &AROS_SLIB_ENTRY(tmpnam,Arosc,145),
    &AROS_SLIB_ENTRY(updatestdio,Arosc,146),
    &AROS_SLIB_ENTRY(__assert,Arosc,147),
    &AROS_SLIB_ENTRY(lldiv,Arosc,148),
    &AROS_SLIB_ENTRY(strtoll,Arosc,149),
    &AROS_SLIB_ENTRY(strtoull,Arosc,150),
    &AROS_SLIB_ENTRY(__progonly_on_exit,Arosc,151),
    &AROS_SLIB_ENTRY(getpid,Arosc,152),
    &AROS_SLIB_ENTRY(popen,Arosc,153),
    &AROS_SLIB_ENTRY(stcd_l,Arosc,154),
    &AROS_SLIB_ENTRY(stch_l,Arosc,155),
    &AROS_SLIB_ENTRY(stcl_d,Arosc,156),
    &AROS_SLIB_ENTRY(stcl_h,Arosc,157),
    &AROS_SLIB_ENTRY(stcl_o,Arosc,158),
    &AROS_SLIB_ENTRY(stco_l,Arosc,159),
    &AROS_SLIB_ENTRY(realloc_nocopy,Arosc,160),
    &AROS_SLIB_ENTRY(getfsstat,Arosc,161),
    &AROS_SLIB_ENTRY(signal,Arosc,162),
    &AROS_SLIB_ENTRY(_exit,Arosc,163),
    &AROS_SLIB_ENTRY(kill,Arosc,164),
    &AROS_SLIB_ENTRY(setlocale,Arosc,165),
    &AROS_SLIB_ENTRY(pclose,Arosc,166),
    &AROS_SLIB_ENTRY(mkstemp,Arosc,167),
    &AROS_SLIB_ENTRY(execvp,Arosc,168),
    &AROS_SLIB_ENTRY(rmdir,Arosc,169),
    &AROS_SLIB_ENTRY(getuid,Arosc,170),
    &AROS_SLIB_ENTRY(geteuid,Arosc,171),
    &AROS_SLIB_ENTRY(__get_arosc_userdata,Arosc,172),
    &AROS_SLIB_ENTRY(__arosc_nixmain_abiv0,Arosc,173),
    &AROS_SLIB_ENTRY(spawnv,Arosc,174),
    &AROS_SLIB_ENTRY(chown,Arosc,175),
    &AROS_SLIB_ENTRY(spawnvp,Arosc,176),
    &AROS_SLIB_ENTRY(readlink,Arosc,177),
    &AROS_SLIB_ENTRY(getpwuid,Arosc,178),
    &AROS_SLIB_ENTRY(getgrgid,Arosc,179),
    &AROS_SLIB_ENTRY(getgid,Arosc,180),
    &AROS_SLIB_ENTRY(getegid,Arosc,181),
    &AROS_SLIB_ENTRY(getpwnam,Arosc,182),
    &AROS_SLIB_ENTRY(setpwent,Arosc,183),
    &AROS_SLIB_ENTRY(getpwent,Arosc,184),
    &AROS_SLIB_ENTRY(endpwent,Arosc,185),
    &AROS_SLIB_ENTRY(getgroups,Arosc,186),
    &AROS_SLIB_ENTRY(getgrnam,Arosc,187),
    &AROS_SLIB_ENTRY(endgrent,Arosc,188),
    &AROS_SLIB_ENTRY(getgrent,Arosc,189),
    &AROS_SLIB_ENTRY(setgrent,Arosc,190),
    &AROS_SLIB_ENTRY(fchown,Arosc,191),
    &AROS_SLIB_ENTRY(fsync,Arosc,192),
    &AROS_SLIB_ENTRY(link,Arosc,193),
    &AROS_SLIB_ENTRY(symlink,Arosc,194),
    &AROS_SLIB_ENTRY(mknod,Arosc,195),
    &AROS_SLIB_ENTRY(getppid,Arosc,196),
    &AROS_SLIB_ENTRY(execl,Arosc,197),
    &AROS_SLIB_ENTRY(getpgrp,Arosc,198),
    &AROS_SLIB_ENTRY(ioctl,Arosc,199),
    &AROS_SLIB_ENTRY(wait,Arosc,200),
    &AROS_SLIB_ENTRY(setuid,Arosc,201),
    &AROS_SLIB_ENTRY(setgid,Arosc,202),
    &AROS_SLIB_ENTRY(getw_abiv0,Arosc,203),
    &AROS_SLIB_ENTRY(putw_abiv0,Arosc,204),
    &AROS_SLIB_ENTRY(getloadavg,Arosc,205),
    &AROS_SLIB_ENTRY(raise,Arosc,206),
    &AROS_SLIB_ENTRY(times,Arosc,207),
    &AROS_SLIB_ENTRY(ftime,Arosc,208),
    &AROS_SLIB_ENTRY(strsep,Arosc,209),
    &AROS_SLIB_ENTRY(tempnam,Arosc,210),
    &AROS_SLIB_ENTRY(tmpfile,Arosc,211),
    &AROS_SLIB_ENTRY(posix_memalign,Arosc,212),
    &AROS_SLIB_ENTRY(localtime_r,Arosc,213),
    &AROS_SLIB_ENTRY(gmtime_r,Arosc,214),
    &AROS_SLIB_ENTRY(ctime_r,Arosc,215),
    &AROS_SLIB_ENTRY(asctime_r,Arosc,216),
    &AROS_SLIB_ENTRY(basename,Arosc,217),
    &AROS_SLIB_ENTRY(dirfd,Arosc,218),
    &AROS_SLIB_ENTRY(dirname,Arosc,219),
    &AROS_SLIB_ENTRY(localeconv,Arosc,220),
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    &AROS_SLIB_ENTRY(mbrtowc,Arosc,238),
    NULL,
    &AROS_SLIB_ENTRY(regcomp,Arosc,240),
    &AROS_SLIB_ENTRY(regerror,Arosc,241),
    &AROS_SLIB_ENTRY(regexec,Arosc,242),
    &AROS_SLIB_ENTRY(regfree,Arosc,243),
    &AROS_SLIB_ENTRY(putc,Arosc,244),
    &AROS_SLIB_ENTRY(getc,Arosc,245),
    &AROS_SLIB_ENTRY(getchar,Arosc,246),
    &AROS_SLIB_ENTRY(gets,Arosc,247),
    NULL,
    NULL,
    &AROS_SLIB_ENTRY(__get_default_file,Arosc,250),
    &AROS_SLIB_ENTRY(statfs,Arosc,251),
    &AROS_SLIB_ENTRY(usleep,Arosc,252),
    &AROS_SLIB_ENTRY(ttyname,Arosc,253),
    &AROS_SLIB_ENTRY(getlogin,Arosc,254),
    &AROS_SLIB_ENTRY(fchdir,Arosc,255),
    &AROS_SLIB_ENTRY(uname,Arosc,256),
    &AROS_SLIB_ENTRY(__env_get_environ,Arosc,257),
    &AROS_SLIB_ENTRY(execve,Arosc,258),
    &AROS_SLIB_ENTRY(strptime,Arosc,259),
    &AROS_SLIB_ENTRY(vfork,Arosc,260),
    &AROS_SLIB_ENTRY(waitpid,Arosc,261),
    &AROS_SLIB_ENTRY(execv,Arosc,262),
    &AROS_SLIB_ENTRY(execlp,Arosc,263),
    &AROS_SLIB_ENTRY(flock,Arosc,264),
    &AROS_SLIB_ENTRY(strtoimax,Arosc,265),
    &AROS_SLIB_ENTRY(strtoumax,Arosc,266),
    &AROS_SLIB_ENTRY(sharecontextwithchild,Arosc,267),
    &AROS_SLIB_ENTRY(sysconf,Arosc,268),
    &AROS_SLIB_ENTRY(lstat,Arosc,269),
    &AROS_SLIB_ENTRY(__arosc_program_startup_abiv0,Arosc,270),
    &AROS_SLIB_ENTRY(__arosc_program_end,Arosc,271),
    &AROS_SLIB_ENTRY(nanosleep,Arosc,272),
    &AROS_SLIB_ENTRY(fseeko,Arosc,273),
    &AROS_SLIB_ENTRY(ftello,Arosc,274),
    &AROS_SLIB_ENTRY(pathconf,Arosc,275),
    &AROS_SLIB_ENTRY(realpath,Arosc,276),
    &AROS_SLIB_ENTRY(sync,Arosc,277),
    &AROS_SLIB_ENTRY(tcgetattr,Arosc,278),
    &AROS_SLIB_ENTRY(tcsetattr,Arosc,279),
    &AROS_SLIB_ENTRY(cfgetispeed,Arosc,280),
    &AROS_SLIB_ENTRY(cfgetospeed,Arosc,281),
    &AROS_SLIB_ENTRY(cfsetispeed,Arosc,282),
    &AROS_SLIB_ENTRY(cfsetospeed,Arosc,283),
    /* Version 43 */
    &AROS_SLIB_ENTRY(__get_arosc_ctype,Arosc,284),
    &AROS_SLIB_ENTRY(getrlimit,Arosc,285),
    &AROS_SLIB_ENTRY(setrlimit,Arosc,286),
    &AROS_SLIB_ENTRY(__posixc_set_environptr,Arosc,287),
    &AROS_SLIB_ENTRY(___gmtoffset,Arosc,288),
    &AROS_SLIB_ENTRY(__arosc_set_errorptr,Arosc,289),
    &AROS_SLIB_ENTRY(__arosc_set_exitjmp,Arosc,290),
    &AROS_SLIB_ENTRY(__arosc_jmp2exit,Arosc,291),
    &AROS_SLIB_ENTRY(__arosc_get_errorptr,Arosc,292),
    &AROS_SLIB_ENTRY(__posixc_get_environptr,Arosc,293),
    &AROS_SLIB_ENTRY(__arosc_program_startup,Arosc,294),
    &AROS_SLIB_ENTRY(__posixc_nixmain,Arosc,295),
    &AROS_SLIB_ENTRY(clock_gettime,Arosc,296),
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    /* Version 44 */
    &AROS_SLIB_ENTRY(fopen64,Arosc,303),
    &AROS_SLIB_ENTRY(fseeko64,Arosc,304),
    NULL,
    &AROS_SLIB_ENTRY(ftello64,Arosc,306),
    &AROS_SLIB_ENTRY(fstat64,Arosc,307),
    NULL,
    NULL,
    &AROS_SLIB_ENTRY(lseek64,Arosc,310),
    (void *)-1
};
