#include <dos/bptr.h>
#include <exec/types.h>
#include <aros/debug.h>
#include <proto/timer.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>

#include <stdio.h>
#include <string.h>

#include "abiv0/include/exec/functions.h"
#include "abiv0/include/aros/proxy.h"
#include "abiv0/include/aros/cpu.h"
#include "abiv0/include/dos/structures.h"
#include "abiv0/include/timer/structures.h"
#include "abiv0/include/intuition/structures.h"
#include "abiv0/include/utility/structures.h"
#include "abiv0/include/graphics/proxy_structures.h"

BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);

struct DosLibraryV0 *abiv0DOSBase;
struct LibraryV0 *abiv0TimerBase;
struct ExecBaseV0 *abiv0SysBase;

struct LibraryV0 *shallow_InitResident32(struct ResidentV0 *resident, BPTR segList, struct ExecBaseV0 *SysBaseV0)
{
    struct LibraryV0 *library = NULL;

    D(bug("InitResident begin 0x%p (\"%s\")", resident, resident->rt_Name));

    /* Check for validity */
    if(resident->rt_MatchWord != RTC_MATCHWORD ||
       resident->rt_MatchTag != (APTR32)(IPTR)resident)
        return NULL;

    /* Depending on the autoinit flag... */
    if(resident->rt_Flags & RTF_AUTOINIT)
    {
        /* ...initialize automatically... */
        struct init
        {
            ULONG dSize;
            APTR32 vectors;
            APTR32 structure;
            APTR32 init;
        };
        struct init *init = (struct init *)(IPTR)resident->rt_Init;
        init->init = 0;

        library = abiv0_InitResident(resident, segList, SysBaseV0);
    }
    else
    {
        D(bug("InitResident !RTF_AUTOINIT"));
asm("int3");
    }

    D(bug("InitResident end 0x%p (\"%s\"), result 0x%p", resident, resident->rt_Name, library));

    return library;
} /* shallow_InitResident32 */


APTR abiv0_AllocMem(ULONG byteSize, ULONG requirements, struct ExecBaseV0 *SysBaseV0)
{
    return AllocMem(byteSize, requirements | MEMF_31BIT);
}
MAKE_PROXY_ARG_3(AllocMem)

APTR abiv0_AllocAbs(ULONG byteSize, APTR location, struct ExecBaseV0 *SysBaseV0)
{
    return NULL;
}
MAKE_PROXY_ARG_3(AllocAbs)

void abiv0_FreeMem(APTR memoryBlock, ULONG byteSize, struct ExecBaseV0 *SysBaseV0)
{
    return FreeMem(memoryBlock, byteSize);
}
MAKE_PROXY_ARG_3(FreeMem)

APTR abiv0_AllocVec(ULONG byteSize, ULONG requirements, struct ExecBaseV0 *SysBaseV0)
{
    return AllocVec(byteSize, requirements | MEMF_31BIT);
}
MAKE_PROXY_ARG_3(AllocVec)

void abiv0_FreeVec(APTR memoryBlock, struct ExecBaseV0 *SysBaseV0)
{
    FreeVec(memoryBlock);
}
MAKE_PROXY_ARG_2(FreeVec)

APTR abiv0_CreatePool(ULONG requirements, ULONG puddleSize, ULONG threshSize, struct ExecBaseV0 *SysBaseV0)
{
    return CreatePool(requirements | MEMF_31BIT, puddleSize, threshSize);
}
MAKE_PROXY_ARG_4(CreatePool)

APTR abiv0_AllocPooled(APTR poolHeader, ULONG memSize, struct ExecBaseV0 *SysBaseV0)
{
    return AllocPooled(poolHeader, memSize);
}
MAKE_PROXY_ARG_3(AllocPooled)

void abiv0_FreePooled(APTR poolHeader, APTR memory, ULONG memSize, struct ExecBaseV0 *SysBaseV0)
{
    FreePooled(poolHeader, memory, memSize);
}
MAKE_PROXY_ARG_4(FreePooled)

APTR abiv0_AllocVecPooled(APTR poolHeader, ULONG memSize, struct ExecBaseV0 *SysBaseV0)
{
    return AllocVecPooled(poolHeader, memSize);
}
MAKE_PROXY_ARG_3(AllocVecPooled)

