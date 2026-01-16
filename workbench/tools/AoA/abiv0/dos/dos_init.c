/*
    Copyright (C) 2025-2026, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <aros/debug.h>
#include <exec/rawfmt.h>

#include <string.h>

#include "../include/exec/structures.h"
#include "../include/exec/functions.h"
#include "../include/aros/cpu.h"
#include "../include/aros/proxy.h"
#include "../include/aros/call32.h"

#include "../include/dos/structures.h"
#include "../include/utility/structures.h"
#include "../include/timer/structures.h"

#include "../exec/exec_ports.h"

#include "../support.h"

struct ExecBaseV0 *DOS_SysBaseV0;
extern struct LibraryV0 *abiv0TimerBase;

struct DosLibraryV0 *abiv0DOSBase;

extern ULONG *dosfunctable;

struct FileLockProxy
{
    ULONG _pad[6]; /* Make sure 'native' pointer is at the same offset as for FileHandlerProxy */

    BPTR native;
};

struct FileHandleProxy
{
    /* The next three are used with packet-based filesystems */
    ULONG  fh_Flags;
    LONG   fh_Interactive;	/* interactive handle flag */
    APTR32 fh_Type;   /* port to send packets to */

    BPTR32  fh_Buf;
    LONG    fh_Pos;
    LONG    fh_End;

    BPTR native;
};

struct DosListProxy
{
    struct DosListV0    base;
    struct DosList      *native;
};

struct FileHandleProxy *makeFileHandleProxy(BPTR native)
{
    struct FileHandleProxy *proxy = abiv0_AllocMem(sizeof(struct FileHandleProxy), MEMF_CLEAR, DOS_SysBaseV0);
    proxy->native = native;
    return proxy;
}

struct FileHandleProxy *makeFileHandleProxyDetailed(BPTR native)
{
    struct FileHandleProxy *proxy = makeFileHandleProxy(native);
    struct FileHandle *fh = BADDR(proxy->native);
    proxy->fh_Pos = fh->fh_Pos; /* Used by 32-bit ReadArgs */
    proxy->fh_End = fh->fh_End;
}

static struct TagItemV0 *LibNextTagItemV0(struct TagItemV0 **tagListPtr)
{
    if (!(*tagListPtr))
        return NULL;

    while(1)
    {
        switch(((*tagListPtr)->ti_Tag))
        {
            case TAG_MORE:
unhandledCodePath(__func__, "TAG_MORE", 0, 0);
                if (!((*tagListPtr) = (struct TagItemV0 *)(IPTR)(*tagListPtr)->ti_Data))
                    return NULL;
                continue;

            case TAG_IGNORE:
                break;

            case TAG_END:
                (*tagListPtr) = 0;
                return NULL;

            case TAG_SKIP:
unhandledCodePath(__func__, "TAG_SKIP", 0, 0);
                (*tagListPtr) += (*tagListPtr)->ti_Data + 1;
                continue;

            default:
                return (*tagListPtr)++;

        }

        (*tagListPtr)++;
    }
}

static struct TagItem *CloneTagItemsV02Native(const struct TagItemV0 *tagList)
{
    struct TagItem *newList;
    LONG numTags = 1;

    struct TagItemV0 *tmp;

    tmp = (struct TagItemV0 *)tagList;
    while (LibNextTagItemV0 (&tmp) != NULL)
        numTags++;

    newList = AllocMem(sizeof(struct TagItem) * numTags, MEMF_CLEAR);

    LONG pos = 0;
    tmp = (struct TagItemV0 *)tagList;
    do
    {
        newList[pos].ti_Tag = tmp->ti_Tag;
        newList[pos].ti_Data = tmp->ti_Data;
        pos++;
    } while (LibNextTagItemV0 (&tmp) != NULL);

    return newList;
}

void abiv0_PutStr(CONST_STRPTR text)
{
    PutStr(text);
}
MAKE_PROXY_ARG_2(PutStr)

LONG abiv0_SetVBuf()
{
    return 0;
}
MAKE_PROXY_ARG_5(SetVBuf)

#define MAXCHILDPROCESSES 8
struct ProcessV0 *g_v0childprocesses[MAXCHILDPROCESSES];
struct Task *g_nativechildprocesses[MAXCHILDPROCESSES];

struct Task * childprocess_getbyv0(struct TaskV0 *childv0)
{
    if (childv0)
        for (LONG i = 0; i < MAXCHILDPROCESSES; i++)
        {
            if (childv0 == (struct TaskV0 *)g_v0childprocesses[i])
                return (struct Task *)g_nativechildprocesses[i];
        }

    return NULL;
}

struct TaskV0 * childprocess_getbynative(struct Task *childnative)
{
    if (childnative != NULL)
        for (LONG i = 0; i < MAXCHILDPROCESSES; i++)
        {
            if (childnative == (struct Task *)g_nativechildprocesses[i])
                return (struct TaskV0 *)g_v0childprocesses[i];
        }

    return NULL;
}

static LONG childprocess_getslot()
{
    for (LONG i = 0; i < MAXCHILDPROCESSES; i++)
    if (g_nativechildprocesses[i] == NULL)
    {
        g_nativechildprocesses[i] = (APTR)0x1;
        return i;
    }

    return -1;
}

struct CreateNewProcMsg
{
    struct Message  msg;
    APTR32          cnp_Entry;
    ULONG           cnp_StackSize;
    LONG            cnp_Slot;
};

static AROS_UFH3(void, createNewProc_trampoline,
    AROS_UFHA(char *,              args, A0),
    AROS_UFHA(ULONG,               argSize, D0),
    AROS_UFHA(struct ExecBase *,   SysBase, A6))
{
    AROS_USERFUNC_INIT

    struct StackSwapStructV0 sss;
    struct StackSwapArgsV0 ssa;

    struct Process *me = (struct Process *)FindTask(NULL);
    WaitPort(&me->pr_MsgPort);
    struct CreateNewProcMsg *msg = (struct CreateNewProcMsg *)GetMsg(&me->pr_MsgPort);

    ULONG stacksize = msg->cnp_StackSize;
    APTR32 entry = msg->cnp_Entry;
    LONG slot = msg->cnp_Slot;
    FreeMem(msg, sizeof(struct CreateNewProcMsg));

    if (stacksize == 0) stacksize = AROS_STACKSIZE;

    /* Child process will be executing 32-bit code and needs 32-bit stack */
    APTR stack31bit = abiv0_AllocMem(stacksize, MEMF_CLEAR | MEMF_31BIT, DOS_SysBaseV0);

    sss.stk_Lower = (APTR32)(IPTR)stack31bit;
    sss.stk_Upper = sss.stk_Lower + stacksize;
    sss.stk_Pointer = sss.stk_Upper;

    STRPTR argptr  = NULL;
    if (args)
    {
        argptr = (STRPTR)abiv0_AllocMem(argSize + 1, MEMF_PUBLIC, DOS_SysBaseV0);
        CopyMem(args, argptr, argSize + 1);
    }

    ssa.Args[0]     = (APTR32)(IPTR)argptr;
    ssa.Args[1]     = argSize;
    ssa.Args[2]     = (APTR32)(IPTR)DOS_SysBaseV0;

    abiv0_NewStackSwap(&sss, (LONG_FUNC)(IPTR)entry, &ssa, DOS_SysBaseV0);

    abiv0_FreeMem(argptr, argSize + 1, DOS_SysBaseV0);
    abiv0_FreeMem(stack31bit, stacksize, DOS_SysBaseV0);

    /* Free slot */
    abiv0_FreeMem(g_v0childprocesses[slot], sizeof(struct ProcessV0), DOS_SysBaseV0);
    g_v0childprocesses[slot] = NULL;
    g_nativechildprocesses[slot] = NULL;

    AROS_USERFUNC_EXIT
}

