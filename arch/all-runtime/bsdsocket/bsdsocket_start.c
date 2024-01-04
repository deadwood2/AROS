/*
    Copyright (C) 1995-2024, The AROS Development Team. All rights reserved.
*/
/* For comments and explanation of generated code look in writestart.c source code
   of the genmodule program */
#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/resident.h>
#include <aros/libcall.h>
#include <aros/asmcall.h>
#include <aros/symbolsets.h>
#include <aros/genmodule.h>
#include <dos/dos.h>

#include "bsdsocket_libdefs.h"


#undef SysBase
#undef OOPBase
#undef UtilityBase

#include <proto/exec.h>
#include <proto/alib.h>

#include <utility/tagitem.h>
#include <sys/select.h>
#ifndef GM_SEGLIST_FIELD
static BPTR __attribute__((unused)) GM_UNIQUENAME(seglist);
#define GM_SEGLIST_FIELD(LIBBASE) (GM_UNIQUENAME(seglist))
#endif
#define LIBBASESIZE (sizeof(LIBBASETYPE) + sizeof(struct Library *)*0)
AROS_LD3(int, IoctlSocket,
         AROS_LDA(int, s, D0),
         AROS_LDA(unsigned long, request, D1),
         AROS_LDA(char *, argp, A0),
         LIBBASETYPEPTR, SocketBase, 19, BSDSocket
);
AROS_LD1(int, CloseSocket,
         AROS_LDA(int, s, D0),
         LIBBASETYPEPTR, SocketBase, 20, BSDSocket
);
AROS_LD6(int, WaitSelect,
         AROS_LDA(int, nfds, D0),
         AROS_LDA(fd_set *, readfds, A0),
         AROS_LDA(fd_set *, writefds, A1),
         AROS_LDA(fd_set *, exceptfds, A2),
         AROS_LDA(struct timeval *, timeout, A3),
         AROS_LDA(ULONG *, sigmask, D1),
         LIBBASETYPEPTR, SocketBase, 21, BSDSocket
);
AROS_LD3(void, SetSocketSignals,
         AROS_LDA(ULONG, intrmask, D0),
         AROS_LDA(ULONG, iomask, D1),
         AROS_LDA(ULONG, urgmask, D2),
         LIBBASETYPEPTR, SocketBase, 22, BSDSocket
);
AROS_LD4(LONG, ObtainSocket,
         AROS_LDA(LONG, id, D0),
         AROS_LDA(LONG, domain, D1),
         AROS_LDA(LONG, type, D2),
         AROS_LDA(LONG, protocol, D3),
         LIBBASETYPEPTR, SocketBase, 24, BSDSocket
);
AROS_LD2(LONG, ReleaseSocket,
         AROS_LDA(LONG, sd, D0),
         AROS_LDA(LONG, id, D1),
         LIBBASETYPEPTR, SocketBase, 25, BSDSocket
);
AROS_LD2(LONG, ReleaseCopyOfSocket,
         AROS_LDA(LONG, sd, D0),
         AROS_LDA(LONG, id, D1),
         LIBBASETYPEPTR, SocketBase, 26, BSDSocket
);
AROS_LD0(LONG, Errno,
         LIBBASETYPEPTR, SocketBase, 27, BSDSocket
);
AROS_LD2(void, SetErrnoPtr,
         AROS_LDA(void *, ptr, A0),
         AROS_LDA(int, size, D0),
         LIBBASETYPEPTR, SocketBase, 28, BSDSocket
);
AROS_LD1(char *, Inet_NtoA,
         AROS_LDA(unsigned long, in, D0),
         LIBBASETYPEPTR, SocketBase, 29, BSDSocket
);
AROS_LD1(unsigned long, Inet_LnaOf,
         AROS_LDA(unsigned long, in, D0),
         LIBBASETYPEPTR, SocketBase, 31, BSDSocket
);
AROS_LD1(unsigned long, Inet_NetOf,
         AROS_LDA(unsigned long, in, D0),
         LIBBASETYPEPTR, SocketBase, 32, BSDSocket
);
AROS_LD2(unsigned long, Inet_MakeAddr,
         AROS_LDA(int, net, D0),
         AROS_LDA(int, lna, D1),
         LIBBASETYPEPTR, SocketBase, 33, BSDSocket
);
AROS_LD2(int, Dup2Socket,
         AROS_LDA(int, fd1, D0),
         AROS_LDA(int, fd2, D1),
         LIBBASETYPEPTR, SocketBase, 44, BSDSocket
);
AROS_LD1(ULONG, SocketBaseTagList,
         AROS_LDA(struct TagItem *, tagList, A0),
         LIBBASETYPEPTR, SocketBase, 49, BSDSocket
);
AROS_LD1(LONG, GetSocketEvents,
         AROS_LDA(ULONG *, eventsp, A0),
         LIBBASETYPEPTR, SocketBase, 50, BSDSocket
);

