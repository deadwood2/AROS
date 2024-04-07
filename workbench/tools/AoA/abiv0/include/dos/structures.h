#ifndef ABIV0_DOS_STRUCTURES_H
#define ABIV0_DOS_STRUCTURES_H

#include "../exec/structures.h"

typedef ULONG   BPTR32;
typedef APTR32  BSTR32;

/* Structure used for CLIs and Shells. Allocate this structure with
   AllocDosObject() only! */
struct CommandLineInterfaceV0
{
      /* Secondary error code, set by last command. */
    LONG cli_Result2;
      /* Name of the current directory. */
    BSTR32 cli_SetName;
      /* Lock of the first directory in path. (struct FileLock *) */
    BPTR32 cli_CommandDir;
      /* Error code, the last command returned. See <dos/dos.h> for
         definitions. */
    LONG cli_ReturnCode;
      /* Name of the command that is currently executed. */
    BSTR32 cli_CommandName;
      /* Fail-Level as set by the command "FailAt". */
    LONG cli_FailLevel;
      /* Current prompt in the CLI window. */
    BSTR32 cli_Prompt;
      /* Standard/Default input file. (struct FileLock *) */
    BPTR32 cli_StandardInput;
      /* Current input file. (struct FileLock *) */
    BPTR32 cli_CurrentInput;
      /* Name of the file that is currently executed. */
    BSTR32 cli_CommandFile;
      /* TRUE if the currently CLI is connected to a controlling terminal,
         otherwise FALSE. */
    LONG cli_Interactive;
      /* FALSE if there is no controlling terminal, otherwise TRUE. */
    LONG cli_Background;
      /* Current output file. (struct FileLock *) */
    BPTR32 cli_CurrentOutput;
      /* Default stack size as set by the command "Stack". */
    LONG cli_DefaultStack;
      /* Standard/Default output file. (struct FileLock *) */
    BPTR32 cli_StandardOutput;
      /* SegList of currently loaded command. */
    BPTR32 cli_Module;

       /* Here begins the aros specific part */
      /* Standard/Default Error file. (struct FileLock *) */
    BPTR32 cli_StandardError;
};

/* Standard process structure. Processes are just extended tasks. */
struct ProcessV0
{
      /* Embedded task structure as defined in <exec/tasks.h>. */
    struct TaskV0 pr_Task;

      /* Processes standard message-port. Used for various puposes. */
    struct MsgPortV0  pr_MsgPort;
    WORD	    pr_Pad;     /* PRIVATE */
      /* SegList array, used by this process. (void **) */
    BPTR32      pr_SegList;
      /* StackSize of the current process. */
    LONG	    pr_StackSize;
    APTR32      pr_GlobVec;
      /* CLI process number. This may be 0, in which case the process is not
         connected to a CLI. */
    LONG	    pr_TaskNum;
      /* Pointer to upper end of stack. (void *) */
    BPTR32      pr_StackBase;
      /* Secondary return-value, as defined in <dos/dos.h>. As of now this
         field is declared PRIVATE. Use IoErr()/SetIoErr() to access it. */
    LONG        pr_Result2;
      /* Lock of the current directory. As of now this is declared READ-ONLY.
         Use CurrentDir() to set it. (struct FileLock *) */
    BPTR32      pr_CurrentDir;
      /* Standard input file. As of now this is declared WRITE-ONLY. Use
         Input() to query it. */
    BPTR32      pr_CIS;
      /* Standard output file. As of now this is declared WRITE-ONLY. Use
         Output() to query it. */
    BPTR32      pr_COS;
      /* Task to handle the console associated with process. */
    APTR32      pr_ConsoleTask;
      /* The task that is responsible for handling the filesystem. */
    APTR32      pr_FileSystemTask;
      /* CLI the process is connected to. (struct CommandLineInterface *) */
    BPTR32      pr_CLI;
    APTR32      pr_ReturnAddr;
      /* Function to be called, when process waits for a packet-message. */
    APTR32      pr_PktWait;
      /* Standard-Window of process. */
    APTR32      pr_WindowPtr;
      /* Lock to home-directory of process. (struct FileLock *) */
    BPTR32      pr_HomeDir;
    LONG	    pr_Flags; /* see below */

      /* Code that is called, when the process exits. pr_ExitData takes an
         argument to be passed to this code. */
    APTR32     pr_ExitCode;
    ULONG      pr_ExitData;
      /* Arguments passed to the process from caller. */
    APTR32     pr_Arguments;

      /* List of local environment variables. This list should be in
         alphabetical order. Multiple entries may have the same name, if they
         are of different types. See <dos/var.h> for more information. */
    struct MinListV0 pr_LocalVars;
    ULONG	   pr_ShellPrivate;
      /* Standard error file. May be NULL, in which case pr_COS is to be used.
         Use this instead of Output() to report errors. */
    BPTR32   pr_CES;
};

/* Devices process structure as returned by GetDeviceProc(). */
struct DevProcV0
{
    APTR32 dvp_Port;
    BPTR32	     dvp_Lock;    /* struct FileLock * */
    ULONG	     dvp_Flags;   /* see below */
    APTR32 dvp_DevNode; /* PRIVATE */
};

