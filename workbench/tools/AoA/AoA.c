#include <dos/bptr.h>
#include <exec/types.h>
#include <proto/dos.h>
#include <proto/exec.h>


BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);

int main()
{
    BPTR seg = LoadSeg32("SYS:Calculator32", DOSBase);
    APTR (*start)() = (APTR)((IPTR)BADDR(seg) + sizeof(BPTR));

    /* Set start at first instruction (skip Seg header) */
    start = (APTR)((IPTR)start + 13);

    /*  Switch to CS = 0x23 during FAR call. This switches 32-bit emulation mode.
        Next, load 0x2B to DS (needed under 32-bit) and NEAR jump to 32-bit code */
    __asm__ volatile(
    "   movl %0, %%ebx\n"
    "   subq $8, %%rsp\n"
    "   movl $0x23, 4(%%rsp)\n"
    "   lea  tramp, %%rax\n"
    "   movl %%eax, (%%rsp)\n"
    "   lcall *(%%rsp)\n"
    "tramp:\n"
    "   .code32\n"
    "   push $0x2b\n"
    "   pop %%ds\n"
    "   int3\n"
    "   jmp *%%ebx\n"
        :: "m"(start) :);

    return 0;
}

