#include <dos/bptr.h>
#include <exec/types.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <aros/debug.h>

#include <stdio.h>
#include <string.h>

#include "exec/functions.h"
#include "aros/asm.h"

BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);

struct DosLibraryV0 *abiv0DOSBase;



APTR abiv0_AllocMem(ULONG byteSize, ULONG requirements, struct ExecBaseV0 *SysBaseV0)
{
    return AllocMem(byteSize, requirements | MEMF_31BIT);
}

APTR abiv0_OpenLibrary(CONST_STRPTR name, ULONG version, struct ExecBaseV0 *SysBaseV0)
{
    bug("abiv0_OpenLibrary: %s\n", name);
    TEXT buff[64];

    if (strcmp(name, "dos.library") == 0)
        return abiv0DOSBase;

    sprintf(buff, "SYS:Libs32/%s", name);

    BPTR seglist = LoadSeg32(buff, DOSBase);


    BPTR seg = seglist;

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
                // node = FindName(list, res->rt_Name);
                // Permit();
                D(bug("[LDInit] Done calling InitResident(%p) on %s, seg %p, node %p\n", res, res->rt_Name, BADDR(seglist), node));

                // return (struct Library*)node;
                return NULL;
            }
        }
        seg = *(BPTR *)BADDR(seg);
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
    CALL_IMPL64(OpenLibrary)
    ENTER32
    LEAVE_PROXY
}

void abiv0_PutStr(CONST_STRPTR text)
{
    PutStr(text);
}

void proxy_PutStr();
void dummy_PutStr()
{
    EXTER_PROXY(PutStr)
    ENTER64
    COPY_ARG_1
    CALL_IMPL64(PutStr)
    ENTER32
    LEAVE_PROXY
}

void abiv0_CloseLibrary()
{
}

void proxy_CloseLibrary();
void dummy_CloseLibrary()
{
    EXTER_PROXY(CloseLibrary)
    ENTER64
    COPY_ARG_1
    CALL_IMPL64(CloseLibrary)
    ENTER32
    LEAVE_PROXY
}

int main()
{
    BPTR seg = LoadSeg32("SYS:helloabi", DOSBase);
    APTR (*start)() = (APTR)((IPTR)BADDR(seg) + sizeof(BPTR));

    /* Set start at first instruction (skip Seg header) */
    start = (APTR)((IPTR)start + 13);

    APTR tmp, lvo;

    tmp = AllocMem(1024, MEMF_31BIT | MEMF_CLEAR);
    struct ExecBaseABIv0 *sysbase = (tmp + 512);

    lvo = ((APTR)sysbase - 0x170);
    *((ULONG *)lvo) = (ULONG)(IPTR)&proxy_OpenLibrary;
    lvo = ((APTR)sysbase - 0x114);
    *((ULONG *)lvo) = (ULONG)(IPTR)&proxy_CloseLibrary;

    tmp = AllocMem(2048, MEMF_31BIT | MEMF_CLEAR);
    abiv0DOSBase = (tmp + 1024);
    abiv0DOSBase->dl_lib.lib_Version = DOSBase->dl_lib.lib_Version;

    lvo = ((APTR)abiv0DOSBase - 0x278);
    *((ULONG *)lvo) = (ULONG)(IPTR)&proxy_PutStr;


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
        :: "m"(start), "m" (sysbase) :);

    return 0;
}

