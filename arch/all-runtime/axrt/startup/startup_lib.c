/*
    Copyright © 2019-2020, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/exec.h>
#include <proto/dos.h>

#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#define __USE_GNU
#include <pthread.h>
#include <libgen.h>
#include <pwd.h>
#include <limits.h>

#include "rt_version.h"

int main_AddDataTypes();
int main_Decoration();
int main_IPrefs();

CONST_STRPTR WANDERER_ROOT = "SYS:System/Wanderer/";

static CONST_STRPTR Kickstart [] =
{
        "boot/runtimelinux/Libs/exec.library", /* EXEC MUST BE AT INDEX 0 */
        "Libs/aros.library",
        "Libs/utility.library",
        "Libs/oop.library",
        "Devs/Drivers/hiddclass.hidd",
        "Devs/Drivers/gfx.hidd",
        "Libs/graphics.library",
        "Libs/cybergraphics.library",
        "Libs/layers.library",
        "Libs/keymap.library",
        "boot/runtimelinux/Libs/intuition.library",
        "boot/runtimelinux/Libs/dos.library",
        "Devs/lddemon.resource",
        "boot/runtimelinux/Libs/expansion.library",
        "Devs/dosboot.resource",
        "boot/runtimelinux/Devs/Drivers/x11gfx.hidd",
        "boot/runtimelinux/Devs/hostlib.resource",
        "Devs/Drivers/mouse.hidd",
        "Devs/Drivers/keyboard.hidd",
        "Devs/FileSystem.resource",
        "boot/runtimelinux/L/emul-handler",
        "boot/runtimelinux/Devs/Drivers/unixio.hidd",
        "boot/runtimelinux/Devs/timer.device",
        "Devs/input.device",
        "Devs/keyboard.device",
        "Devs/gameport.device",
        "L/ram-handler",
        "Libs/debug.library",
        "Devs/console.device",
        "L/con-handler",
        NULL
};

static void __bye()
{
    printf("Exiting...\n");
    exit(100);
}

#define ENV_AXRT_ROOT   "AXRT_ROOT"

static ULONG Kickstart_Count = (sizeof(Kickstart)/sizeof(Kickstart[0]));

static void _sort(struct Resident **RomTag, ULONG num)
{
    LONG i;
    BOOL sorted;
    do
    {
        sorted = TRUE;

        for (i = 0; i < num - 1; i++)
        {
            if (RomTag[i]->rt_Pri < RomTag[i+1]->rt_Pri)
            {
                struct Resident *tmp;

                tmp = RomTag[i+1];
                RomTag[i+1] = RomTag[i];
                RomTag[i] = tmp;

                sorted = FALSE;
            }
        }
    } while (!sorted);
}

static struct ExecBase * EarlyExecInit()
{
    STRPTR root = getenv(ENV_AXRT_ROOT);
    void *(*__get_resident)();
    TEXT path[512] = { 0 };

    if (root) strncat(path, root, strlen(root));
    strncat(path, Kickstart[0], strlen(Kickstart[0]));

    void *__so_handle = dlopen(path, RTLD_LAZY);
    __get_resident = (void *(*)(void *))dlsym(__so_handle, "__get_resident");
    struct Resident *Exec_resident = (struct Resident *)(__get_resident)();

    return AROS_UFC3(struct ExecBase *, Exec_resident->rt_Init,
                AROS_UFCA(struct MemHeader *, NULL, D0),
                AROS_UFCA(struct TagItem *, NULL, A0),
                AROS_UFCA(struct ExecBase *, NULL, A6));
}

static void InitKickstart(struct ExecBase *SysBase)
{
    LONG i = 0;
    struct Resident ** resList = AllocMem(sizeof(struct Resident *) * Kickstart_Count, MEMF_PUBLIC | MEMF_CLEAR);
    STRPTR root = getenv(ENV_AXRT_ROOT);
    void *(*__get_resident)();

    while (Kickstart[i] != NULL)
    {
        TEXT path[512] = { 0 };
        if (root) strncat(path, root, strlen(root));
        strncat(path, Kickstart[i], strlen(Kickstart[i]));

        void *__so_handle = dlopen(path, RTLD_LAZY);
        if (__so_handle == NULL)
        {
            printf("<<ERROR>>: Failed to load kickstart module %s\n", path);
            __bye();
        }
        __get_resident = (void *(*)())dlsym(__so_handle, "__get_resident");
        resList[i] = (struct Resident *)(__get_resident)();

        i++;
    }
    resList[i] = NULL;

    _sort(resList, i);
    SysBase->ResModules = resList;

    InitCode(RTF_SINGLETASK, 0);
    InitCode(RTF_COLDSTART, 0);
}