ULONG abiv0_TypeOfMem(APTR address, struct ExecBaseV0 *SysBaseV0)
{
    return TypeOfMem(address);
}
MAKE_PROXY_ARG_2(TypeOfMem)

VOID abiv0_Forbid(struct ExecBaseV0 *SysBaseV0)
{
    Forbid();
}
MAKE_PROXY_ARG_1(Forbid)

VOID abiv0_Permit(struct ExecBaseV0 *SysBaseV0)
{
    Permit();
}
MAKE_PROXY_ARG_1(Permit)

VOID abiv0_AddMemHandler(APTR memHandler, struct ExecBaseV0 *SysBaseV0)
{
    bug("abiv0_AddMemHandler ignored\n");
}
MAKE_PROXY_ARG_2(AddMemHandler)

struct ResidentV0 * findResident(BPTR seg, CONST_STRPTR name)
{
    /* we may not have any extension fields */
    const int sizeofresident = offsetof(struct ResidentV0, rt_Init) + sizeof(APTR);

    while(seg)
    {
        STRPTR addr = (STRPTR)((IPTR)BADDR(seg) - sizeof(ULONG));
        ULONG size = *(ULONG *)addr;

        for(
            addr += sizeof(BPTR) + sizeof(ULONG),
                size -= sizeof(BPTR) + sizeof(ULONG);
            size >= sizeofresident;
            size -= 2, addr += 2
        )
        {
            struct ResidentV0 *res = (struct ResidentV0 *)addr;
            if(    res->rt_MatchWord == RTC_MATCHWORD
                && res->rt_MatchTag == (APTR32)(IPTR)res )
            {
                if ((name != NULL) && (strcmp((char *)(IPTR)res->rt_Name, name) != 0))
                    continue;

                return res;
            }
        }
        seg = *(BPTR *)BADDR(seg);
    }
}

APTR abiv0_DOS_OpenLibrary(CONST_STRPTR name, ULONG version, struct ExecBaseV0 *SysBaseV0)
{
    bug("abiv0_OpenLibrary: %s\n", name);
    TEXT buff[64];
    struct LibraryV0 *_ret;

    /* Special case */
    if (strcmp(name, "dos.library") == 0)
        return abiv0DOSBase;

    /* Call Exec function, maybe the library is already available */
    _ret = abiv0_OpenLibrary(name, version, SysBaseV0);
    if (_ret)
        return _ret;

    /* Try loading from disk */
    sprintf(buff, "SYS:Libs32/%s", name);

    BPTR seglist = LoadSeg32(buff, DOSBase);

    if (seglist == BNULL)
        return NULL;

    struct ResidentV0 *res = findResident(seglist, NULL);

    if (res)
    {
        (bug("[LDInit] Calling InitResident(%p) on %s\n", res, res->rt_Name));
        /* AOS compatibility requirement.
            * Ramlib ignores InitResident() return code.
            * After InitResident() it checks if lib/dev appeared
            * in Exec lib/dev list via FindName().
            *
            * Evidently InitResident()'s return code was not
            * reliable for some early AOS libraries.
            */
        // Forbid();
        abiv0_InitResident(res, seglist, SysBaseV0);
        _ret = abiv0_OpenLibrary(name, version, SysBaseV0);
        // Permit();
        (bug("[LDInit] Done calling InitResident(%p) on %s, seg %p, node %p\n", res, res->rt_Name, BADDR(seglist), _ret));

        return _ret;
    }

    return NULL;
}

void proxy_OpenLibrary();
void dummy_OpenLibrary()
{
    EXTER_PROXY(OpenLibrary)
    ENTER64
    COPY_ARG_1
    COPY_ARG_2
    COPY_ARG_3
    CALL_IMPL64(DOS_OpenLibrary)
    ENTER32
    LEAVE_PROXY
}

void abiv0_PutStr(CONST_STRPTR text)
{
    PutStr(text);
}
MAKE_PROXY_ARG_2(PutStr)

void abiv0_CloseLibrary()
{
}
MAKE_PROXY_ARG_2(CloseLibrary)

