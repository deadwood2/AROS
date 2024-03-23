#include <proto/exec.h>
#include <proto/dos.h>
#include <aros/debug.h>


#include "../include/exec/structures.h"
#include "../include/exec/functions.h"
#include "../include/aros/cpu.h"
#include "../include/aros/proxy.h"

#include "../include/dos/structures.h"
#include "../include/utility/structures.h"
#include "../include/timer/structures.h"

extern struct ExecBaseV0 *abiv0SysBase;
extern struct LibraryV0 *abiv0TimerBase;

struct DosLibraryV0 *abiv0DOSBase;

struct FileLockProxy
{
    BPTR native;
};

struct FileHandleProxy
{
    BPTR native;
};

struct FileHandleProxy *makeFileHandleProxy(BPTR native)
{
    struct FileHandleProxy *proxy = abiv0_AllocMem(sizeof(struct FileHandleProxy), MEMF_ANY, abiv0SysBase);
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

    struct FileHandleProxy *fhp = abiv0_AllocMem(sizeof(struct FileHandleProxy), MEMF_ANY, abiv0SysBase);
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

LONG abiv0_Seek(BPTR file, LONG position, LONG mode, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileHandleProxy *fhp = (struct FileHandleProxy *)file;
    return Seek(fhp->native, position, mode);
}
MAKE_PROXY_ARG_4(Seek)

BOOL abiv0_Close(BPTR file, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileHandleProxy *fhp = (struct FileHandleProxy *)file;
    return Close(fhp->native);
}
MAKE_PROXY_ARG_2(Close)

static struct FileLockProxy *makeFileLockProxy(BPTR native)
{
    struct FileLockProxy *proxy = abiv0_AllocMem(sizeof(struct FileLockProxy), MEMF_ANY, abiv0SysBase);
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

    struct DevProcProxy *proxy = abiv0_AllocMem(sizeof(struct DevProcProxy), MEMF_CLEAR, abiv0SysBase);
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
        abiv0_FreeMem(proxy, sizeof(struct DevProcProxy), abiv0SysBase);
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
        struct FileInfoBlockProxy *proxy = abiv0_AllocMem(sizeof(struct FileInfoBlockProxy), MEMF_CLEAR, abiv0SysBase);
        proxy->native = fib;
        return proxy;
    }
    else if (type == DOS_EXALLCONTROL && tags == NULL)
    {
        struct ExAllControl *eac = AllocDosObject(type, NULL);
        struct ExAllControlProxy *proxy = abiv0_AllocMem(sizeof(struct ExAllControlProxy), MEMF_CLEAR, abiv0SysBase);
        proxy->native = eac;
        return proxy;
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
        abiv0_FreeMem(proxy, sizeof(struct FileInfoBlockProxy), abiv0SysBase);
        return;
    } else if (type == DOS_EXALLCONTROL)
    {
        struct ExAllControlProxy *proxy = (struct ExAllControlProxy *)ptr;
        FreeDosObject(type, proxy->native);
        abiv0_FreeMem(proxy, sizeof(struct ExAllControlProxy), abiv0SysBase);
        return;
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
        fibproxy->base.fib_Date = fibproxy->native->fib_Date;
bug("abiv0_Examine: STUB\n");
    }
    return res;
}
MAKE_PROXY_ARG_3(Examine)

BPTR abiv0_CurrentDir(BPTR lock, struct DosLibraryV0 *DOSBaseV0)
{
    struct FileLockProxy *flproxy = (struct FileLockProxy *)lock;
    BPTR old = CurrentDir(flproxy->native);
    return (BPTR)makeFileLockProxy(old);
}
MAKE_PROXY_ARG_2(CurrentDir)

BOOL abiv0_ExAll(BPTR lock, struct ExAllDataV0 *buffer, LONG size, LONG type, struct ExAllControlV0 *control, struct DosLibraryV0 *DOSBaseV0)
{
bug("abiv0_ExAll: STUB\n");
    return FALSE;
}
MAKE_PROXY_ARG_6(ExAll)

SIPTR abiv0_IoErr(struct DosLibraryV0 *DOSBaseV0)
{
bug("abiv0_IoErr: STUB\n");
    return ERROR_NO_MORE_ENTRIES;
}
MAKE_PROXY_ARG_1(IoErr)

BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);
APTR abiv0_DOS_OpenLibrary(CONST_STRPTR name, ULONG version, struct ExecBaseV0 *SysBaseV0);

extern ULONG *dosfunctable;
extern ULONG *seginitlist;

void init_dos()
{
    /* Keep it! This fills global variable */
    LoadSeg32("SYS:Libs32/partial/dos.library", DOSBase);

    APTR tmp = AllocMem(2048, MEMF_31BIT | MEMF_CLEAR);
    abiv0DOSBase = (tmp + 1024);
    abiv0DOSBase->dl_lib.lib_Version = DOSBase->dl_lib.lib_Version;
    abiv0DOSBase->dl_TimeReq = (APTR32)(IPTR)AllocMem(sizeof(struct timerequestV0), MEMF_31BIT | MEMF_CLEAR);
    ((struct timerequestV0 *)(IPTR)abiv0DOSBase->dl_TimeReq)->tr_node.io_Device = (APTR32)(IPTR)abiv0TimerBase;

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
        ::"m"(abiv0SysBase), "m"(seginitlist[1]) : "%rax", "%rcx");

    abiv0DOSBase->dl_UtilityBase = (APTR32)(IPTR)abiv0_DOS_OpenLibrary("utility.library", 0L, abiv0SysBase);

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
}