static struct ExecBase * local_SysBase = NULL;

APTR __get_sysbase()
{
    return local_SysBase;
}

static VOID * InitRuntime(VOID *ptr)
{
    /* Two pass initialization of exec.library */
    (void)ptr;
    local_SysBase = EarlyExecInit();
    InitKickstart(local_SysBase);
    asm("int3"); /* Should never reach this point as Exec bootstrap RemTasks itself */
    return NULL;
}

struct ARPSMsg
{
    struct Message arps_Msg;
    VOID (*arps_Target)(APTR, APTR);

    /* Private fields */
    void (*arps_RunProgramSets)(STRPTR, LONG, struct ExecBase *);
    STRPTR  arps_CurrentDir;
    STRPTR  arps_ProgramName;
    STRPTR  arps_ProgramDir;
};

static VOID RunProgram(APTR sysbase, APTR _m)
{
    /* This trampoline is executed by "AxRuntime Program" process */
    struct ARPSMsg *msg;

    /* This library was never properly loaded by Exec. Do manual initialization */
    SysBase = (struct ExecBase *)sysbase;
    if (!set_open_libraries()) asm("int3");

    /* Retrieve message */
    msg = (struct ARPSMsg *)_m;

    /* Make WANDERER assign */
    BPTR lck = Lock(WANDERER_ROOT, SHARED_LOCK);
    AssignLock("WANDERER", lck);
    AssignPath("THEME", "USERSYS:Prefs/Presets/Themes/pixblue");
    AssignPath("FONTS","USERSYS:Fonts");

    /*
     * FIXME: Maybe these should be passed to CreateNewProc instead of being
     * set manually here?
     */
    /* Set CurrentDir/HomeDir for process */
    BPTR currdir = Lock(msg->arps_CurrentDir, SHARED_LOCK);
    CurrentDir(currdir);
    BPTR progdir = Lock(msg->arps_ProgramDir, SHARED_LOCK);
    SetProgramDir(progdir);

    /* Set correct Task name */
    struct Task *me = FindTask(NULL);
    me->tc_Node.ln_Name = AllocMem(strlen(msg->arps_ProgramName) + 1, MEMF_PUBLIC);
    CopyMem(msg->arps_ProgramName, me->tc_Node.ln_Name, strlen(msg->arps_ProgramName));
    // FIXME: this does not change linux-side name!!!

    main_AddDataTypes();
    main_IPrefs();
    main_Decoration();
    Close(Open("RAM:Welcome", MODE_NEWFILE));

    msg->arps_RunProgramSets(NULL, 0, SysBase);

    /* TODO: what to do when process exits */
    asm("int3");
}

__attribute__((visibility("default"))) void __kick_start(void *__run_program_sets, int __version)
{
    /* This thread is not a Process/Task. Restrictions apply. */
    pthread_t execbootstrap;
    pthread_create(&execbootstrap, NULL, &InitRuntime, NULL);
    pthread_setname_np(execbootstrap, "Exec bootstrap");

    pthread_join(execbootstrap, NULL);
    /* At this point Exec is up and running. DOS is waiting for booting. */

    char _t[PATH_MAX];
    STRPTR _p = NULL;
    int ret = 0;

#define SysBase local_SysBase
    /* Sequence:
     *  DOS boot sequence creates "AxRuntime Program" Process (ARPP)
     *  ARPP creates public port
     *  This thread sends pointer to RunProgram to the port
     *  ARPP executes RunProgram()
     *  This thread waits for finishing of execution and exits never reaching main (TODO)
     */
    struct MsgPort *startup = NULL;
    while((startup = FindPort("ARPS")) == NULL)
        usleep(100000);

    struct ARPSMsg msg = {0};
    msg.arps_Msg.mn_Length      = sizeof(struct ARPSMsg);
    msg.arps_Msg.mn_ReplyPort   = NULL;
    msg.arps_Target             = RunProgram;
    msg.arps_RunProgramSets     = __run_program_sets;

    /* Current directory */
    getcwd(_t, PATH_MAX);
    msg.arps_CurrentDir         = calloc(strlen(_t) + 6, 1);
    strcat(msg.arps_CurrentDir, "ROOT:");
    strcat(msg.arps_CurrentDir, _t + 1);
    strcat(msg.arps_CurrentDir, "/");
    printf("CURRENT_DIR : %s\n", msg.arps_CurrentDir);

    /* Program directory and executable name */
    ret = readlink("/proc/self/exe", _t, PATH_MAX);
    _t[ret] = 0;
    msg.arps_ProgramDir         = calloc(strlen(_t) + 6, 1);
    strcat(msg.arps_ProgramDir, "ROOT:");
    strcat(msg.arps_ProgramDir, _t + 1);
    _p = strrchr(msg.arps_ProgramDir, '/');
    *(_p + 1) = 0;
    printf("PROGRAM DIR : %s\n", msg.arps_ProgramDir);

    _p = strrchr(_t, '/');
    msg.arps_ProgramName        = calloc(strlen(_p) + 1, 1);
    strcat(msg.arps_ProgramName, _p + 1);
    printf("PROGRAM NAME: %s\n", msg.arps_ProgramName);


    /* Done, send message */
    PutMsg(startup, (struct Message *)&msg);
#undef SysBase

    /* TODO: handle exit */
    while(1)
        sleep(1);
    asm("int3");
}

