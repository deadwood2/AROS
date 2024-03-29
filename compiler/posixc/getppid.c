/*
    Copyright (C) 2004-2013, The AROS Development Team. All rights reserved.
*/

#include <sys/types.h>

#include <exec/tasks.h>
#include <proto/exec.h>

#include <assert.h>

#include "__posixc_intbase.h"
#include "__vfork.h"

/*****************************************************************************

    NAME */
#include <unistd.h>

        pid_t getppid(

/*  SYNOPSIS */
        void)

/*  FUNCTION
        Returns the Parent process ID of the calling processes.

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
  struct CrtProgCtx *ProgCtx = __aros_get_ProgCtx();
  struct Task *ParentTask;
  struct ETask *eThisTask;
  struct ETask *et;

  if(ProgCtx->vforkflags & PRETEND_CHILD)
  {
    struct vfork_data *udata = ProgCtx->vfork_data;
    eThisTask = GetETask(udata->child);
  }
  else
    eThisTask = GetETask(FindTask(NULL));
  assert(eThisTask);
  ParentTask = (struct Task *)eThisTask->et_Parent;
  if(!ParentTask)
    return (pid_t) 1;
  et = GetETask(ParentTask);
  assert(et);
  return (pid_t) et->et_UniqueID;
}