ULONG abiv0_AllocTaskStorageSlot()
{
    return AllocTaskStorageSlot();
}
MAKE_PROXY_ARG_2(AllocTaskStorageSlot)

BOOL abiv0_SetTaskStorageSlot(LONG slot, ULONG value)
{
    return SetTaskStorageSlot(slot, value);
}
MAKE_PROXY_ARG_3(SetTaskStorageSlot)

ULONG abiv0_GetTaskStorageSlot(LONG id)
{
    return GetTaskStorageSlot(id);
}
MAKE_PROXY_ARG_2(GetTaskStorageSlot)

APTR abiv0_OpenResource(CONST_STRPTR resName)
{
    if (strcmp(resName, "kernel.resource") == 0)
        return NULL;

asm("int3");
    return (APTR)0x1;
}
MAKE_PROXY_ARG_2(OpenResource)

LONG abiv0_OpenDevice(CONST_STRPTR devName, ULONG unitNumber, struct IORequestV0 *iORequest)
{
    if (strcmp(devName, "timer.device") == 0)
    {
        iORequest->io_Device = (APTR32)(IPTR)abiv0TimerBase;
        return 0;
    }

    if (strcmp(devName, "input.device") == 0)
    {
bug("abiv0_OpenDevice: input.device STUB\n");
        iORequest->io_Device = 0;
        return 0;
    }


asm("int3");
    return 0;
}
MAKE_PROXY_ARG_4(OpenDevice)

struct MsgPortV0 * abiv0_CreateMsgPort(struct ExecBaseV0 *SysBaseV0)
{
bug("abiv0_CreateMsgPort: STUB\n");
    return (struct MsgPortV0 *)0x6;
}
MAKE_PROXY_ARG_1(CreateMsgPort)

LONG abiv0_DoIO(struct IORequestV0 *IORequest, struct ExecBaseV0 *SysBaseV0)
{
bug("abiv0_DoIO: STUB\n");
    return 0;
}
MAKE_PROXY_ARG_2(DoIO)


MAKE_PROXY_ARG_6(MakeLibrary)
MAKE_PROXY_ARG_2(AddResource)

void abiv0_CopyMem(APTR source, APTR dest, ULONG size)
{
    return CopyMem(source, dest, size);
}
MAKE_PROXY_ARG_4(CopyMem)

struct LibraryV0 *abiv0_Intuition_OpenLib(ULONG version, struct LibraryV0 *IntuitionBaseV0)
{
    return IntuitionBaseV0;
}
MAKE_PROXY_ARG_2(Intuition_OpenLib)

struct LibraryV0 *abiv0_Layers_OpenLib(ULONG version, struct LibraryV0 *LayersBaseV0)
{
    return LayersBaseV0;
}
MAKE_PROXY_ARG_2(Layers_OpenLib)

struct FileHandleProxy
{
    BPTR native;
};

static struct FileHandleProxy *makeFileHandleProxy(BPTR native)
{
    struct FileHandleProxy *proxy = abiv0_AllocMem(sizeof(struct FileHandleProxy), MEMF_ANY, abiv0SysBase);
    proxy->native = native;
    return proxy;
}

struct TaskV0 *abiv0_FindTask(CONST_STRPTR name, struct ExecBaseV0 *SysBaseV0)
{
    static struct ProcessV0 *dummy = NULL;
    if (dummy == NULL) dummy = abiv0_AllocMem(sizeof(struct ProcessV0), MEMF_CLEAR, SysBaseV0);
    dummy->pr_Task.tc_Node.ln_Type = NT_PROCESS;
    dummy->pr_Task.tc_Node.ln_Name = (APTR32)(IPTR)abiv0_AllocMem(10, MEMF_CLEAR, SysBaseV0);
    strcpy((char *)(IPTR)dummy->pr_Task.tc_Node.ln_Name, "emulator");
    dummy->pr_CLI = (BPTR32)(IPTR)abiv0_AllocMem(sizeof(struct CommandLineInterfaceV0), MEMF_CLEAR, SysBaseV0);
    dummy->pr_CIS = (BPTR32)(IPTR)makeFileHandleProxy(Input());
    dummy->pr_CES = 0x2; //fake
    dummy->pr_COS = (BPTR32)(IPTR)makeFileHandleProxy(Output());
    struct FileHandleProxy *v0homedir = abiv0_AllocMem(sizeof(struct FileHandleProxy), MEMF_ANY, SysBaseV0);
    v0homedir->native = GetProgramDir();
    dummy->pr_HomeDir = (BPTR32)(IPTR)v0homedir;