static void check_install_usersys(const char *runtimeroot, const char *usersys)
{
    struct stat s;
    int err = stat(usersys, &s);

    /* If does not exist, copy it from runtime root */
    if ((err == -1) || (!S_ISDIR(s.st_mode)))
    {
        char command[1024];
        strcpy(command, "cp -r ");
        strcat(command, runtimeroot);
        strcat(command, "/UserSYS/ ");
        strcat(command, usersys);

        printf("<<INFO>>: Installing USERSYS at %s\n", usersys);

        system(command);
    }
}

__attribute__((visibility("default"))) void __set_runtime_env(int __version)
{
    /* Paths needs to end with "/" */
    char *RUNTIME_ROOT = NULL, *AXRTSYS = NULL, *USERSYS = NULL;

    const int _size = 512;
    char tstbuff[_size];
    char __usersys[_size];

    RUNTIME_ROOT    = malloc(_size);
    AXRTSYS         = malloc(_size);
    USERSYS         = malloc(_size);

    printf("<<INFO>>: AxRT %d.%d\n", RT_GET_VERSION(RT_VER), RT_GET_REVISION(RT_VER));

    /* First priority, local sub-directories */
    getcwd(tstbuff, _size);
    strcat(tstbuff, "/AXRTSYS/");

    struct stat s;
    int err = stat(tstbuff, &s);

    if ((err == -1) || (!S_ISDIR(s.st_mode)))
    {
        /* Second priority, absolute */
        char *t;
        struct passwd *pw;
        char buff[_size];

        printf("<<INFO>>: Using absolute paths.\n");

        t = getenv(ENV_AXRT_ROOT);

        if (t)
        {
            printf("<<INFO>>: "ENV_AXRT_ROOT" environment variable set\n");

            strcpy(buff, t);
            int i = strlen(buff);
            if (buff[i-1] == '/') buff[i-1] = 0;
        }
        else
        {
            strcpy(buff, "/usr/lib/x86_64-linux-gnu/axrt/2.0");
        }

        strcpy(RUNTIME_ROOT, buff);
        strcat(RUNTIME_ROOT, "/");

        /* ~/.axrt/ */
        pw = getpwuid(getuid());
        strcpy(__usersys, pw->pw_dir);
        strcat(__usersys, "/.axrt/");
    }
    else
    {
        char buff[_size];

        /* Paths are relative to executable directory */
        printf("<<INFO>>: Using relative paths.\n");

        getcwd(buff, _size);
        strcpy(RUNTIME_ROOT, buff);
        strcat(RUNTIME_ROOT, "/AXRTSYS/");
        strcpy(__usersys, buff);
        strcat(__usersys, "/USERSYS/");
    }

    strcpy(AXRTSYS, "ROOT:");
    strcat(AXRTSYS, RUNTIME_ROOT + 1);
    strcpy(USERSYS, "ROOT:");
    strcat(USERSYS, __usersys + 1);

    /* Check & Install USERSYS */
    check_install_usersys(RUNTIME_ROOT, __usersys);

    /* Summary */
    printf("RUNTIME_ROOT: %s\n", RUNTIME_ROOT);
    printf("AXRTSYS     : %s\n", AXRTSYS);
    printf("USERSYS     : %s\n", USERSYS);

    setenv(ENV_AXRT_ROOT, RUNTIME_ROOT, 1);
    setenv("AXRTSYS", AXRTSYS, 1);
    setenv("USERSYS", USERSYS, 1);
}
