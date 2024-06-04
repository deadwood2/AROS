#include <dos/bptr.h>
#include <exec/types.h>
#include <aros/debug.h>
#include <proto/timer.h>
#include <exec/rawfmt.h>
#include <proto/dos.h>

#include "abiv0/include/exec/functions.h"
#include "abiv0/include/exec/proxy_structures.h"
#include "abiv0/include/aros/proxy.h"
#include "abiv0/include/aros/cpu.h"

struct DeviceProxy *abiv0TimerBase;

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

struct LibraryV0 *abiv0_Layers_OpenLib(ULONG version, struct LibraryV0 *LayersBaseV0)
{
    return LayersBaseV0;
}
MAKE_PROXY_ARG_2(Layers_OpenLib)

#include <proto/layers.h>
#include <graphics/regions.h>

#include "abiv0/include/graphics/structures.h"
#include "abiv0/include/graphics/proxy_structures.h"

struct ExecBaseV0 *Layers_SysBaseV0;

struct RegionV0 *abiv0_InstallClipRegion(struct LayerV0  *l, struct RegionV0 *region, struct LibraryV0 *LayersBaseV0)
{
    struct LayerProxy *lproxy = (struct LayerProxy *)l;
    struct RegionProxy *regionproxy = (struct RegionProxy *)region;
    if (regionproxy)
        InstallClipRegion(lproxy->native, regionproxy->native);
    else
        InstallClipRegion(lproxy->native, NULL);

    return NULL; /* FIXME */
}
MAKE_PROXY_ARG_3(InstallClipRegion)

void abiv0_LockLayerInfo(struct Layer_InfoV0 *li, struct LibraryV0 *LayersBaseV0)
{
    struct Layer_Info *linative = (struct Layer_Info *)*(IPTR *)(&li->PrivateReserve1);
    LockLayerInfo(linative);
}
MAKE_PROXY_ARG_2(LockLayerInfo)

void abiv0_UnlockLayerInfo(struct Layer_InfoV0 *li, struct LibraryV0 *LayersBaseV0)
{
    struct Layer_Info *linative = (struct Layer_Info *)*(IPTR *)(&li->PrivateReserve1);
    UnlockLayerInfo(linative);
}
MAKE_PROXY_ARG_2(UnlockLayerInfo)

void abiv0_LockLayer(LONG dummy, struct LayerV0 *layer, struct LibraryV0 *LayersBaseV0)
{
    struct LayerProxy *proxy = (struct LayerProxy *)layer;
    LockLayer(dummy, proxy->native);

    if (proxy->native->DamageList)
    {
        struct RegionProxy *rproxy = abiv0_AllocMem(sizeof(struct RegionProxy), MEMF_CLEAR, Layers_SysBaseV0);
        rproxy->native  = proxy->native->DamageList;

        rproxy->base.bounds.MaxX    = rproxy->native->bounds.MaxX;
        rproxy->base.bounds.MinX    = rproxy->native->bounds.MinX;
        rproxy->base.bounds.MaxY    = rproxy->native->bounds.MaxY;
        rproxy->base.bounds.MinY    = rproxy->native->bounds.MinY;

        struct RegionRectangle *rrnative = rproxy->native->RegionRectangle;
        struct RegionRectangleV0 *rrv0prev = NULL, *rrv0first = NULL;
        while(rrnative)
        {
            struct RegionRectangleV0 *rrv0 = abiv0_AllocMem(sizeof(struct RegionRectangleV0), MEMF_CLEAR, Layers_SysBaseV0);
            rrv0->bounds.MaxX = rrnative->bounds.MaxX;
            rrv0->bounds.MinX = rrnative->bounds.MinX;
            rrv0->bounds.MaxY = rrnative->bounds.MaxY;
            rrv0->bounds.MinY = rrnative->bounds.MinY;
            if (rrv0prev)
            {
                rrv0prev->Next = (APTR32)(IPTR)rrv0;
                rrv0->Prev = (APTR32)(IPTR)rrv0prev;
                rrv0prev = rrv0;
            }
            if (!rrv0prev)
            {
                rrv0first = rrv0prev = rrv0;
            }

            rrnative = rrnative->Next;
        }

        rproxy->base.RegionRectangle = (APTR32)(IPTR)rrv0first;
        proxy->base.DamageList  = (APTR32)(IPTR)rproxy;
    }
    else
    {
        if (proxy->base.DamageList) bug("abiv0_LockLayer: MEMORY LEAK\n");
        proxy->base.DamageList = (APTR32)(IPTR)NULL;
    }
}
MAKE_PROXY_ARG_3(LockLayer)

