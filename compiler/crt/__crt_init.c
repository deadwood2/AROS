/*
    Copyright (C) 2021, The AROS Development Team. All rights reserved.

    Internal initialisation code for crt.library
*/

#include <aros/debug.h>

#include <proto/exec.h>
#include <aros/symbolsets.h>

#include "__crt_intbase.h"
#include "__crt_progonly.h"

static LONG ProgCtxSlot = -1;

void __aros_setbase_fake_CrtBase(struct CrtIntBase *fCrtBase)
{
    struct CrtIntBase *CrtBase = (struct CrtIntBase *)__aros_getbase_CrtBase();
    CrtBase->fakevforkbase = fCrtBase;
}

struct StdCBase * __aros_getbase_StdCBase()
{
    struct CrtIntBase *CrtBase = (struct CrtIntBase *)__aros_getbase_CrtBase();

    return (struct StdCBase *)CrtBase->StdCBase;
}

struct PosixCBase * __aros_getbase_PosixCBase()
{
    struct CrtIntBase *CrtBase = (struct CrtIntBase *)__aros_getbase_CrtBase();

    if (CrtBase->fakevforkbase)
        return (struct PosixCBase *)CrtBase->fakevforkbase->PosixCBase;
    else
        return (struct PosixCBase *)CrtBase->PosixCBase;
}

struct CrtProgCtx * __aros_get_ProgCtx()
{
    return (struct CrtProgCtx *)GetTaskStorageSlot(ProgCtxSlot);
}

struct CrtProgCtx * __aros_create_ProgCtx()
{
    struct CrtProgCtx *ProgCtx = AllocMem(sizeof(struct CrtProgCtx), MEMF_PUBLIC | MEMF_CLEAR);
    __progonly_init_atexit(ProgCtx);
    ProgCtx->libbase = (struct CrtIntBase *)__aros_getbase_CrtBase();

    SetTaskStorageSlot(ProgCtxSlot, (IPTR)ProgCtx);
    return __aros_get_ProgCtx();
}

void __aros_delete_ProgCtx()
{
    struct CrtProgCtx *ProgCtx = __aros_get_ProgCtx();
    FreeMem(ProgCtx, sizeof(struct CrtProgCtx));
    SetTaskStorageSlot(ProgCtxSlot, (IPTR)NULL);
}

static int __crtext_init(struct CrtIntBase *CrtBase)
{
    ProgCtxSlot = AllocTaskStorageSlot();
}

int __crtext_open(struct CrtIntBase *CrtBase)
{
    D(bug("[crtext] %s(0x%p)\n", __func__, CrtBase));

    CrtBase->StdCBase    = AllocMem(sizeof(struct StdCIntBase), MEMF_PUBLIC | MEMF_CLEAR);
    CrtBase->StdCBase->lib.mb_cur_max = 1;
    CrtBase->PosixCBase  = AllocMem(sizeof(struct PosixCIntBase), MEMF_PUBLIC | MEMF_CLEAR);
    CrtBase->PosixCBase->internalpool = CreatePool(MEMF_PUBLIC|MEMF_CLEAR|MEMF_SEM_PROTECTED, 256, 256);
    CrtBase->PosixCBase->fd_array = AllocPooled(CrtBase->PosixCBase->internalpool, 16 * sizeof(APTR));
    InitSemaphore(&CrtBase->PosixCBase->fd_sem);
    CrtBase->fakevforkbase = NULL;

    return 1;
}

void __crtext_close(struct CrtIntBase *CrtBase)
{
    D(bug("[crtext] %s(0x%p)\n", __func__, CrtBase));

    DeletePool(CrtBase->PosixCBase->internalpool);
    FreeMem(CrtBase->PosixCBase, sizeof(struct PosixCIntBase));
    FreeMem(CrtBase->StdCBase, sizeof(struct StdCIntBase));

}

ADD2INITLIB(__crtext_init, 0);
ADD2OPENLIB(__crtext_open, -101);
ADD2CLOSELIB(__crtext_close, -101);
