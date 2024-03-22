#ifndef ABIV0_EXEC_STRUCTURES_H
#define ABIV0_EXEC_STRUCTURES_H

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

struct __mayalias MinNodeV0;
struct MinNodeV0
{
    APTR32 mln_Succ;
    APTR32 mln_Pred;
};


struct __mayalias ListV0;
struct ListV0
{
    APTR32  lh_Head;
    APTR32  lh_Tail;
    union
    {
        APTR32 lh_TailPred;
        APTR32 lh_TailPred_;
    };
    UBYTE	  lh_Type;
    UBYTE	  l_pad;
};

struct __mayalias MinListV0;
struct MinListV0
{
    APTR32 mlh_Head;
    APTR32 mlh_Tail;
    union
    {
        APTR32 mlh_TailPred;
        APTR32 mlh_TailPred_;
    };
};

#define ForeachNodeV0(list, node)                        \
for                                                    \
(                                                      \
    *(void **)&node = (void *)(IPTR)(((struct ListV0 *)(list))->lh_Head); \
    ((struct NodeV0 *)(node))->ln_Succ;                  \
    *(void **)&node = (void *)(IPTR)(((struct NodeV0 *)(node))->ln_Succ)  \
)

#define NEWLISTV0(_l)                                     \
do                                                      \
{                                                       \
    struct ListV0 *__aros_list_tmp = (struct ListV0 *)(_l), \
                *l = __aros_list_tmp;                   \
                                                        \
    l->lh_TailPred_= (APTR32)(IPTR)l;                   \
    l->lh_Tail     = 0;                                 \
    l->lh_Head     = (APTR32)(IPTR)(struct NodeV0 *)&l->lh_Tail; \
} while (0)

#define GetHeadV0(_l)                                   \
({                                                      \
    struct ListV0 *__aros_list_tmp = (struct ListV0 *)(_l), \
                *l = __aros_list_tmp;                   \
                                                        \
   ((struct NodeV0 *)(IPTR)l->lh_Head)->ln_Succ ? (struct NodeV0 *)(IPTR)l->lh_Head : (struct NodeV0 *)0; \
})

#define GetSuccV0(_n)                                    \
({                                                       \
    struct NodeV0 *__aros_node_tmp = (struct NodeV0 *)(_n),  \
                *n = __aros_node_tmp;                    \
                                                         \
    (n && n->ln_Succ && ((struct NodeV0 *)(IPTR)n->ln_Succ)->ln_Succ) ? (struct NodeV0 *)(IPTR)n->ln_Succ : (struct NodeV0 *)0; \
})
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

#define RTF_AUTOINIT   (1<<7)

#define RTF_EXTENDED   (1<<6) /* MorphOS extension: extended
                                 structure fields are valid */

struct InterruptV0
{
    struct NodeV0 is_Node;
    APTR32        is_Data;
    APTR32        is_Code; /* server code entry */
};

struct IntVectorV0
{
    APTR32        iv_Data;
    APTR32        iv_Code;
    APTR32        iv_Node;
};


