#ifndef ABIV0_DOS_STRUCTURES_H
#define ABIV0_DOS_STRUCTURES_H

#include "../exec/structures.h"

typedef ULONG BPTR32;

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

struct DosLibraryV0
{
    /* A normal library-base as defined in <exec/libraries.h>. */
    struct LibraryV0 dl_lib;
};

#endif