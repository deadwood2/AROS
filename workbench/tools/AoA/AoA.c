#include <dos/bptr.h>
#include <exec/types.h>
#include <aros/debug.h>
#include <proto/timer.h>

#include <proto/dos.h>

#include "abiv0/include/exec/functions.h"
#include "abiv0/include/aros/proxy.h"
#include "abiv0/include/aros/cpu.h"

struct LibraryV0 *abiv0TimerBase;

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

void abiv0_GetSysTime(struct timeval *dest, struct LibraryV0 *TimerBaseV0)
{
    return GetSysTime(dest);
}
MAKE_PROXY_ARG_2(GetSysTime)

#include <proto/cybergraphics.h>
#include "abiv0/include/graphics/structures.h"

ULONG abiv0_FillPixelArray(struct RastPortV0 *rp, UWORD destx, UWORD desty, UWORD width, UWORD height, ULONG pixel)
{
    struct RastPort *rpnative = (struct RastPort *)*(IPTR *)&rp->longreserved;
    return FillPixelArray(rpnative, destx, desty, width, height, pixel);
}
MAKE_PROXY_ARG_6(FillPixelArray)

BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);
struct ResidentV0 * findResident(BPTR seg, CONST_STRPTR name);

void init_graphics(struct ExecBaseV0 *);
void init_intuition(struct ExecBaseV0 *);
void init_dos(struct ExecBaseV0 *);
struct ExecBaseV0 *init_exec();

LONG_FUNC run_emulation()
{
    /* Init ROM */
    struct ExecBaseV0 *SysBaseV0 = init_exec();

    APTR tmp = AllocMem(1024, MEMF_31BIT | MEMF_CLEAR);
    abiv0TimerBase = (tmp + 512);
    __AROS_SETVECADDRV0(abiv0TimerBase, 11, (APTR32)(IPTR)proxy_GetSysTime);

    init_dos(SysBaseV0);

    init_intuition(SysBaseV0);

    init_graphics(SysBaseV0);

    BPTR layersseg = LoadSeg32("SYS:Libs32/partial/layers.library", DOSBase);
    struct ResidentV0 *layersres = findResident(layersseg, NULL);
    struct LibraryV0 *abiv0LayersBase = shallow_InitResident32(layersres, layersseg, SysBaseV0);
    /* Remove all vectors for now */
    for (int i = 1; i <= 45; i++) __AROS_SETVECADDRV0(abiv0LayersBase, i, 0);
    __AROS_SETVECADDRV0(abiv0LayersBase,   1, (APTR32)(IPTR)proxy_Layers_OpenLib);

    BPTR cgfxseg = LoadSeg32("SYS:Libs32/partial/cybergraphics.library", DOSBase);
    struct ResidentV0 *cgfxres = findResident(cgfxseg, NULL);
    struct LibraryV0 *abiv0CyberGfxBase = shallow_InitResident32(cgfxres, cgfxseg, SysBaseV0);
    /* Remove all vectors for now (leave LibOpen) */
    for (int i = 5; i <= 38; i++) __AROS_SETVECADDRV0(abiv0CyberGfxBase, i, 0);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase, 25, (APTR32)(IPTR)proxy_FillPixelArray);




    /* Start Program */

    BPTR seg = LoadSeg32("SYS:Calculator", DOSBase);
    // BPTR seg = LoadSeg32("SYS:helloabi", DOSBase);
    APTR (*start)() = (APTR)((IPTR)BADDR(seg) + sizeof(BPTR));

    /* Set start at first instruction (skip Seg header) */
    start = (APTR)((IPTR)start + 13);

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
        :: "m"(start), "m" (SysBaseV0) :);
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