struct ProcessV0 *abiv0_CreateNewProc(const struct TagItemV0 *tags, struct DosLibraryV0 *DOSBaseV0)
{

    LONG childprocessidx;

    childprocessidx = childprocess_getslot();

    if (childprocessidx == -1)
unhandledCodePath(__func__, "Out of child processes", 0, 0);

    struct CreateNewProcMsg *msg = (struct CreateNewProcMsg *)AllocMem(sizeof(struct CreateNewProcMsg), MEMF_CLEAR);
    msg->cnp_Slot = childprocessidx;

    struct TagItem *tagListNative = CloneTagItemsV02Native(tags);

    struct TagItem *tagNative = tagListNative;

    while (tagNative->ti_Tag != TAG_DONE)
    {
        switch(tagNative->ti_Tag)
        {
            case(NP_Name):
                bug("Process: %s\n", tagNative->ti_Data);
                STRPTR p = (STRPTR)tagNative->ti_Data;
                if (p[0] == 'W' && p[1] == 'o')
                    return (APTR)0x1; // Disable "Workbench Handler"
                if (p[0] == 'N' && p[10] == 'c')
                    return (APTR)0x1; // Disable "NList.mcc clipboard server"
                if (p[0] == 'N' && p[14] == 'c')
                    return (APTR)0x1; // Disable "NListtree.mcc clipboard server"
                if (p[0] == 'T' && p[15] == 'c')
                    return (APTR)0x1; // Disable "TextEditor.mcc clipboard server"

                break;
            case(NP_StackSize):
                msg->cnp_StackSize = tagNative->ti_Data;
                break;
            case(NP_Entry):
                msg->cnp_Entry = (APTR32)tagNative->ti_Data;
                tagNative->ti_Data = (IPTR)createNewProc_trampoline;
                break;
            case NP_Output:
            {
                if (tagNative->ti_Data)
                {
                    BPTR fh = (BPTR)tagNative->ti_Data;
                    struct FileHandleProxy *fhproxy = (struct FileHandleProxy *)fh;
                    tagNative->ti_Data = (IPTR)fhproxy->native;
                }
                break;
            }
            case NP_Arguments:
            case NP_UserData:
            case NP_CloseOutput:
            case NP_Priority:
            case TAG_IGNORE:
                break;
            default:
unhandledCodePath(__func__, "Tag", 0, tagNative->ti_Tag);
        }
        tagNative++;
    }

    struct Process *p = CreateNewProc(tagListNative);
    g_nativechildprocesses[childprocessidx] = (struct Task *)p;
    struct ProcessV0 *pV0 = (struct ProcessV0 *)abiv0_AllocMem(sizeof(struct ProcessV0), MEMF_CLEAR, DOS_SysBaseV0);

    pV0->pr_MsgPort.mp_SigBit = p->pr_MsgPort.mp_SigBit;
    NEWLISTV0(&pV0->pr_MsgPort.mp_MsgList);
    MsgPortV0_fixed_connectnative(&pV0->pr_MsgPort, &p->pr_MsgPort);
    pV0->pr_Task.tc_UserData = (APTR32)(IPTR)p->pr_Task.tc_UserData;

    g_v0childprocesses[childprocessidx] = pV0;

    PutMsg(&p->pr_MsgPort, (struct Message *)msg); /* Allow child process to proceed */

bug("abiv0_CreateNewProc: STUB\n");
    return g_v0childprocesses[childprocessidx];
}
MAKE_PROXY_ARG_2(CreateNewProc)

LONG abiv0_FPutC(BPTR file, LONG character, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileHandleProxy *proxy = (struct FileHandleProxy *)file;
    return FPutC(proxy->native, character);
}
MAKE_PROXY_ARG_3(FPutC)

LONG abiv0_FGetC(BPTR file, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileHandleProxy *fhp = (struct FileHandleProxy *)file;
    return FGetC(fhp->native);
}
MAKE_PROXY_ARG_2(FGetC)

LONG abiv0_UnGetC(BPTR file, LONG character, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileHandleProxy *fhp = (struct FileHandleProxy *)file;
    return UnGetC(fhp->native, character);
}
MAKE_PROXY_ARG_3(UnGetC)

BPTR abiv0_Open(CONST_STRPTR name, LONG accessMode, struct DosLibraryV0 *DOSBaseV0)
{
    if (strcmp(name, "LIBS:Zune/TheBar.mcc") == 0)
        name = "LIBSV0:Zune/TheBar.mcc";

    BPTR tmp = Open(name, accessMode);

    if (tmp == BNULL)
        return BNULL;

    struct FileHandleProxy *fhp = abiv0_AllocMem(sizeof(struct FileHandleProxy), MEMF_ANY, DOS_SysBaseV0);
    fhp->native = tmp;
    return (BPTR)fhp;
}
MAKE_PROXY_ARG_3(Open)

LONG abiv0_IsInteractive(BPTR file, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileHandleProxy *fhp = (struct FileHandleProxy *)file;
    return IsInteractive(fhp->native);
}
MAKE_PROXY_ARG_2(IsInteractive)

LONG abiv0_Read(BPTR file, APTR buffer, LONG length, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileHandleProxy *fhp = (struct FileHandleProxy *)file;
    return Read(fhp->native, buffer, length);
}
MAKE_PROXY_ARG_4(Read)

LONG abiv0_Write(BPTR file, APTR buffer, LONG length, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileHandleProxy *fhp = (struct FileHandleProxy *)file;
    return Write(fhp->native, buffer, length);
}
MAKE_PROXY_ARG_4(Write)

