/*
    Copyright (C) 1995-2013, The AROS Development Team. All rights reserved.

    POSIX.1-2008 function getpid().
*/

#include <proto/exec.h>
#include <assert.h>

#include "__posixc_intbase.h"
#include "__vfork.h"

/*****************************************************************************

    NAME */
#include <sys/types.h>
#include <unistd.h>

        pid_t getpid (

/*  SYNOPSIS */
        )

/*  FUNCTION
        Returns the process ID of the calling process

    RESULT
        The process ID of the calling process.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
    struct CrtProgCtx *ProgCtx = __aros_get_ProgCtx();
    struct ETask *et;

    if(ProgCtx && ProgCtx->vforkflags & PRETEND_CHILD)
    {
        struct vfork_data *udata = ProgCtx->vfork_data;
        et = GetETask(udata->child);
    }
    else
        et = GetETask(FindTask(NULL));
    assert(et);
    return (pid_t) et->et_UniqueID;
} /* getpid */