extern int GM_UNIQUENAME(End)(void);
extern const APTR GM_UNIQUENAME(FuncTable)[];
struct InitTable
{
    IPTR              Size;
    const APTR       *FuncTable;
    struct DataTable *DataTable;
    APTR              InitLibTable;
};
static const struct InitTable GM_UNIQUENAME(InitTable);

extern const char GM_UNIQUENAME(LibName)[];
extern const char GM_UNIQUENAME(LibID)[];
extern const char GM_UNIQUENAME(Copyright)[];

#define __freebase(LIBBASE)\
do {\
    UWORD negsize, possize;\
    UBYTE *negptr = (UBYTE *)LIBBASE;\
    negsize = ((struct Library *)LIBBASE)->lib_NegSize;\
    negptr -= negsize;\
    possize = ((struct Library *)LIBBASE)->lib_PosSize;\
    FreeMem (negptr, negsize+possize);\
} while(0)

AROS_UFP3 (LIBBASETYPEPTR, GM_UNIQUENAME(InitLib),
    AROS_UFPA(LIBBASETYPEPTR, LIBBASE, D0),
    AROS_UFPA(BPTR, segList, A0),
    AROS_UFPA(struct ExecBase *, sysBase, A6)
);
AROS_LD1(BPTR, GM_UNIQUENAME(ExpungeLib),
    AROS_LDA(LIBBASETYPEPTR, extralh, D0),
    LIBBASETYPEPTR, LIBBASE, 3, BSDSocket
);

__section(".text.romtag") struct Resident const GM_UNIQUENAME(ROMTag) =
{
    RTC_MATCHWORD,
    (struct Resident *)&GM_UNIQUENAME(ROMTag),
    (APTR)&GM_UNIQUENAME(End),
    RESIDENTFLAGS,
    VERSION_NUMBER,
    NT_LIBRARY,
    RESIDENTPRI,
    (CONST_STRPTR)&GM_UNIQUENAME(LibName)[0],
    (CONST_STRPTR)&GM_UNIQUENAME(LibID)[6],
    (APTR)&GM_UNIQUENAME(InitTable)
};

__section(".text.romtag") static struct InitTable const GM_UNIQUENAME(InitTable) =
{
    LIBBASESIZE,
    &GM_UNIQUENAME(FuncTable)[0],
    NULL,
    (APTR)GM_UNIQUENAME(InitLib)
};

#if defined(MOD_NAME_STRING)
__section(".text.romtag") const char GM_UNIQUENAME(LibName)[] = MOD_NAME_STRING;
#endif
#if defined(VERSION_STRING)
__section(".text.romtag") const char GM_UNIQUENAME(LibID)[] = VERSION_STRING;
#endif
#if defined(COPYRIGHT_STRING)
__section(".text.romtag") const char GM_UNIQUENAME(Copyright)[] = COPYRIGHT_STRING;
#endif

THIS_PROGRAM_HANDLES_SYMBOLSET(INIT)
THIS_PROGRAM_HANDLES_SYMBOLSET(EXIT)
DECLARESET(INIT)
DECLARESET(EXIT)
THIS_PROGRAM_HANDLES_SYMBOLSET(PROGRAM_ENTRIES)
DECLARESET(PROGRAM_ENTRIES)
THIS_PROGRAM_HANDLES_SYMBOLSET(CTORS)
THIS_PROGRAM_HANDLES_SYMBOLSET(DTORS)
DECLARESET(CTORS)
DECLARESET(DTORS)
THIS_PROGRAM_HANDLES_SYMBOLSET(INIT_ARRAY)
THIS_PROGRAM_HANDLES_SYMBOLSET(FINI_ARRAY)
DECLARESET(INIT_ARRAY)
DECLARESET(FINI_ARRAY)
THIS_PROGRAM_HANDLES_SYMBOLSET(INITLIB)
THIS_PROGRAM_HANDLES_SYMBOLSET(EXPUNGELIB)
DECLARESET(INITLIB)
DECLARESET(EXPUNGELIB)
THIS_PROGRAM_HANDLES_SYMBOLSET(OPENLIB)
THIS_PROGRAM_HANDLES_SYMBOLSET(CLOSELIB)
DECLARESET(OPENLIB)
DECLARESET(CLOSELIB)