LONG  abiv0_FRead(BPTR fh, APTR block, ULONG blocklen, ULONG number, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileHandleProxy *fhp = (struct FileHandleProxy *)fh;
    return FRead(fhp->native, block, blocklen, number);
}
MAKE_PROXY_ARG_5(FRead)

LONG abiv0_FWrite(BPTR fh, CONST_APTR block, ULONG blocklen, ULONG numblocks, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileHandleProxy *fhp = (struct FileHandleProxy *)fh;
    return FWrite(fhp->native, block, blocklen, numblocks);
}
MAKE_PROXY_ARG_5(FWrite)

LONG abiv0_Seek(BPTR file, LONG position, LONG mode, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileHandleProxy *fhproxy = (struct FileHandleProxy *)file;
    return Seek(fhproxy->native, position, mode);
}
MAKE_PROXY_ARG_4(Seek)

BOOL abiv0_Close(BPTR file, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileHandleProxy *fhp = (struct FileHandleProxy *)file;
    struct FileLock *fl = (struct FileLock *)BADDR(fhp->native);

    if (fhp->native == NULL)
    {
        bug("FIXME: NULL native FH in Close!!\n");
        return TRUE;
    }

    /* ABI_V0 compatibility */
    /* Up to 2010-12-03 UnLock was an alias/define to Close */
    if ((fl->fl_Access  == SHARED_LOCK) ||
            (fl->fl_Access == EXCLUSIVE_LOCK))
    {
        return UnLock(fhp->native);
    }

    return Close(fhp->native);
}
MAKE_PROXY_ARG_2(Close)

LONG abiv0_SetFileSize(BPTR file, LONG offset, LONG mode, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileHandleProxy *fhp = (struct FileHandleProxy *)file;
    return SetFileSize(fhp->native, offset, mode);
}
MAKE_PROXY_ARG_4(SetFileSize)

LONG abiv0_SetProtection(CONST_STRPTR name, ULONG protect, struct DosLibraryV0 *DOSBaseV0)
{
    return SetProtection(name, protect);
}
MAKE_PROXY_ARG_3(SetProtection)

static struct FileLockProxy *makeFileLockProxy(BPTR native)
{
    struct FileLockProxy *proxy = abiv0_AllocMem(sizeof(struct FileLockProxy), MEMF_CLEAR, DOS_SysBaseV0);
    proxy->native = native;
    return proxy;
}

BPTR abiv0_DupLockFromFH(BPTR handle, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileHandleProxy *fhp = (struct FileHandleProxy *)handle;
    BPTR dup = DupLockFromFH(fhp->native);
    return (BPTR)makeFileLockProxy(dup);
}

MAKE_PROXY_ARG_2(DupLockFromFH)

BOOL abiv0_NameFromFH(BPTR fh, STRPTR buffer, LONG length, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileHandleProxy *fhp = (struct FileHandleProxy *)fh;
    return NameFromFH(fhp->native, buffer, length);
}
MAKE_PROXY_ARG_4(NameFromFH)

BOOL abiv0_IsFileSystem(CONST_STRPTR devicename, struct DosLibraryV0 *DOSBaseV0)
{
    return IsFileSystem(devicename);
}
MAKE_PROXY_ARG_2(IsFileSystem)

extern struct TaskV0 *g_v0maintask;
extern struct Task *g_nativemaintask;

static APTR lock_WindowPtrEnter(struct Task *me)
{
    struct ProcessV0 *meV0 = NULL;
    if (me == g_nativemaintask)
        meV0 = (struct ProcessV0 *)g_v0maintask;
    if (!meV0)
        meV0 = (struct ProcessV0 *)childprocess_getbynative(me);

    if (!meV0)  unhandledCodePath(__func__, "!meV0", 0, 0);

    if (meV0->pr_WindowPtr == (APTR32)-1)
    {
        struct Process *p = (struct Process *)me;
        APTR window = p->pr_WindowPtr;
        p->pr_WindowPtr = (APTR)-1;
        return window;
    }

    return NULL;
}

static void lock_WindowPtrExit(APTR window, struct Task *me)
{
    struct Process *p = (struct Process *)me;
    if (window && p->pr_WindowPtr == (APTR)-1)
        p->pr_WindowPtr = window;
}

BPTR abiv0_Lock(CONST_STRPTR name, LONG accessMode, struct DosLibraryV0 *DOSBaseV0)
{
    BPTR _ret = BNULL;
    APTR window = NULL;
    struct Task *me = FindTask(NULL);

    /* Workaround for Hollywood looking for it's plugins in LIBS:Hollywood */
    if (strcmp(name, "LIBS:Hollywood") == 0)
        name = "LIBSV0:Hollywood";

    /* Workaround for client code setting pr_WindowPtr to -1 before calling Lock() */
    window = lock_WindowPtrEnter(me);

    BPTR tmp = Lock(name, accessMode);

    lock_WindowPtrExit(window, me);

    if (tmp != BNULL)
        _ret = (BPTR)makeFileLockProxy(tmp);

    return _ret;
}
MAKE_PROXY_ARG_3(Lock)

BPTR abiv0_CreateDir(CONST_STRPTR name, struct DosLibraryV0 *DOSBaseV0)
{
    return (BPTR)makeFileLockProxy(CreateDir(name));
}
MAKE_PROXY_ARG_2(CreateDir)

BOOL abiv0_GetCurrentDirName(STRPTR buf, LONG len, struct DosLibraryV0 *DOSBaseV0)
{
    return GetCurrentDirName(buf, len);
}
MAKE_PROXY_ARG_3(GetCurrentDirName)

struct DevProcProxy
{
    struct DevProcV0 base;
    struct DevProc *native;
};
struct DevProcV0 *abiv0_GetDeviceProc(CONST_STRPTR name, struct DevProcV0 *dp, struct DosLibraryV0 *DOSBaseV0)
{
    struct DevProc *nativedp = NULL;
    if (dp) nativedp = ((struct DevProcProxy *)dp)->native;
    struct DevProc *nativeret = GetDeviceProc(name, nativedp);
    if (nativeret == NULL)
        return NULL;

    struct DevProcProxy *proxy = abiv0_AllocMem(sizeof(struct DevProcProxy), MEMF_CLEAR, DOS_SysBaseV0);
    proxy->base.dvp_Lock = (BPTR32)(IPTR)makeFileLockProxy(nativeret->dvp_Lock);
    proxy->native = nativeret;
    return (struct DevProcV0 *)proxy;
}
MAKE_PROXY_ARG_3(GetDeviceProc)

