#include <dos/bptr.h>
#include <exec/types.h>
#include <proto/dos.h>
#include <proto/exec.h>

typedef ULONG APTR32;

struct __mayalias NodeV0;
struct NodeV0
{
    APTR32  ln_Succ;
    APTR32  ln_Pred;
    APTR32  ln_Name;
    UBYTE   ln_Type;
    BYTE    ln_Pri;
};

struct LibraryV0 {
    struct  NodeV0 lib_Node;
    UBYTE   lib_Flags;
    UBYTE   lib_pad;
    UWORD   lib_NegSize;	    /* number of bytes before library */
    UWORD   lib_PosSize;	    /* number of bytes after library */
    UWORD   lib_Version;	    /* major */
    UWORD   lib_Revision;	    /* minor */
#ifdef AROS_NEED_LONG_ALIGN
    UWORD   lib_pad1;		    /* make sure it is longword aligned */
#endif
    APTR32  lib_IdString;	    /* ASCII identification */
    ULONG   lib_Sum;		    /* the checksum */
    UWORD   lib_OpenCnt;	    /* how many people use us right now? */
#ifdef AROS_NEED_LONG_ALIGN
    UWORD   lib_pad2;		    /* make sure it is longword aligned */
#endif
};

BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);

struct ExecBaseABIv0
{
    LONG dummy;
};

struct DosLibraryV0
{
    /* A normal library-base as defined in <exec/libraries.h>. */
    struct LibraryV0 dl_lib;
};

struct DosLibraryV0 *abiv0DOSBase;

#define EXTER_PROXY(name)       \
__asm__ volatile(               \
    "   .code32\n"              \
    "proxy_" #name ":\n"

#define LEAVE_PROXY             \
    "   ret\n"                  \
    "   .code64\n"              \
    ::);

#define ENTER64                 \
    "   push $0x33\n"           \
    "   lea 1f, %%ecx\n"        \
    "   push %%ecx\n"           \
    "   lret\n"                 \
    "   .code64\n"              \
    "1:\n"

#define ENTER32                 \
    "   subq $8, %%rsp\n"       \
    "   movl $0x23, 4(%%rsp)\n" \
    "   lea 2f,%%ecx\n"         \
    "   movl %%ecx, (%%rsp)\n"  \
    "   lret\n"                 \
    "   .code32\n"              \
    "2:\n"

#define CALL_IMPL64(name)       \
    "   call abiv0_" #name "\n"

#define COPY_ARG_1              \
    "   movl 4(%%rsp), %%edi\n"

APTR abiv0_OpenLibrary()
{
    return abiv0DOSBase;
}

void proxy_OpenLibrary();
void dummy_OpenLibrary()
{
    EXTER_PROXY(OpenLibrary)
    ENTER64
    COPY_ARG_1
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