void abiv0_UnlockLayer(struct LayerV0 *layer, struct LibraryV0 *LayersBaseV0)
{
    struct LayerProxy *proxy = (struct LayerProxy *)layer;
    UnlockLayer(proxy->native);
}
MAKE_PROXY_ARG_2(UnlockLayer)

void abiv0_EndUpdate(struct LayerV0 *l, UWORD flag, struct LibraryV0 *LayersBaseV0)
{
    struct LayerProxy *proxy = (struct LayerProxy *)l;
    EndUpdate(proxy->native, flag);
}
MAKE_PROXY_ARG_3(EndUpdate)

LONG abiv0_BeginUpdate(struct LayerV0 *l, struct LibraryV0 *LayersBaseV0)
{
    struct LayerProxy *proxy = (struct LayerProxy *)l;
    BeginUpdate(proxy->native);
}
MAKE_PROXY_ARG_3(BeginUpdate)

void abiv0_GetSysTime(struct timeval *dest, struct LibraryV0 *TimerBaseV0)
{
    GetSysTime(dest);
}
MAKE_PROXY_ARG_2(GetSysTime)

void abiv0_SubTime(struct timeval *dest, struct timeval *src, struct LibraryV0 *TimerBaseV0)
{
    SubTime(dest, src);
}
MAKE_PROXY_ARG_3(SubTime)

#include <proto/cybergraphics.h>

ULONG abiv0_FillPixelArray(struct RastPortV0 *rp, UWORD destx, UWORD desty, UWORD width, UWORD height, ULONG pixel)
{
    struct RastPort *rpnative = (struct RastPort *)*(IPTR *)&rp->longreserved;
    return FillPixelArray(rpnative, destx, desty, width, height, pixel);
}
MAKE_PROXY_ARG_6(FillPixelArray)

ULONG abiv0_WritePixelArrayAlpha(APTR src, UWORD srcx, UWORD srcy, UWORD srcmod, struct RastPortV0 *rp,
    UWORD destx, UWORD desty, UWORD width, UWORD height, ULONG globalalpha, struct LibraryV0 *CyberGfxBaseV0)
{
    struct RastPort *rpnative = (struct RastPort *)*(IPTR *)&rp->longreserved;
    return WritePixelArrayAlpha(src, srcx, srcy, srcmod, rpnative, destx, desty, width, height, globalalpha);
}
MAKE_PROXY_ARG_12(WritePixelArrayAlpha)

LONG abiv0_WriteLUTPixelArray(APTR srcRect, UWORD SrcX, UWORD SrcY, UWORD SrcMod, struct RastPortV0 *rp,
    APTR CTable, UWORD DestX, UWORD DestY, UWORD SizeX, UWORD SizeY, UBYTE CTabFormat, struct LibraryV0 *CyberGfxBaseV0)
{
    struct RastPort *rpnative = (struct RastPort *)*(IPTR *)&rp->longreserved;
    return WriteLUTPixelArray(srcRect, SrcX, SrcY, SrcMod, rpnative, CTable, DestX, DestY, SizeX, SizeY, CTabFormat);
}
MAKE_PROXY_ARG_12(WriteLUTPixelArray)

#include <proto/graphics.h>

ULONG abiv0_WritePixelArray(APTR src, UWORD srcx, UWORD srcy, UWORD srcmod, struct RastPortV0 *rp,
    UWORD destx, UWORD desty, UWORD width, UWORD height, UBYTE srcformat, struct LibraryV0 *CyberGfxBaseV0)
{
    struct RastPort *rpnative = (struct RastPort *)*(IPTR *)&rp->longreserved;
    struct RastPort rptmp;

    /* picture.datatype uses locally created RastPort */
    if (rpnative == NULL)
    {
        InitRastPort(&rptmp);
        rptmp.FgPen     = rp->FgPen;
        rptmp.BgPen     = rp->BgPen;
        rptmp.DrawMode  = rp->DrawMode;
        rptmp.linpatcnt = rp->linpatcnt;
        rptmp.Flags     = rp->Flags;
        rptmp.cp_x      = rp->cp_x;
        rptmp.cp_y      = rp->cp_y;

        rptmp.BitMap = ((struct BitMapProxy *)(IPTR)rp->BitMap)->native;

        rpnative = &rptmp;
    }

    return WritePixelArray(src, srcx, srcy, srcmod, rpnative, destx, desty, width, height, srcformat);
}
MAKE_PROXY_ARG_12(WritePixelArray)

#include "abiv0/include/utility/structures.h"

