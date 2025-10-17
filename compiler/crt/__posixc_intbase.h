#ifndef __POSIXC_INTBASE_H
#define __POSIXC_INTBASE_H

/* This file replaces the header found in compiler/stdc */

#include "__crt_intbase.h"

#include <exec/lists.h>
#include <dos/dos.h>
#include <devices/timer.h>
#include <exec/semaphores.h>

#include <limits.h>
#include <stdint.h>
#include <sys/stat.h>
#include <pwd.h>

/* don't include stdio.h as it will make it include in each .c file before defines can take action */
struct __sFILE;
typedef struct __sFILE FILE;

struct PosixCBase
{
    FILE *_stdin, *_stdout, *_stderr;
};

/* Some private structs */
struct __random_state;
struct __env_item;
struct _fdesc;
struct vfork_data;

struct PosixCIntBase
{
    struct PosixCBase lib;

    /* optional libs */
    struct Library           *PosixCUserGroupBase;

    /* common */
    APTR internalpool;

    struct Device *timerBase;
    struct timerequest timerReq;
    struct MsgPort timerPort;

    /* random.c */
    struct __random_state *rs;

    /* getpwuid.c */
    struct passwd pwd;

    /* getpass.c */
    char passbuffer[PASS_MAX];

    /* __posixc_environ.c; don't use this field outside that file */
    char ***environptr;

    /* __env.c */
    struct __env_item *env_list;

    /* __exec.c */
    char **exec_tmparray;

    /* __fdesc.c */
    int fd_slots;
    struct _fdesc ***fd_array;
    struct SignalSemaphore fd_sem;

    /* __upath.c */
    char *upathbuf;  /* Buffer that holds intermediate converted paths */
    int doupath;   /* BOOL - does the conversion need to be done?  */

    /* chdir.c/fchdir.c */
    int cd_changed;
    BPTR cd_lock;

    /* flock.c */
    struct MinList *file_locks;

    /* umask */
    mode_t umask;

    /* __stdio.c */
    struct MinList stdio_files;

    /* setuid.c/getuid.c */
    uid_t uid; /* Real user id of process */
    uid_t euid; /* Effective user id of process */
    /* set(e)gid.c/get(e)gid.c */
    gid_t gid; /* Real group id of process */
    gid_t egid; /* Effective group id of process */
};



struct PosixCBase *__aros_getbase_PosixCBase(void);

#endif
