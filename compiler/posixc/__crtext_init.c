/*
    Copyright (C) 2021, The AROS Development Team. All rights reserved.

    Internal initialisation code for crtext.library
*/

#include <aros/debug.h>

#include <proto/exec.h>
#include <aros/symbolsets.h>

#include "__crtext_intbase.h"

static int __crtext_open(struct CrtExtIntBase *CrtExtBase)
{
    D(bug("[crtext] %s(0x%p)\n", __func__, CrtExtBase));

    CrtExtBase->StdCBase    = AllocMem(sizeof(struct StdCIntBase), MEMF_PUBLIC | MEMF_CLEAR);
    CrtExtBase->PosixCBase  = AllocMem(sizeof(struct PosixCIntBase), MEMF_PUBLIC | MEMF_CLEAR);

    return 1;
}

static void __crtext_close(struct CrtExtIntBase *CrtExtBase)
{
    D(bug("[crtext] %s(0x%p)\n", __func__, CrtExtBase));

    FreeMem(CrtExtBase->StdCBase, sizeof(struct StdCIntBase));
    FreeMem(CrtExtBase->PosixCBase, sizeof(struct PosixCIntBase));
}

struct StdCBase * __aros_getbase_StdCBase()
{
    struct CrtExtIntBase *CrtExtBase = (struct CrtExtIntBase *)__aros_getbase_CrtExtBase();
    return (struct StdCBase *)CrtExtBase->StdCBase;
}

struct PosixCBase * __aros_getbase_PosixCBase()
{
    struct CrtExtIntBase *CrtExtBase = (struct CrtExtIntBase *)__aros_getbase_CrtExtBase();
    return (struct PosixCBase *)CrtExtBase->PosixCBase;
}

ADD2OPENLIB(__crtext_open, -50);
ADD2CLOSELIB(__crtext_close, -50);
