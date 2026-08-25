/*
    Copyright (C) 2024-2026, The AROS Development Team. All rights reserved.
*/

#include <dos/bptr.h>
#include <exec/types.h>
#include <aros/debug.h>
#include <proto/timer.h>
#include <exec/rawfmt.h>
#include <proto/dos.h>
#include <string.h>

#include "abiv0/include/exec/functions.h"
#include "abiv0/include/exec/proxy_structures.h"
#include "abiv0/include/aros/proxy.h"
#include "abiv0/include/aros/cpu.h"
#include "abiv0/include/aros/call32.h"
#include "abiv0/include/input/structures.h"

#include "abiv0/libs/exec/exec_libraries.h"
#include "abiv0/libs/layers/layers_init.h"
#include "abiv0/libs/cybergraphics/cybergraphics_init.h"
#include "abiv0/devs/timer/timer_init.h"
#include "abiv0/devs/input/input_init.h"
#include "abiv0/devs/console/console_init.h"

#include "abiv0/support.h"

const TEXT version_string[] = "$VER: EmuV0 1.13 (25.08.2026)";

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
unhandledCodePath(__func__, "!RTF_AUTOINIT", 0, 0);
    }

    D(bug("InitResident end 0x%p (\"%s\"), result 0x%p", resident, resident->rt_Name, library));

    return library;
} /* shallow_InitResident32 */

BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);

void init_graphics(struct ExecBaseV0 *);
void exit_graphics();
void init_intuition(struct ExecBaseV0 *, struct DeviceProxy *);
void exit_intuition();
void init_dos(struct ExecBaseV0 *);
void exit_dos();
struct ExecBaseV0 *init_exec();
void exit_exec();
void exec_expunge_libraries(struct ExecBaseV0 *);
void exec_force_expunge(struct ExecBaseV0 *SysBaseV0, STRPTR libname);

#if 0
void LibList(struct ExecBaseV0 *SysBaseV0)
{
    struct NodeV0 *node;
    struct ListV0 *list = &SysBaseV0->LibList;

    bug ("Listing Libraries\n");
    /* Look through the list */
    for (node=GetHeadV0(list); node; node=GetSuccV0(node))
    {
        /* Only compare the names if this node has one. */
        if(node->ln_Name)
        {
            bug("   %s -> %d\n", node->ln_Name, ((struct LibraryV0 *)node)->lib_OpenCnt);
        }
    }


}
#endif

void execute_in_32_bit(APTR start, CONST_STRPTR argstr, LONG argsize, struct ExecBaseV0 *SysBaseV0)
{
    __asm__ volatile(
    "   subq $16, %%rsp\n"
    "   movl $0, %%eax\n"
    "   movl %%eax, 12(%%rsp)\n" //pad
    "   movl %3, %%eax\n"
    "   movl %%eax, 8(%%rsp)\n" //SysBase
    "   movl %2, %%eax\n"
    "   movl %%eax, 4(%%rsp)\n" //argsize
    "   movl %1, %%eax\n"
    "   movl %%eax, (%%rsp)\n" //argstr
    "   movl %0, %%edx\n"  // start
    ENTER32
    "   call *%%edx\n"
    ENTER64
    "   addq $16, %%rsp"// Clean up stack
        :: "mr"(start), "mr" (argstr), "mr" (argsize), "mr" (SysBaseV0)
        : SCRATCH_REGS_64_TO_32 );
}

void refresh_g_v0maintask();

STRPTR emu_argstr = NULL;
LONG emu_argsize = 0;

//     // This code is used to generate proxies for unhandled functions
//     LONG slvo = 5, elvo = 45;
//     STRPTR libname = "layers.library";
//     struct LibraryV0 *libbase = abiv0LayersBase;
//     STRPTR libbasename = "abiv0LayersBase";
//     for (LONG i = slvo; i <= elvo; i++)
//     {
//         if (__AROS_GETVECADDRV0(libbase, i) == (APTR32)(IPTR)NULL)
//         {
//             bug("static void abiv0_unhandledLVO%d() { unhandledLibraryFunction(\"%s\", %d); }\n", i, libname, i);
//             bug("MAKE_PROXY_ARG_1(unhandledLVO%d)\n", i);
//         }
//     }

//     bug("void _Unhandled_init(struct LibraryV0 *%s)\n", libbasename);
//     bug("{\n");
//     for (LONG i = slvo; i <= elvo; i++)
//     {
//         if (__AROS_GETVECADDRV0(libbase, i) == (APTR32)(IPTR)NULL)
//         {
//             bug("    __AROS_SETVECADDRV0(%s,  %d, (APTR32)(IPTR)proxy_unhandledLVO%d);\n", libbasename, i, i);
//         }
//     }
//     bug("}\n");

