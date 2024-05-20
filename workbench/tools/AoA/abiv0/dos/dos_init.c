#include <proto/exec.h>
#include <proto/dos.h>
#include <aros/debug.h>
#include <exec/rawfmt.h>

#include <string.h>

#include "../include/exec/structures.h"
#include "../include/exec/functions.h"
#include "../include/aros/cpu.h"
#include "../include/aros/proxy.h"

#include "../include/dos/structures.h"
#include "../include/utility/structures.h"
#include "../include/timer/structures.h"

struct ExecBaseV0 *DOS_SysBaseV0;
extern struct LibraryV0 *abiv0TimerBase;

struct DosLibraryV0 *abiv0DOSBase;

extern ULONG *dosfunctable;

struct FileLockProxy
{
    BPTR native;
};

struct FileHandleProxy
{
    BPTR native;
};

struct DosListProxy
{
    struct DosListV0    base;
    struct DosList      *native;
};

struct FileHandleProxy *makeFileHandleProxy(BPTR native)
{
    struct FileHandleProxy *proxy = abiv0_AllocMem(sizeof(struct FileHandleProxy), MEMF_ANY, DOS_SysBaseV0);
    proxy->native = native;
    return proxy;
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

struct ProcessV0 *abiv0_CreateNewProc()
{
    return (APTR)0x1;
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

BPTR abiv0_Open(CONST_STRPTR name, LONG accessMode, struct DosLibraryV0 *DOSBaseV0)
{
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

LONG  abiv0_FRead(BPTR fh, APTR block, ULONG blocklen, ULONG number, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileHandleProxy *fhp = (struct FileHandleProxy *)fh;
    return FRead(fhp->native, block, blocklen, number);
}
MAKE_PROXY_ARG_5(FRead)

LONG abiv0_Seek(BPTR file, LONG position, LONG mode, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileHandleProxy *fhproxy = (struct FileHandleProxy *)file;
    return Seek(fhproxy->native, position, mode);
}
MAKE_PROXY_ARG_4(Seek)

BOOL abiv0_Close(BPTR file, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileHandleProxy *fhp = (struct FileHandleProxy *)file;
    return Close(fhp->native);
}
MAKE_PROXY_ARG_2(Close)

LONG abiv0_SetFileSize(BPTR file, LONG offset, LONG mode, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileHandleProxy *fhp = (struct FileHandleProxy *)file;
    return SetFileSize(fhp->native, offset, mode);
}
MAKE_PROXY_ARG_4(SetFileSize)

BOOL abiv0_IsFileSystem(CONST_STRPTR devicename, struct DosLibraryV0 *DOSBaseV0)
{
    return IsFileSystem(devicename);
}
MAKE_PROXY_ARG_2(IsFileSystem)

static struct FileLockProxy *makeFileLockProxy(BPTR native)
{
    struct FileLockProxy *proxy = abiv0_AllocMem(sizeof(struct FileLockProxy), MEMF_ANY, DOS_SysBaseV0);
    proxy->native = native;
    return proxy;
}

BPTR abiv0_Lock(CONST_STRPTR name, LONG accessMode, struct DosLibraryV0 *DOSBaseV0)
{
    BPTR tmp = Lock(name, accessMode);
    if (tmp == BNULL)
        return BNULL;

    return (BPTR)makeFileLockProxy(tmp);
}
MAKE_PROXY_ARG_3(Lock)

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

struct FileInfoBlockProxy
{
    struct FileInfoBlockV0 base;
    struct FileInfoBlock *native;
};

struct ExAllControlProxy
{
    struct ExAllControlV0 base;
    struct ExAllControl *native;
};

APTR abiv0_AllocDosObject(ULONG type, const struct TagItemV0 * tags, struct DosLibraryV0 *DOSBaseV0)
{
    if (type == DOS_FIB && tags == NULL)
    {
        struct FileInfoBlock *fib = AllocDosObject(type, NULL);
        struct FileInfoBlockProxy *proxy = abiv0_AllocMem(sizeof(struct FileInfoBlockProxy), MEMF_CLEAR, DOS_SysBaseV0);
        proxy->native = fib;
        return proxy;
    }
    else if (type == DOS_EXALLCONTROL && tags == NULL)
    {
        struct ExAllControl *eac = AllocDosObject(type, NULL);
        struct ExAllControlProxy *proxy = abiv0_AllocMem(sizeof(struct ExAllControlProxy), MEMF_CLEAR, DOS_SysBaseV0);
        proxy->native = eac;
        return proxy;
    } else if (type == DOS_RDARGS && tags == NULL)
    {
        /* Call original function */
        __asm__ volatile (
            "subq $12, %%rsp\n"
            "movl %3, %%eax\n"
            "movl %%eax, 8(%%rsp)\n"
            "movl %2, %%eax\n"
            "movl %%eax, 4(%%rsp)\n"
            "movl %1, %%eax\n"
            "movl %%eax, (%%rsp)\n"
            "movl %0, %%eax\n"
            ENTER32
            "call *%%eax\n"
            ENTER64
            "addq $12, %%rsp\n"
            "leave\n"
            "ret\n"
            ::"m"(dosfunctable[37]), "m"(type), "m"(tags), "m"(DOSBaseV0) : "%rax", "%rcx");
    }
asm("int3");
}
MAKE_PROXY_ARG_3(AllocDosObject)

void abiv0_FreeDosObject(ULONG type, APTR ptr, struct DosLibraryV0 *DOSBaseV0)
{
    if (type == DOS_FIB)
    {
        struct FileInfoBlockProxy *proxy = (struct FileInfoBlockProxy *)ptr;
        FreeDosObject(type, proxy->native);
        abiv0_FreeMem(proxy, sizeof(struct FileInfoBlockProxy), DOS_SysBaseV0);
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
        __asm__ volatile (
            "subq $12, %%rsp\n"
            "movl %3, %%eax\n"
            "movl %%eax, 8(%%rsp)\n"
            "movl %2, %%eax\n"
            "movl %%eax, 4(%%rsp)\n"
            "movl %1, %%eax\n"
            "movl %%eax, (%%rsp)\n"
            "movl %0, %%eax\n"
            ENTER32
            "call *%%eax\n"
            ENTER64
            "addq $12, %%rsp\n"
            "leave\n"
            "ret\n"
            ::"m"(dosfunctable[38]), "m"(type), "m"(ptr), "m"(DOSBaseV0) : "%rax", "%rcx");
    }
asm("int3");
}
MAKE_PROXY_ARG_3(FreeDosObject)

LONG abiv0_Examine(BPTR lock, struct FileInfoBlockV0 *fib, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileLockProxy *flproxy = (struct FileLockProxy *)lock;
    struct FileInfoBlockProxy *fibproxy = (struct FileInfoBlockProxy *)fib;
    LONG res = Examine(flproxy->native, fibproxy->native);
    if (res)
    {
        fibproxy->base.fib_Date         = fibproxy->native->fib_Date;
        fibproxy->base.fib_Size         = fibproxy->native->fib_Size;
        fibproxy->base.fib_DirEntryType = fibproxy->native->fib_DirEntryType;
        CopyMem(fibproxy->native->fib_FileName, fibproxy->base.fib_FileName, 108);
bug("abiv0_Examine: STUB\n");
    }
    return res;
}
MAKE_PROXY_ARG_3(Examine)

LONG abiv0_ExNext(BPTR lock, struct FileInfoBlockV0 *fileInfoBlock, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileLockProxy *flproxy = (struct FileLockProxy *)lock;
    struct FileInfoBlockProxy *fibproxy = (struct FileInfoBlockProxy *)fileInfoBlock;
    LONG res = ExNext(flproxy->native, fibproxy->native);
    if (res)
    {
        fibproxy->base.fib_Date         = fibproxy->native->fib_Date;
        fibproxy->base.fib_Size         = fibproxy->native->fib_Size;
        fibproxy->base.fib_DirEntryType = fibproxy->native->fib_DirEntryType;
        CopyMem(fibproxy->native->fib_FileName, fibproxy->base.fib_FileName, 108);
bug("abiv0_ExNext: STUB\n");
    }
    return res;
}
MAKE_PROXY_ARG_3(ExNext)

BOOL abiv0_ExamineFH(BPTR fh, struct FileInfoBlockV0 *fib, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileHandleProxy *fhproxy = (struct FileHandleProxy *)fh;
    struct FileInfoBlockProxy *fibproxy = (struct FileInfoBlockProxy *)fib;
    BOOL res = ExamineFH(fhproxy->native, fibproxy->native);
    if (res)
    {
        fibproxy->base.fib_Date = fibproxy->native->fib_Date;
        fibproxy->base.fib_Size = fibproxy->native->fib_Size;
bug("abiv0_ExamineFH: STUB\n");
    }
    return res;
}
MAKE_PROXY_ARG_3(ExamineFH)

LONG abiv0_Flush(BPTR file, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileHandleProxy *fhproxy = (struct FileHandleProxy *)file;
    return Flush(fhproxy->native);

}
MAKE_PROXY_ARG_2(Flush)

BPTR abiv0_CurrentDir(BPTR lock, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileLockProxy *flproxy = (struct FileLockProxy *)lock;
    BPTR old = CurrentDir(flproxy->native);
    return (BPTR)makeFileLockProxy(old);
}
MAKE_PROXY_ARG_2(CurrentDir)

BOOL abiv0_ExAll(BPTR lock, struct ExAllDataV0 *buffer, LONG size, LONG type, struct ExAllControlV0 *control, struct DosLibraryV0 *DOSBaseV0)
{
    struct ExAllControlProxy *eacproxy = (struct ExAllControlProxy *)control;
    struct FileHandleProxy *fhproxy = (struct FileHandleProxy *)lock;

if (type != ED_DATE) asm("int3"); // Copying assumes ED_DATE sizes of structures!!!!

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
            writename = (APTR)writeit + 8; // only up to ED_DATE
            if (eadnativeit->ed_Next == NULL)
                writenext = NULL;
            else
                writenext = writename + namelen;

            writeit->ed_Next    = (APTR32)(IPTR)writenext;
            writeit->ed_Name    = (APTR32)(IPTR)writename;
            writeit->ed_Type    = eadnativeit->ed_Type;
            writeit->ed_Size    = eadnativeit->ed_Size;
            writeit->ed_Prot    = eadnativeit->ed_Prot;
            writeit->ed_Days    = eadnativeit->ed_Days;
            writeit->ed_Mins    = eadnativeit->ed_Mins;
            writeit->ed_Ticks   = eadnativeit->ed_Ticks;
            // ed_Comment, ed_OwnerUID, ed_OwnerGUI missing
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

static struct TagItemV0 *LibNextTagItemV0(struct TagItemV0 **tagListPtr)
{
    if (!(*tagListPtr))
        return NULL;

    while(1)
    {
        switch(((*tagListPtr)->ti_Tag))
        {
            case TAG_MORE:
asm("int3");
                if (!((*tagListPtr) = (struct TagItemV0 *)(IPTR)(*tagListPtr)->ti_Data))
                    return NULL;
                continue;

            case TAG_IGNORE:
                break;

            case TAG_END:
                (*tagListPtr) = 0;
                return NULL;

            case TAG_SKIP:
asm("int3");
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
            case(SYS_UserShell):
                break;
            default:
                bug("%x\n", tagNative->ti_Tag);
                asm("int3");
        }

        tagNative++;
    }

bug("abiv0_SystemTagList: STUB\n");
    return SystemTagList(command, tagListNative);
}
MAKE_PROXY_ARG_3(SystemTagList)

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
extern CONST_STRPTR SYSNAME;

void init_dos(struct ExecBaseV0 *SysBaseV0)
{
    TEXT path[64];
    NewRawDoFmt("%s:Libs32/partial/dos.library", RAWFMTFUNC_STRING, path, SYSNAME);
    /* Keep it! This fills global variable */
    LoadSeg32(path, DOSBase);

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
        ::"m"(SysBaseV0), "m"(seginitlist[1]) : "%rax", "%rcx");

    abiv0DOSBase->dl_UtilityBase = (APTR32)(IPTR)abiv0_DOS_OpenLibrary("utility.library", 0L, SysBaseV0);
    NEWLISTV0(&((struct IntDosBaseV0 *)abiv0DOSBase)->segdata);
    abiv0_InitSemaphore(&((struct IntDosBaseV0 *)abiv0DOSBase)->segsem, SysBaseV0);

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
    __AROS_SETVECADDRV0(abiv0DOSBase,  25, dosfunctable[ 24]);  // LoadSeg              // This is tricky becaused on LoadSeg32
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
}
