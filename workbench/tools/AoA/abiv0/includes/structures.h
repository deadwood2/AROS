#ifndef ABIV0_STRUCTURES_H
#define ABIV0_STRUCTURES_H

#include <exec/types.h>

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

struct __mayalias ResidentV0;
struct ResidentV0
{
    UWORD                  rt_MatchWord; /* equal to RTC_MATCHWORD (see below) */
    APTR32                 rt_MatchTag;  /* Pointer to this struct */
    APTR32                 rt_EndSkip;
    UBYTE                  rt_Flags;     /* see below */
    UBYTE                  rt_Version;
    UBYTE                  rt_Type;
    BYTE                   rt_Pri;
    APTR32                 rt_Name;
    APTR32                 rt_IdString;
    APTR32                 rt_Init;

    /* Extension taken over from MorphOS. Only valid
       if RTF_EXTENDED is set */

    UWORD                  rt_Revision;
    APTR32                 rt_Tags;
};

#define RTC_MATCHWORD  (0x4AFC)

struct ExecBaseABIv0
{
    LONG dummy;
};

struct DosLibraryV0
{
    /* A normal library-base as defined in <exec/libraries.h>. */
    struct LibraryV0 dl_lib;
};

#endif