VOID abiv0_ProcessPixelArray(struct RastPortV0 *rp, ULONG destX, ULONG destY, ULONG sizeX, ULONG sizeY, ULONG operation,
        LONG value, struct TagItemV0 *taglist, struct LibraryV0 *CyberGfxBaseV0)
{
    struct RastPort *rpnative = (struct RastPort *)*(IPTR *)&rp->longreserved;
    struct RastPort rptmp;

/* dtpic.mui uses locally created RastPort */
    if (rpnative == NULL)
    {
        InitRastPort(&rptmp);
        rptmp.FgPen     = rp->FgPen;
        rptmp.BgPen     = rp->BgPen;
        rptmp.DrawMode  = rp->DrawMode;
        rptmp.linpatcnt = rp->linpatcnt;
        rptmp.Flags     = rp->Flags;
        rptmp.cp_x      = rp->cp_x;
        rptmp.cp_y      = rp->cp_y;

        rptmp.BitMap = ((struct BitMapProxy *)(IPTR)rp->BitMap)->native;

        rpnative = &rptmp;
    }

    ProcessPixelArray(rpnative, destX, destY, sizeX, sizeY, operation, value, NULL);
}
MAKE_PROXY_ARG_12(ProcessPixelArray)

BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);
struct ResidentV0 * findResident(BPTR seg, CONST_STRPTR name);

void init_graphics(struct ExecBaseV0 *);
void init_intuition(struct ExecBaseV0 *, struct DeviceProxy *);
void init_dos(struct ExecBaseV0 *);
struct ExecBaseV0 *init_exec();

void execute_in_32_bit(APTR start, struct ExecBaseV0 *SysBaseV0)
{
    __asm__ volatile(
    "   subq $16, %%rsp\n"
    "   movl $0, %%eax\n"
    "   movl %%eax, 12(%%rsp)\n" //pad
    "   movl %1, %%eax\n"
    "   movl %%eax, 8(%%rsp)\n" //SysBase
    "   movl $0, %%eax\n"
    "   movl %%eax, 4(%%rsp)\n" //argsize
    "   movl $0, %%eax\n"
    "   movl %%eax, (%%rsp)\n" //argstr
    "   movl %0, %%edx\n"  // start
    ENTER32
    "   call *%%edx\n"
    ENTER64
    "   addq $16, %%rsp"// Clean up stack
        :: "m"(start), "m" (SysBaseV0)
        : SCRATCH_REGS_64_TO_32 );
}