void abiv0_FreeDeviceProc(struct DevProcV0 *dp, struct DosLibraryV0 *DOSBaseV0)
{
    if (dp)
    {
        struct DevProcProxy *proxy = (struct DevProcProxy *)dp;
        FreeDeviceProc(proxy->native);
        abiv0_FreeMem(proxy, sizeof(struct DevProcProxy), DOS_SysBaseV0);
    }
}
MAKE_PROXY_ARG_2(FreeDeviceProc)

BPTR abiv0_DupLock(BPTR lock, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileLockProxy *proxy = (struct FileLockProxy *)lock;
    BPTR dup = DupLock(proxy->native);
    return (BPTR)makeFileLockProxy(dup);
    /* ABI_V0 compatibility MISSING*/
    /* Up to 2010-12-03 DupLockFromFH was an alias/define to DupLock */
}
MAKE_PROXY_ARG_2(DupLock)

BOOL abiv0_NameFromLock(BPTR lock, STRPTR buffer, LONG length, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileLockProxy *proxy = (struct FileLockProxy *)lock;
    /* ABI_V0 compatibility */
    /* Up to 2010-12-03 NameFromFH was an alias/define to NameFromLock. Example: HFinder */
    struct FileLock *fl = (struct FileLock *)BADDR(proxy->native);
    if ((fl->fl_Access != SHARED_LOCK) && (fl->fl_Access != EXCLUSIVE_LOCK))
        return NameFromFH(proxy->native, buffer, length);

    return NameFromLock(proxy->native, buffer, length);
}
MAKE_PROXY_ARG_4(NameFromLock)

LONG abiv0_SameLock(BPTR lock1, BPTR lock2, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileLockProxy *proxy1 = (struct FileLockProxy *)lock1;
    struct FileLockProxy *proxy2 = (struct FileLockProxy *)lock2;
    return SameLock(proxy1->native, proxy2->native);
}
MAKE_PROXY_ARG_3(SameLock)

BOOL abiv0_UnLock(BPTR lock, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileLockProxy *proxy = (struct FileLockProxy *)lock;
    return UnLock(proxy->native);
}
MAKE_PROXY_ARG_2(UnLock)

LONG abiv0_AssignLock(CONST_STRPTR name, BPTR lock, struct DosLibraryV0 *DOSBaseV0)
{
    if (lock != BNULL)
    {
        struct FileLockProxy *proxy = (struct FileLockProxy *)lock;
        return AssignLock(name, proxy->native);
    }

    return AssignLock(name, BNULL);
}
MAKE_PROXY_ARG_3(AssignLock)

BOOL abiv0_DeleteFile(CONST_STRPTR name, struct DosLibraryV0 *DOSBaseV0)
{
    DeleteFile(name);
}
MAKE_PROXY_ARG_2(DeleteFile)

BPTR abiv0_ParentDir(BPTR lock, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileLockProxy *proxy = (struct FileLockProxy *)lock;
    BPTR plock = ParentDir(proxy->native);
    return (BPTR)makeFileLockProxy(plock);
}
MAKE_PROXY_ARG_2(ParentDir)

/* In MatchFirst/MatchNext FIB is a fields of structure, not allocated via DOS */
#define FIB_PROXY_MAGIC (0x05060708)
struct FileInfoBlock *getNativeFIB(struct FileInfoBlockV0 *v0fib)
{
    if (v0fib->fib_DiskKey != FIB_PROXY_MAGIC)
    {
        struct FileInfoBlock *nativefib = AllocDosObject(DOS_FIB, NULL);
        v0fib->fib_DiskKey = FIB_PROXY_MAGIC;
        *((IPTR *)(&v0fib->fib_Reserved[24])) = (IPTR)nativefib;
    }

    return (struct FileInfoBlock *)*(IPTR *)(&v0fib->fib_Reserved[24]);
}

struct ExAllControlProxy
{
    struct ExAllControlV0 base;
    struct ExAllControl *native;
};

APTR abiv0_AllocDosObject(ULONG type, const struct TagItemV0 * tags, struct DosLibraryV0 *DOSBaseV0)
{
    if (type == DOS_FIB && tags == NULL)
    {
        struct FileInfoBlockV0 *v0fib = abiv0_AllocMem(sizeof(struct FileInfoBlockV0), MEMF_CLEAR, DOS_SysBaseV0);
        return v0fib;
    }
    else if (type == DOS_EXALLCONTROL && tags == NULL)
    {
        struct ExAllControl *eac = AllocDosObject(type, NULL);
        struct ExAllControlProxy *proxy = abiv0_AllocMem(sizeof(struct ExAllControlProxy), MEMF_CLEAR, DOS_SysBaseV0);
        proxy->native = eac;
        return proxy;
    } else if (type == DOS_RDARGS && tags == NULL)
    {
        APTR _ret;
        /* Call original function */
        CALL32_ARG_3(_ret, dosfunctable[37], type, tags, DOSBaseV0);
        return _ret;
    }
unhandledCodePath(__func__, "type/tags", type, (ULONG)(IPTR)tags);
}
MAKE_PROXY_ARG_3(AllocDosObject)

void abiv0_FreeDosObject(ULONG type, APTR ptr, struct DosLibraryV0 *DOSBaseV0)
{
    if (type == DOS_FIB)
    {
        struct FileInfoBlockV0 *v0fib = (struct FileInfoBlockV0 *)ptr;
        FreeDosObject(type, getNativeFIB(v0fib));
        abiv0_FreeMem(v0fib, sizeof(struct FileInfoBlockV0), DOS_SysBaseV0);
        return;
    } else if (type == DOS_EXALLCONTROL)
    {
        struct ExAllControlProxy *proxy = (struct ExAllControlProxy *)ptr;
        FreeDosObject(type, proxy->native);
        abiv0_FreeMem(proxy, sizeof(struct ExAllControlProxy), DOS_SysBaseV0);
        return;
    } else if (type == DOS_RDARGS)
    {
        /* Call original function */
        CALL32_ARG_3_NR(dosfunctable[38], type, ptr, DOSBaseV0);
        return;
    }
unhandledCodePath(__func__, "type", type, 0);
}
MAKE_PROXY_ARG_3(FreeDosObject)

LONG abiv0_Examine(BPTR lock, struct FileInfoBlockV0 *fib, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileLockProxy *flproxy = (struct FileLockProxy *)lock;
    struct FileInfoBlock *fibnative = getNativeFIB(fib);
    LONG res;

    /* ABI_V0 compatibility */
    /* Up to 2010-12-03 ExamineFH was an alias/define to Examine. Example Crossboard_Live.hwa */
    struct FileLock *fl = (struct FileLock *)BADDR(flproxy->native);
    if ((fl->fl_Access != SHARED_LOCK) && (fl->fl_Access != EXCLUSIVE_LOCK))
        res = ExamineFH(flproxy->native, fibnative);
    else
        res = Examine(flproxy->native, fibnative);

    if (res)
    {
        fib->fib_Date           = fibnative->fib_Date;
        fib->fib_Size           = fibnative->fib_Size;
        fib->fib_DirEntryType   = fibnative->fib_DirEntryType;
        CopyMem(fibnative->fib_FileName, fib->fib_FileName, 108);
bug("abiv0_Examine: STUB\n");
    }
    return res;
}
MAKE_PROXY_ARG_3(Examine)