extern const LONG __aros_libreq_SysBase __attribute__((weak));

AROS_UFH3 (LIBBASETYPEPTR, GM_UNIQUENAME(InitLib),
    AROS_UFHA(LIBBASETYPEPTR, LIBBASE, D0),
    AROS_UFHA(BPTR, segList, A0),
    AROS_UFHA(struct ExecBase *, sysBase, A6)
)
{
    AROS_USERFUNC_INIT

    int ok;
    int initcalled = 0;
    struct ExecBase *SysBase = sysBase;

#ifdef GM_SYSBASE_FIELD
    GM_SYSBASE_FIELD(LIBBASE) = (APTR)SysBase;
#endif
    if (!SysBase || SysBase->LibNode.lib_Version < __aros_libreq_SysBase)
        return NULL;

#ifdef GM_OOPBASE_FIELD
    GM_OOPBASE_FIELD(LIBBASE) = OpenLibrary("oop.library",0);
    if (GM_OOPBASE_FIELD(LIBBASE) == NULL)
        return NULL;
#endif
#if defined(REVISION_NUMBER)
    ((struct Library *)LIBBASE)->lib_Revision = REVISION_NUMBER;
#endif
    GM_SEGLIST_FIELD(LIBBASE) = segList;
    if (set_call_funcs(SETNAME(INIT), 1, 1) &&1)
    {
        set_call_funcs(SETNAME(CTORS), -1, 0);
        set_call_funcs(SETNAME(INIT_ARRAY), 1, 0);

        initcalled = 1;
        ok = set_call_libfuncs(SETNAME(INITLIB), 1, 1, LIBBASE);
    }
    else
        ok = 0;

    if (!ok)
    {
        if (initcalled)
            set_call_libfuncs(SETNAME(EXPUNGELIB), -1, 0, LIBBASE);
        set_call_funcs(SETNAME(FINI_ARRAY), -1, 0);
        set_call_funcs(SETNAME(DTORS), 1, 0);
        set_call_funcs(SETNAME(EXIT), -1, 0);

        __freebase(LIBBASE);
        return NULL;
    }
    else
    {
        return  LIBBASE;
    }

    AROS_USERFUNC_EXIT
}

AROS_LH1 (LIBBASETYPEPTR, GM_UNIQUENAME(OpenLib),
    AROS_LHA (ULONG, version, D0),
    LIBBASETYPEPTR, LIBBASE, 1, BSDSocket
)
{
    AROS_LIBFUNC_INIT

    if ( set_call_libfuncs(SETNAME(OPENLIB), 1, 1, LIBBASE) )
    {
        ((struct Library *)LIBBASE)->lib_OpenCnt++;
        ((struct Library *)LIBBASE)->lib_Flags &= ~LIBF_DELEXP;
        return LIBBASE;
    }

    return NULL;

    AROS_LIBFUNC_EXIT
}

AROS_LH0 (BPTR, GM_UNIQUENAME(CloseLib),
    LIBBASETYPEPTR, LIBBASE, 2, BSDSocket
)
{
    AROS_LIBFUNC_INIT

    ((struct Library *)LIBBASE)->lib_OpenCnt--;
    set_call_libfuncs(SETNAME(CLOSELIB), -1, 0, LIBBASE);
    if
    (
        (((struct Library *)LIBBASE)->lib_OpenCnt == 0)
        && (((struct Library *)LIBBASE)->lib_Flags & LIBF_DELEXP)
    )
    {
        return AROS_LC1(BPTR, GM_UNIQUENAME(ExpungeLib),
                   AROS_LCA(LIBBASETYPEPTR, LIBBASE, D0),
                   LIBBASETYPEPTR, LIBBASE, 3, BSDSocket
        );
    }

    return BNULL;

    AROS_LIBFUNC_EXIT
}