// asm("int3");

LONG_FUNC run_emulation(CONST_STRPTR program_path)
{
    TEXT path[64];
    TEXT currdir[256];

    /* Init ROM */
    struct ExecBaseV0 *SysBaseV0 = init_exec();

    init_timer(SysBaseV0);

    init_input(SysBaseV0);

    init_console(SysBaseV0);

    init_dos(SysBaseV0);

    init_graphics(SysBaseV0);

    init_layers(SysBaseV0);

    init_cybergraphics(SysBaseV0);

    init_intuition(SysBaseV0, abiv0TimerBase);

    /* Install datatypes */
    NewRawDoFmt("EmuV0:C/AddDataTypes", RAWFMTFUNC_STRING, path);
    BPTR adtseg = LoadSeg32(path, DOSBase);
    APTR (*adtstart)() = (APTR)((IPTR)BADDR(adtseg) + sizeof(BPTR));
    /* Inject arguments for AddDataTypes*/
    struct FileHandle *fhinput = BADDR(Input());
    CopyMem("REFRESH\n", BADDR(fhinput->fh_Buf), 9);
    fhinput->fh_Pos = 0;
    fhinput->fh_End = 9;

    execute_in_32_bit(adtstart, "\n", 1, SysBaseV0);
    UnLoadSeg(adtseg);

    /* Start Program */
    NewRawDoFmt("%s", RAWFMTFUNC_STRING, path, program_path);
    BPTR seg = LoadSeg32(path, DOSBase);
    APTR (*start)() = (APTR)((IPTR)BADDR(seg) + sizeof(BPTR));

    /* Set arguments for main program */
    if (fhinput->fh_BufSize < emu_argsize) unhandledCodePath(__func__, "Arguments", emu_argsize, fhinput->fh_BufSize);
    CopyMem(emu_argstr, BADDR(fhinput->fh_Buf), emu_argsize);
    fhinput->fh_Pos = 0;
    fhinput->fh_End = emu_argsize;

    /* Make sure PROGDIR: is correct */
    *(PathPart(path)) = '\0';
    BPTR progdir = Lock(path, SHARED_LOCK);
    BPTR oldprogdir = SetProgramDir(progdir);
    BPTR oldcurdir = CurrentDir(DupLock(progdir));
    GetCurrentDirName(currdir, 256);
    SetCurrentDirName(path);
    refresh_g_v0maintask();

    execute_in_32_bit(start, emu_argstr, emu_argsize, SysBaseV0);
    UnLoadSeg(seg);

    SetCurrentDirName(currdir);
    UnLock(CurrentDir(oldcurdir));
    SetProgramDir(oldprogdir);
    UnLock(progdir);

    exec_expunge_libraries(SysBaseV0);

    exec_force_expunge(SysBaseV0, "icon.library");
    exec_force_expunge(SysBaseV0, "iffparse.library");
    exec_force_expunge(SysBaseV0, "stdlib.library");

    /* Finish expunge for partial libraries */
    exit_cybergraphics();

    exit_layers();
    exit_intuition();
    exit_graphics();
    exit_dos();

    exit_console(SysBaseV0);
    exit_input(SysBaseV0);
    exit_timer(SysBaseV0);

    exit_exec();
}

struct timerequest tr;
struct Device *TimerBase;

STRPTR program_name = NULL;

int main(int argc, char **argv)
{
    STRPTR program_path = NULL;

    if (argc > 1)
    {
        struct Process *me;
        STRPTR p; LONG qc = 0;
        program_path = argv[1];
        BPTR tmp = Lock(program_path, SHARED_LOCK);
        if (tmp == BNULL)
        {
            Printf("Program '%s' not found.\n", program_path);
            return 0;
        }

        /* Build emu_argstr */
        p = ((struct Process *)FindTask(NULL))->pr_Arguments;
        /* Assume worst case: "program with space" "arg1 with space" arg arg3 \n */
        /* Get rid off '"program with space"' or 'program' */
        while (*p != '\n')
        {
            if (*p == '"') qc++;
            if (*p == ' ')
                if (qc == 0 || qc == 2) { p++; break; }
            p++;
        }

        emu_argsize = strlen(p);
        emu_argstr = AllocMem(emu_argsize + 1, MEMF_31BIT | MEMF_CLEAR);
        CopyMem(p, emu_argstr, emu_argsize);
    }
    else
    {
        Printf("EmuV0 needs at least one argument - program name\n");
        return 0;
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
    if (emu_argstr) FreeMem(emu_argstr, emu_argsize + 1);
    return 0;
}