    return (struct TaskV0 *)dummy;
}
MAKE_PROXY_ARG_2(FindTask)

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

void abiv0_GetSysTime(struct timeval *dest, struct LibraryV0 *TimerBaseV0)
{
    return GetSysTime(dest);
}
MAKE_PROXY_ARG_2(GetSysTime)

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

struct FileLockProxy
{
    BPTR native;
};

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

struct ScreenProxy
{
    struct ScreenV0 base;
    struct Screen   *native;
};



struct ScreenV0 *abiv0_LockPubScreen(CONST_STRPTR name, struct LibraryV0 *IntuitionBaseV0)
{
    struct Screen *scr = LockPubScreen(name);
    if (scr == NULL)
        return NULL;

    struct ScreenProxy *ret = abiv0_AllocMem(sizeof(struct ScreenProxy), MEMF_CLEAR, abiv0SysBase);
    ret->base.Width     = scr->Width;
    ret->base.Height    = scr->Height;
    ret->native         = scr;

    struct ColorMapProxy *cmproxy = abiv0_AllocMem(sizeof(struct ColorMapProxy), MEMF_CLEAR, abiv0SysBase);
    cmproxy->native = scr->ViewPort.ColorMap;
    ret->base.ViewPort.ColorMap = (APTR32)(IPTR)cmproxy;

    struct TextAttrV0 * v0font = abiv0_AllocMem(sizeof(struct TextAttrV0), MEMF_CLEAR, abiv0SysBase);
    v0font->ta_YSize    = scr->Font->ta_YSize;
    v0font->ta_Flags    = scr->Font->ta_Flags;
    v0font->ta_Style    = scr->Font->ta_Style;
    STRPTR v0font_name = abiv0_AllocMem(strlen(scr->Font->ta_Name) + 1, MEMF_CLEAR, abiv0SysBase);
    CopyMem(scr->Font->ta_Name, v0font_name, strlen(scr->Font->ta_Name) + 1);
    v0font->ta_Name     = (APTR32)(IPTR)v0font_name;
    ret->base.Font = (APTR32)(IPTR)v0font;

bug("abiv0_LockPubScreen: STUB\n");
    return (struct ScreenV0 *)ret;
}
MAKE_PROXY_ARG_2(LockPubScreen)

void abiv0_ScreenDepth(struct ScreenV0 *screen, ULONG flags, APTR reserved, struct LibraryV0 *IntuitionBaseV0)
{
bug("abiv0_ScreenDepth: STUB\n");
}
MAKE_PROXY_ARG_4(ScreenDepth)

struct DrawInfoV0 *abiv0_GetScreenDrawInfo(struct ScreenV0 *screen, struct LibraryV0 *IntuitionBaseV0)
{
    struct ScreenProxy *proxy = (struct ScreenProxy *)screen;
    struct DrawInfo *dri = GetScreenDrawInfo(proxy->native);
    if (dri == NULL)
        return NULL;

    struct DrawInfoV0 *ret = abiv0_AllocMem(sizeof(struct DrawInfoV0), MEMF_CLEAR, abiv0SysBase);
    ret->dri_Pens = (APTR32)(IPTR)abiv0_AllocMem(NUMDRIPENS * sizeof(UWORD), MEMF_CLEAR, abiv0SysBase);
    CopyMem(dri->dri_Pens, (APTR)(IPTR)ret->dri_Pens, NUMDRIPENS * sizeof(UWORD));

bug("abiv0_GetScreenDrawInfo: STUB\n");
    return ret;
}
MAKE_PROXY_ARG_2(GetScreenDrawInfo)

ULONG *execfunctable;
ULONG *dosfunctable;

ULONG *seginitlist;
ULONG *segclassesinitlist;

APTR32 global_SysBaseV0Ptr;

void init_graphics();