AROS_LH1 (BPTR, GM_UNIQUENAME(ExpungeLib),
    AROS_LHA(LIBBASETYPEPTR, extralh, D0),
    LIBBASETYPEPTR, LIBBASE, 3, BSDSocket
)
{
    AROS_LIBFUNC_INIT

#ifdef GM_SYSBASE_FIELD
    struct ExecBase *SysBase = (struct ExecBase *)GM_SYSBASE_FIELD(LIBBASE);
#endif

    if ( ((struct Library *)LIBBASE)->lib_OpenCnt == 0 )
    {
        BPTR seglist = GM_SEGLIST_FIELD(LIBBASE);

        if(!set_call_libfuncs(SETNAME(EXPUNGELIB), -1, 1, LIBBASE))
        {
            ((struct Library *)LIBBASE)->lib_Flags |= LIBF_DELEXP;
            return BNULL;
        }

        Remove((struct Node *)LIBBASE);

        set_call_funcs(SETNAME(FINI_ARRAY), -1, 0);
        set_call_funcs(SETNAME(DTORS), 1, 0);
        set_call_funcs(SETNAME(EXIT), -1, 0);

        __freebase(LIBBASE);

        return seglist;
    }

    ((struct Library *)LIBBASE)->lib_Flags |= LIBF_DELEXP;

    return BNULL;

    AROS_LIBFUNC_EXIT
}

AROS_LH0 (LIBBASETYPEPTR, GM_UNIQUENAME(ExtFuncLib),
    LIBBASETYPEPTR, LIBBASE, 4, BSDSocket
)
{
    AROS_LIBFUNC_INIT
    return NULL;
    AROS_LIBFUNC_EXIT
}

DEFINESET(INIT)
DEFINESET(EXIT)
DEFINESET(CTORS)
DEFINESET(DTORS)
DEFINESET(INIT_ARRAY)
DEFINESET(FINI_ARRAY)
DEFINESET(INITLIB)
DEFINESET(EXPUNGELIB)
DEFINESET(OPENLIB)
DEFINESET(CLOSELIB)


__section(".text.romtag") const APTR GM_UNIQUENAME(FuncTable)[]=
{
    &AROS_SLIB_ENTRY(GM_UNIQUENAME(OpenLib),BSDSocket,1),
    &AROS_SLIB_ENTRY(GM_UNIQUENAME(CloseLib),BSDSocket,2),
    &AROS_SLIB_ENTRY(GM_UNIQUENAME(ExpungeLib),BSDSocket,3),
    &AROS_SLIB_ENTRY(GM_UNIQUENAME(ExtFuncLib),BSDSocket,4),
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    /* Version 4 */
    &AROS_SLIB_ENTRY(IoctlSocket,BSDSocket,19),
    &AROS_SLIB_ENTRY(CloseSocket,BSDSocket,20),
    &AROS_SLIB_ENTRY(WaitSelect,BSDSocket,21),
    &AROS_SLIB_ENTRY(SetSocketSignals,BSDSocket,22),
    NULL,
    &AROS_SLIB_ENTRY(ObtainSocket,BSDSocket,24),
    &AROS_SLIB_ENTRY(ReleaseSocket,BSDSocket,25),
    &AROS_SLIB_ENTRY(ReleaseCopyOfSocket,BSDSocket,26),
    &AROS_SLIB_ENTRY(Errno,BSDSocket,27),
    &AROS_SLIB_ENTRY(SetErrnoPtr,BSDSocket,28),
    &AROS_SLIB_ENTRY(Inet_NtoA,BSDSocket,29),
    NULL,
    &AROS_SLIB_ENTRY(Inet_LnaOf,BSDSocket,31),
    &AROS_SLIB_ENTRY(Inet_NetOf,BSDSocket,32),
    &AROS_SLIB_ENTRY(Inet_MakeAddr,BSDSocket,33),
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    &AROS_SLIB_ENTRY(Dup2Socket,BSDSocket,44),
    NULL,
    NULL,
    NULL,
    NULL,
    &AROS_SLIB_ENTRY(SocketBaseTagList,BSDSocket,49),
    &AROS_SLIB_ENTRY(GetSocketEvents,BSDSocket,50),
    (void *)-1
};