LONG abiv0_ExNext(BPTR lock, struct FileInfoBlockV0 *fileInfoBlock, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileLockProxy *flproxy = (struct FileLockProxy *)lock;
    struct FileInfoBlock *fibnative = getNativeFIB(fileInfoBlock);

    LONG res = ExNext(flproxy->native, fibnative);
    if (res)
    {
        fileInfoBlock->fib_Date         = fibnative->fib_Date;
        fileInfoBlock->fib_Size         = fibnative->fib_Size;
        fileInfoBlock->fib_DirEntryType = fibnative->fib_DirEntryType;
        CopyMem(fibnative->fib_FileName, fileInfoBlock->fib_FileName, 108);
// bug("abiv0_ExNext: STUB\n");
    }
    return res;
}
MAKE_PROXY_ARG_3(ExNext)

BOOL abiv0_ExamineFH(BPTR fh, struct FileInfoBlockV0 *fib, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileHandleProxy *fhproxy = (struct FileHandleProxy *)fh;
    struct FileInfoBlock *fibnative = getNativeFIB(fib);
    BOOL res = ExamineFH(fhproxy->native, fibnative);
    if (res)
    {
        fib->fib_Date = fibnative->fib_Date;
        fib->fib_Size = fibnative->fib_Size;
bug("abiv0_ExamineFH: STUB\n");
    }
    return res;
}
MAKE_PROXY_ARG_3(ExamineFH)

LONG abiv0_Info(BPTR lock,  struct InfoData * parameterBlock, struct DosLibraryV0 *DOSBaseV0)
{
bug("abiv0_Info: STUB\n");
    return 0;
}
MAKE_PROXY_ARG_3(Info)

LONG abiv0_Flush(BPTR file, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileHandleProxy *fhproxy = (struct FileHandleProxy *)file;
    return Flush(fhproxy->native);

}
MAKE_PROXY_ARG_2(Flush)

BPTR abiv0_CurrentDir(BPTR lock, struct DosLibraryV0 *DOSBaseV0)
{
    BPTR old;

    if (lock != BNULL)
    {
        struct FileLockProxy *flproxy = (struct FileLockProxy *)lock;
        old = CurrentDir(flproxy->native);
    }
    else
    {
        old = CurrentDir(BNULL);
    }
    return (BPTR)makeFileLockProxy(old);
}
MAKE_PROXY_ARG_2(CurrentDir)

BOOL abiv0_ExAll(BPTR lock, struct ExAllDataV0 *buffer, LONG size, LONG type, struct ExAllControlV0 *control, struct DosLibraryV0 *DOSBaseV0)
{
    struct ExAllControlProxy *eacproxy = (struct ExAllControlProxy *)control;
    struct FileHandleProxy *fhproxy = (struct FileHandleProxy *)lock;

if (type > ED_DATE)  unhandledCodePath(__func__, "type", type, 0);

    if (eacproxy->base.eac_LastKey == 0)
    {
        eacproxy->native->eac_LastKey = 0;
        eacproxy->base.eac_LastKey = 0xbaadf00d;
    }

    APTR buffernative = AllocMem(size, MEMF_CLEAR);

    BOOL res = ExAll(fhproxy->native, buffernative , size, type, eacproxy->native);
    eacproxy->base.eac_Entries = eacproxy->native->eac_Entries;

    if (eacproxy->native->eac_Entries > 0)
    {
        struct ExAllData *eadnativeit = (struct ExAllData *)buffernative;
        struct ExAllDataV0 *writeit = buffer;
        APTR writenext = NULL;
        APTR writename = NULL;
        for (; eadnativeit != NULL; eadnativeit = eadnativeit->ed_Next)
        {
            int namelen = strlen(eadnativeit->ed_Name) + 1;
            writename = (APTR)writeit + 1 * 4; // ed_Next
            if (type >= ED_NAME) writename += 1 * 4; // ed_Name
            if (type >= ED_TYPE) writename += 1 * 4; // ed_Type
            if (type >= ED_SIZE) writename += 1 * 4; // ed_Size
            if (type >= ED_PROTECTION) writename += 1 * 4; // ed_Prot
            if (type >= ED_DATE) writename += 3 * 4; // ed_Days, ed_Mins, ed_Ticks

            if (eadnativeit->ed_Next == NULL)
                writenext = NULL;
            else
                writenext = writename + namelen;

            writeit->ed_Next    = (APTR32)(IPTR)writenext;
            if (type >= ED_NAME)
                writeit->ed_Name    = (APTR32)(IPTR)writename;
            if (type >= ED_TYPE)
                writeit->ed_Type    = eadnativeit->ed_Type;
            if (type >= ED_SIZE)
                writeit->ed_Size    = eadnativeit->ed_Size;
            if (type >= ED_PROTECTION)
                writeit->ed_Prot    = eadnativeit->ed_Prot;
            if (type >= ED_DATE)
            {
                writeit->ed_Days    = eadnativeit->ed_Days;
                writeit->ed_Mins    = eadnativeit->ed_Mins;
                writeit->ed_Ticks   = eadnativeit->ed_Ticks;
            }
            // ed_Comment, ed_OwnerUID, ed_OwnerGUI missing
            if (type >= ED_NAME)
                CopyMem(eadnativeit->ed_Name, writename, namelen);

            writeit = (struct ExAllDataV0 *)writenext;
        }
    }

bug("abiv0_ExAll: STUB\n");
    FreeMem(buffernative, size);
    return res;
}
MAKE_PROXY_ARG_6(ExAll)

struct DosListV0 *abiv0_LockDosList(ULONG flags, struct DosLibraryV0 *DOSBaseV0)
{
    struct DosList *native = LockDosList(flags);

    if (native)
    {
        struct DosListProxy *proxy = abiv0_AllocMem(sizeof(struct DosListProxy), MEMF_CLEAR, DOS_SysBaseV0);
        proxy->base.dol_Type = native->dol_Type;
        proxy->native = native;
bug("abiv0_LockDosList: STUB\n");
        return (struct DosListV0 *)proxy;
    }

    return NULL;
}
MAKE_PROXY_ARG_2(LockDosList)