LONG_FUNC run_emulation()
{
    BPTR seg = LoadSeg32("SYS:Calculator", DOSBase);
    // BPTR seg = LoadSeg32("SYS:helloabi", DOSBase);
    APTR (*start)() = (APTR)((IPTR)BADDR(seg) + sizeof(BPTR));

    /* Set start at first instruction (skip Seg header) */
    start = (APTR)((IPTR)start + 13);

    APTR tmp;

    tmp = AllocMem(2048, MEMF_31BIT | MEMF_CLEAR);
    abiv0SysBase = (tmp + 1024);
    global_SysBaseV0Ptr = (APTR32)(IPTR)&abiv0SysBase; /* Needed for LoadSeg32 to resolve SysBase in kernel */

    /* Keep it! This fills global variable */
    LoadSeg32("SYS:Libs32/partial/kernel", DOSBase);

    NEWLISTV0(&abiv0SysBase->LibList);
    NEWLISTV0(&abiv0SysBase->ResourceList);
    abiv0SysBase->LibNode.lib_Version = 51;

    __AROS_SETVECADDRV0(abiv0SysBase, 92, (APTR32)(IPTR)proxy_OpenLibrary);
    __AROS_SETVECADDRV0(abiv0SysBase, 69, (APTR32)(IPTR)proxy_CloseLibrary);
    __AROS_SETVECADDRV0(abiv0SysBase, 49, (APTR32)(IPTR)proxy_FindTask);
    __AROS_SETVECADDRV0(abiv0SysBase,180, (APTR32)(IPTR)proxy_AllocTaskStorageSlot);
    __AROS_SETVECADDRV0(abiv0SysBase,184, (APTR32)(IPTR)proxy_SetTaskStorageSlot);
    __AROS_SETVECADDRV0(abiv0SysBase,185, (APTR32)(IPTR)proxy_GetTaskStorageSlot);
    __AROS_SETVECADDRV0(abiv0SysBase, 83, (APTR32)(IPTR)proxy_OpenResource);
    __AROS_SETVECADDRV0(abiv0SysBase, 93, execfunctable[92]);    // InitSemaphore
    __AROS_SETVECADDRV0(abiv0SysBase, 33, (APTR32)(IPTR)proxy_AllocMem);
    __AROS_SETVECADDRV0(abiv0SysBase, 14, (APTR32)(IPTR)proxy_MakeLibrary);
    __AROS_SETVECADDRV0(abiv0SysBase,104, (APTR32)(IPTR)proxy_CopyMem);
    __AROS_SETVECADDRV0(abiv0SysBase,116, (APTR32)(IPTR)proxy_CreatePool);
    __AROS_SETVECADDRV0(abiv0SysBase, 74, (APTR32)(IPTR)proxy_OpenDevice);
    __AROS_SETVECADDRV0(abiv0SysBase,118, (APTR32)(IPTR)proxy_AllocPooled);
    __AROS_SETVECADDRV0(abiv0SysBase,114, (APTR32)(IPTR)proxy_AllocVec);
    __AROS_SETVECADDRV0(abiv0SysBase, 46, execfunctable[45]);    // FindName
    __AROS_SETVECADDRV0(abiv0SysBase,135, execfunctable[134]);   // TaggedOpenLibrary
    __AROS_SETVECADDRV0(abiv0SysBase, 89, (APTR32)(IPTR)proxy_TypeOfMem);
    __AROS_SETVECADDRV0(abiv0SysBase, 41, execfunctable[40]);    // AddTail
    __AROS_SETVECADDRV0(abiv0SysBase, 87, execfunctable[86]);    // RawDoFmt
    __AROS_SETVECADDRV0(abiv0SysBase, 35, (APTR32)(IPTR)proxy_FreeMem);
    __AROS_SETVECADDRV0(abiv0SysBase,113, execfunctable[112]);   // ObtainSemaphoreShared
    __AROS_SETVECADDRV0(abiv0SysBase, 94, execfunctable[93]);    // ObtainSemaphore
    __AROS_SETVECADDRV0(abiv0SysBase, 40, execfunctable[39]);    // AddHead
    __AROS_SETVECADDRV0(abiv0SysBase, 95, execfunctable[94]);    // ReleaseSemaphore
    __AROS_SETVECADDRV0(abiv0SysBase, 81, (APTR32)(IPTR)proxy_AddResource);
    __AROS_SETVECADDRV0(abiv0SysBase, 22, (APTR32)(IPTR)proxy_Forbid);
    __AROS_SETVECADDRV0(abiv0SysBase, 23, (APTR32)(IPTR)proxy_Permit);
    __AROS_SETVECADDRV0(abiv0SysBase,129, (APTR32)(IPTR)proxy_AddMemHandler);
    __AROS_SETVECADDRV0(abiv0SysBase, 70, execfunctable[69]);    // SetFunction
    __AROS_SETVECADDRV0(abiv0SysBase, 71, execfunctable[70]);    // SumLibrary
    __AROS_SETVECADDRV0(abiv0SysBase, 45, execfunctable[44]);    // Enqueue
    __AROS_SETVECADDRV0(abiv0SysBase, 34, (APTR32)(IPTR)proxy_AllocAbs);
    __AROS_SETVECADDRV0(abiv0SysBase,115, (APTR32)(IPTR)proxy_FreeVec);
    __AROS_SETVECADDRV0(abiv0SysBase,111, (APTR32)(IPTR)proxy_CreateMsgPort);
    __AROS_SETVECADDRV0(abiv0SysBase,109, execfunctable[108]);  // CreateIORequest
    __AROS_SETVECADDRV0(abiv0SysBase, 42, execfunctable[41]);   // Remove
    __AROS_SETVECADDRV0(abiv0SysBase,149, (APTR32)(IPTR)proxy_AllocVecPooled);
    __AROS_SETVECADDRV0(abiv0SysBase, 76, (APTR32)(IPTR)proxy_DoIO);
    __AROS_SETVECADDRV0(abiv0SysBase,119, (APTR32)(IPTR)proxy_FreePooled);

    tmp = AllocMem(1024, MEMF_31BIT | MEMF_CLEAR);
    abiv0TimerBase = (tmp + 512);
    __AROS_SETVECADDRV0(abiv0TimerBase, 11, (APTR32)(IPTR)proxy_GetSysTime);


    /* Keep it! This fills global variable */
    LoadSeg32("SYS:Libs32/partial/dos.library", DOSBase);

    tmp = AllocMem(2048, MEMF_31BIT | MEMF_CLEAR);
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

    BPTR cgfxseg = LoadSeg32("SYS:Libs32/partial/cybergraphics.library", DOSBase);
    struct ResidentV0 *cgfxres = findResident(cgfxseg, NULL);
    struct LibraryV0 *abiv0CyberGfxBase = shallow_InitResident32(cgfxres, cgfxseg, abiv0SysBase);
    /* Remove all vectors for now (leave LibOpen) */
    for (int i = 5; i <= 38; i++) __AROS_SETVECADDRV0(abiv0CyberGfxBase, i, 0);

    BPTR intuitionseg = LoadSeg32("SYS:Libs32/partial/intuition.library", DOSBase);
    struct ResidentV0 *intuitionres = findResident(intuitionseg, NULL);
    struct LibraryV0 *abiv0IntuitionBase = shallow_InitResident32(intuitionres, intuitionseg, abiv0SysBase);
    /* Remove all vectors for now */
    const ULONG intuitionjmpsize = 165 * sizeof(APTR32);
    APTR32 *intuitionjmp = AllocMem(intuitionjmpsize, MEMF_CLEAR);
    CopyMem((APTR)abiv0IntuitionBase - intuitionjmpsize, intuitionjmp, intuitionjmpsize);
    for (int i = 1; i <= 164; i++) __AROS_SETVECADDRV0(abiv0IntuitionBase, i, 0);

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

    __AROS_SETVECADDRV0(abiv0IntuitionBase,   1, (APTR32)(IPTR)proxy_Intuition_OpenLib);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 113, intuitionjmp[165 - 113]);  // MakeClass
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 112, intuitionjmp[165 - 112]);  // FindClass
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 114, intuitionjmp[165 - 114]);  // AddClass
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 106, intuitionjmp[165 - 106]);  // NewObjectA
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 108, intuitionjmp[165 - 108]);  // SetAttrs
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 109, intuitionjmp[165 - 109]);  // GetAttr
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 111, intuitionjmp[165 - 111]);  // NextObject
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  85, (APTR32)(IPTR)proxy_LockPubScreen);
    __AROS_SETVECADDRV0(abiv0IntuitionBase,  42, intuitionjmp[165 -  42]);  // ScreenToFront
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 131, (APTR32)(IPTR)proxy_ScreenDepth);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 115, (APTR32)(IPTR)proxy_GetScreenDrawInfo);
    __AROS_SETVECADDRV0(abiv0IntuitionBase, 107, intuitionjmp[165 - 107]);  // DisposeObject

    /* Call CLASSESINIT_LIST */
    ULONG pos = 1;
    APTR32 func = segclassesinitlist[pos];
    while (func != 0)
    {
        __asm__ volatile (
            "subq $4, %%rsp\n"
            "movl %0, %%eax\n"
            "movl %%eax, (%%rsp)\n"
            "movl %1, %%eax\n"
            ENTER32
            "call *%%eax\n"
            ENTER64
            "addq $4, %%rsp\n"
            ::"m"(abiv0IntuitionBase), "m"(func) : "%rax", "%rcx");
        pos++;
        func = segclassesinitlist[pos];
    }

    /* Set internal Intuition pointer of utility */
    *(ULONG *)((IPTR)abiv0IntuitionBase + 0x60) = (APTR32)(IPTR)abiv0_DOS_OpenLibrary("utility.library", 0L, abiv0SysBase);

    init_graphics();

    BPTR layersseg = LoadSeg32("SYS:Libs32/partial/layers.library", DOSBase);
    struct ResidentV0 *layersres = findResident(layersseg, NULL);
    struct LibraryV0 *abiv0LayersBase = shallow_InitResident32(layersres, layersseg, abiv0SysBase);
    /* Remove all vectors for now */
    for (int i = 1; i <= 45; i++) __AROS_SETVECADDRV0(abiv0LayersBase, i, 0);
    __AROS_SETVECADDRV0(abiv0LayersBase,   1, (APTR32)(IPTR)proxy_Layers_OpenLib);

    /*  Switch to CS = 0x23 during FAR call. This switches 32-bit emulation mode.
        Next, load 0x2B to DS (needed under 32-bit) and NEAR jump to 32-bit code */
    __asm__ volatile(
    "   movl %0, %%ecx\n"
    "   movl %1, %%edx\n"
    "   subq $8, %%rsp\n"
    "   movl $0x23, 4(%%rsp)\n" // Jump to 32-bit mode
    "   lea  tramp, %%rax\n"
    "   movl %%eax, (%%rsp)\n"
    "   lret\n"
    "tramp:\n"
    "   .code32\n"
    "   push $0x2b\n"
    "   pop %%ds\n"
    "   mov $0x0, %%eax\n"
    "   push %%edx\n" //SysBase
    "   push %%eax\n" //argsize
    "   push %%eax\n" //argstr
    "   call *%%ecx\n"
    "   pop %%eax\n" // Clean up stack
    "   pop %%eax\n"
    "   pop %%eax\n"
    "   push $0x33\n" // Jump back to 64-bit mode
    "   lea finished, %%eax\n"
    "   push %%eax\n"
    "   lret\n"
    "   .code64\n"
    "finished:"
        :: "m"(start), "m" (abiv0SysBase) :);
}

struct timerequest tr;
struct Device *TimerBase;

int main()
{
    OpenDevice("timer.device", UNIT_VBLANK, &tr.tr_node, 0);
    TimerBase = tr.tr_node.io_Device;

    /* Run emulation code with stack allocated in 31-bit memory */
    APTR stack31bit = AllocMem(64 * 1024, MEMF_CLEAR | MEMF_31BIT);
    struct StackSwapStruct sss;
    sss.stk_Lower = stack31bit;
    sss.stk_Upper = sss.stk_Lower + 64 * 1024;
    sss.stk_Pointer = sss.stk_Upper;

    NewStackSwap(&sss, run_emulation, NULL);

    return 0;
}