/* Most fields are PRIVATE */
struct ExecBaseV0
{
/* Standard Library Structure */
    struct LibraryV0 LibNode;

/* System Constants */
    UWORD SoftVer;      /* OBSOLETE */
    WORD  LowMemChkSum;
    ULONG  ChkBase;
    APTR32 ColdCapture;
    APTR32 CoolCapture;
    APTR32 WarmCapture;
    APTR32 SysStkUpper;  /* System Stack Bounds */
    APTR32 SysStkLower;
    ULONG  MaxLocMem;    /* Top address of Chip memory + 1, or Chip RAM size. Amiga-specific */
    APTR32 DebugEntry;
    APTR32 DebugData;
    APTR32 AlertData;
    APTR32 MaxExtMem;    /* Top address of "Slow memory" + 1 (A500 only) */
    UWORD ChkSum;       /* SoftVer to MaxExtMem */

/* Interrupts */
    struct IntVectorV0 IntVects[16];

/* System Variables */
    APTR32       ThisTask;       /* Pointer to currently running task (readable) */
    ULONG        IdleCount;      /* Incremented when system goes idle            */
    ULONG        DispCount;      /* Incremented when a task is dispatched        */
    UWORD        Quantum;        /* # of ticks, a task may run                   */
    UWORD        Elapsed;        /* # of ticks, the current task has run         */
    UWORD        SysFlags;       /* Private flags                                */
    BYTE         IDNestCnt;      /* Disable() nesting count                      */
    BYTE         TDNestCnt;      /* Forbid() nesting count                       */
    UWORD        AttnFlags;      /* Attention Flags (readable, see below)        */
    UWORD        AttnResched;    /* Private scheduler flags                      */
    APTR32       ResModules;     /* Resident modules list                        */
    APTR32       TaskTrapCode;   /* Trap handling code                           */
    APTR32       TaskExceptCode; /* User-mode exception handling code            */
    APTR32       TaskExitCode;   /* Termination code                             */
    ULONG        TaskSigAlloc;   /* Allocated signals bitmask                    */
    UWORD        TaskTrapAlloc;  /* Allocated traps bitmask                      */

/* PRIVATE Lists */
    struct ListV0      MemList;
    struct ListV0      ResourceList;
    struct ListV0      DeviceList;
    struct ListV0      IntrList;
    struct ListV0      LibList;
    struct ListV0      PortList;
};

/* You must use Exec functions to modify task structure fields. */
struct TaskV0
{
    struct NodeV0 tc_Node;
    UBYTE	tc_Flags;
    UBYTE	tc_State;
    BYTE	tc_IDNestCnt;	/* Interrupt disabled nesting */
    BYTE	tc_TDNestCnt;	/* Task disabled nesting */
    ULONG	tc_SigAlloc;	/* Allocated signals */
    ULONG	tc_SigWait;	/* Signals we are waiting for */
    ULONG	tc_SigRecvd;	/* Received signals */
    ULONG	tc_SigExcept;	/* Signals we will take exceptions for */
    union
    {
	struct
	{
	    UWORD tc_ETrapAlloc;   /* Allocated traps */
	    UWORD tc_ETrapAble;    /* Enabled traps */
	} tc_ETrap;
	APTR32  tc_ETask;	   /* Valid if TF_ETASK is set */
    }		tc_UnionETask;
    APTR32	tc_ExceptData;	/* Exception data */
    APTR32	tc_ExceptCode;	/* Exception code */
    APTR32	tc_TrapData;	/* Trap data */
    APTR32	tc_TrapCode;	/* Trap code */
    APTR32	tc_SPReg;	/* Stack pointer */
    APTR32	tc_SPLower;	/* Stack lower bound */
    APTR32	tc_SPUpper;	/* Stack upper bound */
    APTR32  tc_Switch;   /* Task loses CPU */
    APTR32  tc_Launch;   /* Task gets CPU */
    struct ListV0 tc_MemEntry;	/* Allocated memory. Freed by RemTask(). */
    APTR32	tc_UserData;	/* For use by the task; no restrictions! */
};

/* MsgPort */
struct MsgPortV0
{
    struct NodeV0 mp_Node;
    UBYTE	mp_Flags;
    UBYTE	mp_SigBit;  /* Signal bit number */
    APTR32  mp_SigTask; /* Object to be signalled */
    struct ListV0 mp_MsgList; /* Linked list of messages */
};

/* Message */
struct MessageV0
{
    struct NodeV0    mn_Node;
    APTR32           mn_ReplyPort;  /* message reply port */
    UWORD	     mn_Length;     /* total message length, in bytes */
				    /* (include the size of the Message
				       structure in the length) */
};

struct IORequestV0
{
    struct MessageV0 io_Message;
    APTR32          io_Device;
    APTR32          io_Unit;
    UWORD           io_Command;
    UBYTE           io_Flags;
    BYTE            io_Error;
};

struct SemaphoreRequestV0
{
    struct MinNodeV0            sr_Link;
    APTR32                      sr_Waiter;
};


struct SignalSemaphoreV0
{
    struct NodeV0               ss_Link;
    WORD                        ss_NestCount;
    struct MinListV0            ss_WaitQueue;
    struct SemaphoreRequestV0   ss_MultipleLink;
    APTR32                      ss_Owner;
    WORD                        ss_QueueCount;
};

#endif