struct FileInfoBlockV0
{
    ULONG	     fib_DiskKey;
      /* See <dos/dosextens.h> for definitions. Generally: if this is >= 0
         the file described is a directory, otherwise it is a plain file. */
    LONG	     fib_DirEntryType;
      /* The filename.
       *
       *       User applications should always treat this as ASCIIZ.
       *
       * NOTE: This is created as a BCPL string in the ACTION_EXAMINE_*
       *       filesystem handler code, but is converted to a C style
       *       '\0'-terminated string by the Dos/Examine???() functions.
       *
       */
    UBYTE	     fib_FileName[MAXFILENAMELENGTH];
    LONG	     fib_Protection;   /* The protection bits (see below). */
    LONG	     fib_EntryType;
    LONG	     fib_Size;         /* The size of the file. */
    LONG	     fib_NumBlocks;    /* Number of blocks used for file. */
    struct DateStamp fib_Date;         /* Date of last change to file. */
    /* The filecomment. Follows the same BSTR/CSTR rules as fib_FileName */
    UBYTE	     fib_Comment[MAXCOMMENTLENGTH];
    UWORD	     fib_OwnerUID;     /* UserID of fileowner. */
    UWORD	     fib_OwnerGID;     /* GroupID of fileowner. */
    UBYTE	     fib_Reserved[32]; /* PRIVATE */
};

/* Structure as used for controlling ExAll(). Allocate this structure by using
   AllocDosObject(DOS_EXALLCONTROL,...) only. All fields must be initialized
   to 0, before using this structure. (AllocDosObject() does that for you.)
   After calling ExAll() the first time, this structure is READ-ONLY. */
struct ExAllControlV0
{
      /* The number of entries that were returned in the buffer. */
    ULONG         eac_Entries;
    ULONG         eac_LastKey;     /* PRIVATE */
      /* Parsed pattern string, as created by ParsePattern(). This may be NULL.
      */
    APTR32        eac_MatchString;
      /* You may supply a hook, which is called for each entry. This hook
         should return TRUE, if the current entry is to be included in
         the file list and FALSE, if it should be ignored. */
    APTR32        eac_MatchFunc;
};

/* Structure (as used in ExAll()), containing information about a file. This
   structure is only as long as it need to be. If is for example ED_SIZE was
   specified, when calling ExAll(), this structure only consists of the fields
   ed_Name through ed_Size. Therefore you can use the ED_ definitions below
   as longword offsets into this structure. */

struct ExAllDataV0
{
    APTR32 ed_Next;

    APTR32 ed_Name;     /* Name of the file. */
    LONG    ed_Type;     /* Type of file. See <dos/dosextens.h>. */
    ULONG   ed_Size;     /* Size of file. */
    ULONG   ed_Prot;     /* Protection bits. */

    /* The following three fields are de facto an embedded datestamp
       structure (see <dos/dos.h>), which describes the last modification
       date. */
    ULONG ed_Days;
    ULONG ed_Mins;
    ULONG ed_Ticks;

    APTR32 ed_Comment;  /* The file comment. */

    UWORD ed_OwnerUID; /* The owner ID. */
    UWORD ed_OwnerGID; /* The group-owner ID. */
};

/* This structure is returned by LockDosList() and similar calls. This
 * structure is identical to the AmigaOS one, but this structure is PRIVATE
 * anyway. Use system-calls for dos list-handling.
 */
struct DosListV0
{
      /* PRIVATE pointer to next entry. */
    BPTR32 dol_Next;
      /* Type of the current node (see below). */
    LONG             dol_Type;
      /* Filesystem task handling this entry (for old-style filesystems) */
    APTR32 dol_Task;
      /* The lock passed to AssignLock(). Only set if the type is
         DLT_DIRECTORY. */
    BPTR32           dol_Lock;

      /* This union combines all the different types. */
    union {
          /* See struct DevInfo below. */
        struct {
            BSTR32    dol_Handler;
            LONG    dol_StackSize;
            LONG    dol_Priority;
            BPTR32    dol_Startup;
            BPTR32    dol_SegList;
            BPTR32    dol_GlobVec;
        } dol_handler;
          /* See struct DeviceList below. */
        struct {
            struct DateStamp dol_VolumeDate;
            BPTR32             dol_LockList;
            LONG             dol_DiskType;
            BPTR32             dol_unused;
        } dol_volume;
          /* Structure used for assigns. */
        struct {
              /* The name for the late or nonbinding assign. */
            APTR32 dol_AssignName;
              /* A list of locks, used by AssignAdd(). */
            APTR32 dol_List;
        } dol_assign;
    } dol_misc;

    /* Name as a BCPL string */
    BSTR32 dol_Name;
};

struct DosLibraryV0
{
    /* A normal library-base as defined in <exec/libraries.h>. */
    struct LibraryV0 dl_lib;

    APTR32 dl_Root;

    /* private BCPL fields. Do not use. */
    APTR32  dl_GV;
    APTR32  dl_A2;
    APTR32  dl_A5;
    APTR32  dl_A6;

    /* The following fields are PRIVATE! */
    APTR32 dl_Errors;
    APTR32 dl_TimeReq;
    APTR32 dl_UtilityBase;
    APTR32 dl_IntuitionBase;
};
#endif