LONG_FUNC run_emulation(CONST_STRPTR program_path)
{
    TEXT path[64];

    /* Init ROM */
    struct ExecBaseV0 *SysBaseV0 = init_exec();

    APTR tmp = AllocMem(1024, MEMF_31BIT | MEMF_CLEAR);
    abiv0TimerBase = (tmp + 512);
    __AROS_SETVECADDRV0(abiv0TimerBase, 11, (APTR32)(IPTR)proxy_GetSysTime);
    __AROS_SETVECADDRV0(abiv0TimerBase,  8, (APTR32)(IPTR)proxy_SubTime);
    abiv0TimerBase->type = DEVPROXY_TYPE_TIMER;

    init_dos(SysBaseV0);

    init_graphics(SysBaseV0);

    NewRawDoFmt("LIBSV0:partial/layers.library", RAWFMTFUNC_STRING, path);
    BPTR layersseg = LoadSeg32(path, DOSBase);
    struct ResidentV0 *layersres = findResident(layersseg, NULL);
    struct LibraryV0 *abiv0LayersBase = shallow_InitResident32(layersres, layersseg, SysBaseV0);
    Layers_SysBaseV0 = SysBaseV0;
    /* Remove all vectors for now */
    for (int i = 1; i <= 45; i++) __AROS_SETVECADDRV0(abiv0LayersBase, i, 0);
    __AROS_SETVECADDRV0(abiv0LayersBase,   1, (APTR32)(IPTR)proxy_Layers_OpenLib);
    __AROS_SETVECADDRV0(abiv0LayersBase,  29, (APTR32)(IPTR)proxy_InstallClipRegion);
    __AROS_SETVECADDRV0(abiv0LayersBase,  20, (APTR32)(IPTR)proxy_LockLayerInfo);
    __AROS_SETVECADDRV0(abiv0LayersBase,  23, (APTR32)(IPTR)proxy_UnlockLayerInfo);
    __AROS_SETVECADDRV0(abiv0LayersBase,  16, (APTR32)(IPTR)proxy_LockLayer);
    __AROS_SETVECADDRV0(abiv0LayersBase,  17, (APTR32)(IPTR)proxy_UnlockLayer);
    __AROS_SETVECADDRV0(abiv0LayersBase,  14, (APTR32)(IPTR)proxy_EndUpdate);
    __AROS_SETVECADDRV0(abiv0LayersBase,  13, (APTR32)(IPTR)proxy_BeginUpdate);

    NewRawDoFmt("LIBSV0:partial/cybergraphics.library", RAWFMTFUNC_STRING, path);
    BPTR cgfxseg = LoadSeg32(path, DOSBase);
    struct ResidentV0 *cgfxres = findResident(cgfxseg, NULL);
    struct LibraryV0 *abiv0CyberGfxBase = shallow_InitResident32(cgfxres, cgfxseg, SysBaseV0);
    /* Remove all vectors for now (leave LibOpen) */
    for (int i = 5; i <= 38; i++) __AROS_SETVECADDRV0(abiv0CyberGfxBase, i, 0);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase, 25, (APTR32)(IPTR)proxy_FillPixelArray);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase, 36, (APTR32)(IPTR)proxy_WritePixelArrayAlpha);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase, 33, (APTR32)(IPTR)proxy_WriteLUTPixelArray);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase, 21, (APTR32)(IPTR)proxy_WritePixelArray);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase, 38, (APTR32)(IPTR)proxy_ProcessPixelArray);

    init_intuition(SysBaseV0, abiv0TimerBase);

    /* Install datatypes */
    NewRawDoFmt("SYSV0:C/AddDataTypes", RAWFMTFUNC_STRING, path);
    BPTR adtseg = LoadSeg32(path, DOSBase);
    APTR (*adtstart)() = (APTR)((IPTR)BADDR(adtseg) + sizeof(BPTR));
    /* Inject arguments for AddDataTypes*/
    struct FileHandle *fhinput = BADDR(Input());
    CopyMem("REFRESH\n", BADDR(fhinput->fh_Buf), 9);
    fhinput->fh_Pos = 0;
    fhinput->fh_End = 9;

    execute_in_32_bit(adtstart, SysBaseV0);

    /* Start Program */
    NewRawDoFmt("%s", RAWFMTFUNC_STRING, path, program_path);
    BPTR seg = LoadSeg32(path, DOSBase);
    APTR (*start)() = (APTR)((IPTR)BADDR(seg) + sizeof(BPTR));

    /* Clear arguments when running main program */
    CopyMem("\n", BADDR(fhinput->fh_Buf), 2);
    fhinput->fh_Pos = 0;
    fhinput->fh_End = 1;

    /* Make sure PROGDIR: is correct */
    *(PathPart(path)) = '\0';
    BPTR progdir = Lock(path, SHARED_LOCK);
    BPTR oldprogdir = SetProgramDir(progdir);
    BPTR oldcurdir = CurrentDir(DupLock(progdir));

    execute_in_32_bit(start, SysBaseV0);

    UnLock(CurrentDir(oldcurdir));
    SetProgramDir(oldprogdir);
    UnLock(progdir);
}

struct timerequest tr;
struct Device *TimerBase;

STRPTR program_name = NULL;

int main(int argc, char **argv)
{
    STRPTR program_path = NULL;

    if (argc == 2)
    {
        program_path = argv[1];
        BPTR tmp = Lock(program_path, SHARED_LOCK);
        if (tmp == BNULL)
        {
            Printf("Program '%s' not found.\n", program_path);
            return 0;
        }
    }
    else
    {
        program_path = "SYSV0:Programs/MCAmiga/MCAmiga";
        // program_path = "SYSV0:Programs/HFinder/HFinder";
        // program_path = "SYSV0:Programs/ZuneARC/ZuneARC";
        // program_path = "SYSV0:Programs/Calculator";
        // program_path = "SYSV0:Programs/helloabi";
    }

    /* Save program name - dependency - this need to be set before first call to abiv0_FindTask() */
    program_name = StrDup(FilePart(program_path));

    OpenDevice("timer.device", UNIT_VBLANK, &tr.tr_node, 0);
    TimerBase = tr.tr_node.io_Device;

    /* Run emulation code with stack allocated in 31-bit memory */
    APTR stack31bit = AllocMem(64 * 1024, MEMF_CLEAR | MEMF_31BIT);
    struct StackSwapStruct sss;
    sss.stk_Lower = stack31bit;
    sss.stk_Upper = sss.stk_Lower + 64 * 1024;
    sss.stk_Pointer = sss.stk_Upper;

    struct StackSwapArgs ssa;
    ssa.Args[0] = (IPTR)program_path;

    NewStackSwap(&sss, run_emulation, &ssa);

    FreeVec(program_name);
    return 0;
}

