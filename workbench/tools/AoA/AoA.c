#include <dos/bptr.h>
#include <exec/types.h>
#include <proto/dos.h>
#include <proto/exec.h>


BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);

struct ExecBaseABIv0
{
    LONG dummy;
};

struct DosLibraryABIv0
{
    LONG dummy;
};

struct DosLibraryABIv0 *abiv0DOSBase;

APTR abiv0_OpenLibrary()
{
    return abiv0DOSBase;
}

void proxy_OpenLibrary();

void dummy_OpenLibrary()
{
    __asm__ volatile(
    "   .code32\n"
    "   .globl proxy_OpenLibrary\n"
    "proxy_OpenLibrary:\n"
    "   push $0x33\n"
    "   lea swTO64A, %%ecx\n"
    "   push %%ecx\n"
    "   lret\n"
    "   .code64\n"
    "swTO64A:\n"
    "   call abiv0_OpenLibrary\n"
    "   subq $8, %%rsp\n"
    "   movl $0x23, 4(%%rsp)\n"
    "   lea swTO32A, %%ecx\n"
    "   movl %%ecx, (%%rsp)\n"
    "   lret\n"
    "   .code32\n"
    "swTO32A:\n"
    "   ret\n"
    "   .code64\n"
    ::);
}

void abiv0_PutStr()
{
    PutStr("Boo!\n");
}

void proxy_PutStr();

void dummy_PutStr()
{
    __asm__ volatile(
    "   .code32\n"
    "   .globl proxy_PutStr\n"
    "proxy_PutStr:\n"
    "   push $0x33\n"
    "   lea swTO64B, %%ecx\n"
    "   push %%ecx\n"
    "   lret\n"
    "   .code64\n"
    "swTO64B:\n"
    "   call abiv0_PutStr\n"
    "   subq $8, %%rsp\n"
    "   movl $0x23, 4(%%rsp)\n"
    "   lea swTO32B, %%ecx\n"
    "   movl %%ecx, (%%rsp)\n"
    "   lret\n"
    "   .code32\n"
    "swTO32B:\n"
    "   ret\n"
    "   .code64\n"
    ::);
}

void abiv0_CloseLibrary()
{
}

void proxy_CloseLibrary();

void dummy_CloseLibrary()
{
    __asm__ volatile(
    "   .code32\n"
    "   .globl proxy_CloseLibrary\n"
    "proxy_CloseLibrary:\n"
    "   push $0x33\n"
    "   lea swTO64C, %%ecx\n"
    "   push %%ecx\n"
    "   lret\n"
    "   .code64\n"
    "swTO64C:\n"
    "   call abiv0_CloseLibrary\n"
    "   subq $8, %%rsp\n"
    "   movl $0x23, 4(%%rsp)\n"
    "   lea swTO32C, %%ecx\n"
    "   movl %%ecx, (%%rsp)\n"
    "   lret\n"
    "   .code32\n"
    "swTO32C:\n"
    "   ret\n"
    "   .code64\n"
    ::);
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