struct DosListV0 *abiv0_NextDosEntry(struct DosListV0 *dlist, ULONG flags, struct DosLibraryV0 *DOSBaseV0)
{
    struct DosListProxy *proxy = (struct DosListProxy *)dlist;
    struct DosList *native = NextDosEntry(proxy->native, flags);

    if (native)
    {
        struct DosListProxy *proxy = abiv0_AllocMem(sizeof(struct DosListProxy), MEMF_CLEAR, DOS_SysBaseV0);
        proxy->base.dol_Type = native->dol_Type;
        proxy->base.dol_Task = (APTR32)(IPTR)native->dol_Task; /* treat this as just a "marker" for now */

        LONG nlen = AROS_BSTR_strlen(native->dol_Name);
        char *v0name = abiv0_AllocMem(nlen + 1, MEMF_CLEAR, DOS_SysBaseV0);
        CopyMem(native->dol_Name, v0name, nlen + 1);
        proxy->base.dol_Name = (APTR32)(IPTR)v0name;

        proxy->native = native;

bug("abiv0_NextDosEntry: STUB\n");

        return (struct DosListV0 *)proxy;
    }

    return NULL;
}
MAKE_PROXY_ARG_3(NextDosEntry)

struct DosListV0 *abiv0_FindDosEntry(struct DosListV0 *dlist, CONST_STRPTR name, ULONG flags, struct DosLibraryV0 *DOSBaseV0)
{
    struct DosListProxy *proxy = (struct DosListProxy *)dlist;
    struct DosList *native = FindDosEntry(proxy->native, name, flags);

    if (native)
    {
        struct DosListProxy *proxy = abiv0_AllocMem(sizeof(struct DosListProxy), MEMF_CLEAR, DOS_SysBaseV0);
        proxy->base.dol_Type = native->dol_Type;
        proxy->base.dol_Task = (APTR32)(IPTR)native->dol_Task; /* treat this as just a "marker" for now */

        LONG nlen = AROS_BSTR_strlen(native->dol_Name);
        char *v0name = abiv0_AllocMem(nlen + 1, MEMF_CLEAR, DOS_SysBaseV0);
        CopyMem(native->dol_Name, v0name, nlen + 1);
        proxy->base.dol_Name = (APTR32)(IPTR)v0name;

        proxy->native = native;

bug("abiv0_FindDosEntry: STUB\n");

        return (struct DosListV0 *)proxy;
    }

    return NULL;
}
MAKE_PROXY_ARG_4(FindDosEntry)

void abiv0_UnLockDosList(ULONG flags, struct DosLibraryV0 *DOSBaseV0)
{
    UnLockDosList(flags);
}
MAKE_PROXY_ARG_2(UnLockDosList);

SIPTR abiv0_IoErr(struct DosLibraryV0 *DOSBaseV0)
{
// possibly copy Result2 to task proxy as well
    return IoErr();
}
MAKE_PROXY_ARG_1(IoErr)

LONG abiv0_SystemTagList(CONST_STRPTR command, const struct TagItemV0 *tags, struct DosLibraryV0 *DOSBaseV0)
{
    struct TagItem *tagListNative = CloneTagItemsV02Native(tags);

    struct TagItem *tagNative = tagListNative;

    while (tagNative->ti_Tag != TAG_DONE)
    {
        if (tagNative->ti_Tag == SYS_Dummy + 10) /* ABI_V0 compatibility: SYS_Error had value SYS_Dummy + 10 */
            tagNative->ti_Tag = SYS_Error;

        switch(tagNative->ti_Tag)
        {
            case(SYS_Input):
            case(SYS_Output):
            case(SYS_ScriptInput):
            case SYS_Error:
            {
                if (tagNative->ti_Data)
                {
                    BPTR fh = (BPTR)tagNative->ti_Data;
                    struct FileHandleProxy *fhproxy = (struct FileHandleProxy *)fh;
                    tagNative->ti_Data = (IPTR)fhproxy->native;
                }
                break;
            }
            case SYS_UserShell:
            case SYS_Background:
            case SYS_Asynch:
                break;
            default:
 unhandledCodePath(__func__, "Tag", 0, tagNative->ti_Tag);
        }

        tagNative++;
    }

bug("abiv0_SystemTagList: STUB\n");
    return SystemTagList(command, tagListNative);
}
MAKE_PROXY_ARG_3(SystemTagList)

LONG abiv0_CheckSignal(LONG mask, struct DosLibraryV0 *DOSBaseV0)
{
    return CheckSignal(mask);
}
MAKE_PROXY_ARG_2(CheckSignal)

BOOL abiv0_SetVar(CONST_STRPTR name, CONST_STRPTR buffer, LONG size, LONG flags, struct DosLibraryV0 *DOSBaseV0)
{
    return SetVar(name, buffer, size, flags);
}
MAKE_PROXY_ARG_6(SetVar)

LONG abiv0_GetVar(CONST_STRPTR name, STRPTR buffer, LONG size, LONG flags, struct DosLibraryV0 *DOSBaseV0)
{
    return GetVar(name, buffer, size, flags);
}
MAKE_PROXY_ARG_5(GetVar)

void abiv0_Delay(ULONG timeout, struct DosLibraryV0 *DOSBaseV0)
{
    Delay(timeout);
}
MAKE_PROXY_ARG_2(Delay)

LONG abiv0_SetComment(CONST_STRPTR name, CONST_STRPTR comment, struct DosLibraryV0 *DOSBaseV0)
{
    return SetComment(name, comment);
}
MAKE_PROXY_ARG_3(SetComment)

BOOL abiv0_SetFileDate(CONST_STRPTR name, const struct DateStamp *date, struct DosLibraryV0 *DOSBaseV0)
{
    return SetFileDate(name, date);
}
MAKE_PROXY_ARG_3(SetFileDate)

#include <proto/utility.h>

BPTR abiv0_LoadSeg(CONST_STRPTR name, struct DosLibraryV0 *DOSBaseV0)
{
    TEXT tmp[128]; // Assumes 32-bit stack only
    /* Workaround for Hollywood looking for it's plugins in LIBS:Hollywood */
    if (strstr(name, "LIBS:Hollywood") == (char *)name)
        SNPrintf(tmp, 128, "%s%s", "LIBSV0", (name + 4));
    else
        SNPrintf(tmp, 128, "%s", name);
    CONST_STRPTR p = tmp;

    BPTR _ret = BNULL;
    /* Call original function */
    CALL32_ARG_2(_ret, dosfunctable[24], p , DOSBaseV0);

    return _ret;
}
MAKE_PROXY_ARG_2(LoadSeg)

struct IntDosBaseV0
{
    struct DosLibraryV0         pub;
    APTR32                      debugBase;
    BYTE                        rootNode[108]  __attribute__((aligned(4)));
    BYTE                        errors[8]  __attribute__((aligned(4)));
    struct SignalSemaphoreV0    segsem;
    struct ListV0               segdata;
};

BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);
APTR abiv0_DOS_OpenLibrary(CONST_STRPTR name, ULONG version, struct ExecBaseV0 *SysBaseV0);

extern ULONG *seginitlist;
BPTR dosseg;

void Dos_Unhandled_init(struct LibraryV0 *abiv0DOSBase);

void init_dos(struct ExecBaseV0 *SysBaseV0)
{
    TEXT path[64];
    NewRawDoFmt("LIBSV0:partial/dos.library", RAWFMTFUNC_STRING, path);
    /* Keep it! This fills global variable */
    dosseg = LoadSeg32(path, DOSBase);

    APTR tmp = AllocMem(2048, MEMF_31BIT | MEMF_CLEAR);
    abiv0DOSBase = (tmp + 1024);
    abiv0DOSBase->dl_lib.lib_Node.ln_Name = 0x0000D0FF;
    abiv0DOSBase->dl_lib.lib_Version = DOSBase->dl_lib.lib_Version;
    abiv0DOSBase->dl_TimeReq = (APTR32)(IPTR)AllocMem(sizeof(struct timerequestV0), MEMF_31BIT | MEMF_CLEAR);
    ((struct timerequestV0 *)(IPTR)abiv0DOSBase->dl_TimeReq)->tr_node.io_Device = (APTR32)(IPTR)abiv0TimerBase;
    DOS_SysBaseV0 = SysBaseV0;

    /* Call SysBase_autoinit */
    __asm__ volatile (
        "subq $4, %%rsp\n"
        "movl %0, %%eax\n"
        "movl %%eax, (%%rsp)\n"
        "movl %1, %%eax\n"
        ENTER32
        "call *%%eax\n"
        ENTER64
        "addq $4, %%rsp\n"
        ::"m"(SysBaseV0), "m"(seginitlist[1])
        : SCRATCH_REGS_64_TO_32 );

    abiv0DOSBase->dl_UtilityBase = (APTR32)(IPTR)abiv0_DOS_OpenLibrary("utility.library", 0L, SysBaseV0);
    NEWLISTV0(&((struct IntDosBaseV0 *)abiv0DOSBase)->segdata);
    abiv0_InitSemaphore(&((struct IntDosBaseV0 *)abiv0DOSBase)->segsem, SysBaseV0);

    /* Set all unhandled LVO addresses to a catch function */
    Dos_Unhandled_init((struct LibraryV0 *)abiv0DOSBase);

    /* Set all working LVOs */
    __AROS_SETVECADDRV0(abiv0DOSBase, 158, (APTR32)(IPTR)proxy_PutStr);
    __AROS_SETVECADDRV0(abiv0DOSBase,   9, dosfunctable[8]);    // Input
    __AROS_SETVECADDRV0(abiv0DOSBase,  10, dosfunctable[9]);    // Output
    __AROS_SETVECADDRV0(abiv0DOSBase,  61, (APTR32)(IPTR)proxy_SetVBuf);
    __AROS_SETVECADDRV0(abiv0DOSBase,  32, dosfunctable[31]);   // DateStamp
    __AROS_SETVECADDRV0(abiv0DOSBase,  82, dosfunctable[81]);   // Cli
    __AROS_SETVECADDRV0(abiv0DOSBase, 159, dosfunctable[158]);  // VPrintf
    __AROS_SETVECADDRV0(abiv0DOSBase,  52, (APTR32)(IPTR)proxy_FPutC);
    __AROS_SETVECADDRV0(abiv0DOSBase,  83, (APTR32)(IPTR)proxy_CreateNewProc);
    __AROS_SETVECADDRV0(abiv0DOSBase,  77, dosfunctable[76]);   // SetIoErr
    __AROS_SETVECADDRV0(abiv0DOSBase,   5, (APTR32)(IPTR)proxy_Open);
    __AROS_SETVECADDRV0(abiv0DOSBase, 133, dosfunctable[132]);  // ReadArgs
    __AROS_SETVECADDRV0(abiv0DOSBase,  36, (APTR32)(IPTR)proxy_IsInteractive);
    __AROS_SETVECADDRV0(abiv0DOSBase,  56, dosfunctable[55]);   // FGets
    __AROS_SETVECADDRV0(abiv0DOSBase,  51, (APTR32)(IPTR)proxy_FGetC);
    __AROS_SETVECADDRV0(abiv0DOSBase, 135, dosfunctable[134]);  // ReadItem
    __AROS_SETVECADDRV0(abiv0DOSBase, 143, dosfunctable[142]);  // FreeArgs
    __AROS_SETVECADDRV0(abiv0DOSBase,   7, (APTR32)(IPTR)proxy_Read);
    __AROS_SETVECADDRV0(abiv0DOSBase,  11, (APTR32)(IPTR)proxy_Seek);
    __AROS_SETVECADDRV0(abiv0DOSBase,   6, (APTR32)(IPTR)proxy_Close);
    __AROS_SETVECADDRV0(abiv0DOSBase, 145, dosfunctable[144]);  // FilePart
    __AROS_SETVECADDRV0(abiv0DOSBase, 100, dosfunctable[ 99]);  // GetProgramDir
    __AROS_SETVECADDRV0(abiv0DOSBase,  14, (APTR32)(IPTR)proxy_Lock);
    __AROS_SETVECADDRV0(abiv0DOSBase, 107, (APTR32)(IPTR)proxy_GetDeviceProc);
    __AROS_SETVECADDRV0(abiv0DOSBase,  16, (APTR32)(IPTR)proxy_DupLock);
    __AROS_SETVECADDRV0(abiv0DOSBase,  38, (APTR32)(IPTR)proxy_AllocDosObject);
    __AROS_SETVECADDRV0(abiv0DOSBase,  17, (APTR32)(IPTR)proxy_Examine);
    __AROS_SETVECADDRV0(abiv0DOSBase,  21, (APTR32)(IPTR)proxy_CurrentDir);
    __AROS_SETVECADDRV0(abiv0DOSBase,  72, (APTR32)(IPTR)proxy_ExAll);
    __AROS_SETVECADDRV0(abiv0DOSBase,  22, (APTR32)(IPTR)proxy_IoErr);
    __AROS_SETVECADDRV0(abiv0DOSBase,  39, (APTR32)(IPTR)proxy_FreeDosObject);
    __AROS_SETVECADDRV0(abiv0DOSBase, 108, (APTR32)(IPTR)proxy_FreeDeviceProc);
    __AROS_SETVECADDRV0(abiv0DOSBase, 123, dosfunctable[122]);  // CompareDates
    __AROS_SETVECADDRV0(abiv0DOSBase,  65, (APTR32)(IPTR)proxy_ExamineFH);
    __AROS_SETVECADDRV0(abiv0DOSBase,  60, (APTR32)(IPTR)proxy_Flush);
    __AROS_SETVECADDRV0(abiv0DOSBase,  25, (APTR32)(IPTR)proxy_LoadSeg);
    __AROS_SETVECADDRV0(abiv0DOSBase, 126, dosfunctable[125]);  // InternalLoadSeg
    __AROS_SETVECADDRV0(abiv0DOSBase,  26, dosfunctable[ 25]);  // UnLoadSeg            // This is tricky becaused on LoadSeg32
    __AROS_SETVECADDRV0(abiv0DOSBase, 127, dosfunctable[126]);  // InternalUnLoadSeg
    __AROS_SETVECADDRV0(abiv0DOSBase, 136, dosfunctable[135]);  // StrToLong
    __AROS_SETVECADDRV0(abiv0DOSBase, 134, dosfunctable[133]);  // FindArg
    __AROS_SETVECADDRV0(abiv0DOSBase, 146, dosfunctable[145]);  // PathPart
    __AROS_SETVECADDRV0(abiv0DOSBase,  70, (APTR32)(IPTR)proxy_SameLock);
    __AROS_SETVECADDRV0(abiv0DOSBase,  15, (APTR32)(IPTR)proxy_UnLock);
    __AROS_SETVECADDRV0(abiv0DOSBase, 142, dosfunctable[141]);  // ErrorOutput
    __AROS_SETVECADDRV0(abiv0DOSBase, 118, (APTR32)(IPTR)proxy_IsFileSystem);
    __AROS_SETVECADDRV0(abiv0DOSBase,  67, (APTR32)(IPTR)proxy_NameFromLock);
    __AROS_SETVECADDRV0(abiv0DOSBase, 161, dosfunctable[160]);  // ParsePatternNoCase
    __AROS_SETVECADDRV0(abiv0DOSBase, 109, (APTR32)(IPTR)proxy_LockDosList);
    __AROS_SETVECADDRV0(abiv0DOSBase, 115, (APTR32)(IPTR)proxy_NextDosEntry);
    __AROS_SETVECADDRV0(abiv0DOSBase, 110, (APTR32)(IPTR)proxy_UnLockDosList);
    __AROS_SETVECADDRV0(abiv0DOSBase,  69, dosfunctable[68]);   // SplitName
    __AROS_SETVECADDRV0(abiv0DOSBase, 114, (APTR32)(IPTR)proxy_FindDosEntry);
    __AROS_SETVECADDRV0(abiv0DOSBase,  18, (APTR32)(IPTR)proxy_ExNext);
    __AROS_SETVECADDRV0(abiv0DOSBase, 162, dosfunctable[161]);  // MatchPatternNoCase
    __AROS_SETVECADDRV0(abiv0DOSBase, 147, dosfunctable[146]);  // AddPart
    __AROS_SETVECADDRV0(abiv0DOSBase,  76, (APTR32)(IPTR)proxy_SetFileSize);
    __AROS_SETVECADDRV0(abiv0DOSBase, 101, (APTR32)(IPTR)proxy_SystemTagList);
    __AROS_SETVECADDRV0(abiv0DOSBase,  96, dosfunctable[ 95]);  // GetProgramName
    __AROS_SETVECADDRV0(abiv0DOSBase,  54, (APTR32)(IPTR)proxy_FRead);
    __AROS_SETVECADDRV0(abiv0DOSBase,  35, (APTR32)(IPTR)proxy_ParentDir);
    __AROS_SETVECADDRV0(abiv0DOSBase,  55, (APTR32)(IPTR)proxy_FWrite);
    __AROS_SETVECADDRV0(abiv0DOSBase,  12, (APTR32)(IPTR)proxy_DeleteFile);
    __AROS_SETVECADDRV0(abiv0DOSBase, 137, dosfunctable[136]);  // MatchFirst
    __AROS_SETVECADDRV0(abiv0DOSBase, 138, dosfunctable[137]);  // MatchNext
    __AROS_SETVECADDRV0(abiv0DOSBase, 132, (APTR32)(IPTR)proxy_CheckSignal);
    __AROS_SETVECADDRV0(abiv0DOSBase, 139, dosfunctable[138]);  // MatchEnd
    __AROS_SETVECADDRV0(abiv0DOSBase,  89, dosfunctable[ 88]);  // GetArgStr
    __AROS_SETVECADDRV0(abiv0DOSBase, 151, (APTR32)(IPTR)proxy_GetVar);
    __AROS_SETVECADDRV0(abiv0DOSBase,  33, (APTR32)(IPTR)proxy_Delay);
    __AROS_SETVECADDRV0(abiv0DOSBase,   8, (APTR32)(IPTR)proxy_Write);
    __AROS_SETVECADDRV0(abiv0DOSBase,  20, (APTR32)(IPTR)proxy_CreateDir);
    __AROS_SETVECADDRV0(abiv0DOSBase,  94, (APTR32)(IPTR)proxy_GetCurrentDirName);
    __AROS_SETVECADDRV0(abiv0DOSBase,  31, (APTR32)(IPTR)proxy_SetProtection);
    __AROS_SETVECADDRV0(abiv0DOSBase,  37, dosfunctable[ 36]);  // Execute
    __AROS_SETVECADDRV0(abiv0DOSBase, 102, (APTR32)(IPTR)proxy_AssignLock);
    __AROS_SETVECADDRV0(abiv0DOSBase,  53, (APTR32)(IPTR)proxy_UnGetC);
    __AROS_SETVECADDRV0(abiv0DOSBase, 150, (APTR32)(IPTR)proxy_SetVar);
    __AROS_SETVECADDRV0(abiv0DOSBase,  19, (APTR32)(IPTR)proxy_Info);
    __AROS_SETVECADDRV0(abiv0DOSBase,  68, (APTR32)(IPTR)proxy_NameFromLock);
    __AROS_SETVECADDRV0(abiv0DOSBase,  62, (APTR32)(IPTR)proxy_DupLockFromFH);
    __AROS_SETVECADDRV0(abiv0DOSBase,  30, (APTR32)(IPTR)proxy_SetComment);
    __AROS_SETVECADDRV0(abiv0DOSBase,  66, (APTR32)(IPTR)proxy_SetFileDate);
    __AROS_SETVECADDRV0(abiv0DOSBase,  57, dosfunctable[ 56]);  // FPuts
}

void exit_dos()
{
    FreeMem((APTR)((IPTR)abiv0DOSBase - 1024), 2048);
    UnLoadSeg(dosseg);
}
