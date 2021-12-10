/*
    Copyright (C) 2021, The AROS Development Team. All rights reserved.

    Internal initialisation code for crtext.library
*/

#include <aros/debug.h>

#include <proto/exec.h>
#include <aros/symbolsets.h>

#include "__crtext_intbase.h"

LONG ProgCtxSlot = -1;

int __crtext_open(struct CrtExtIntBase *CrtExtBase)
{
    D(bug("[crtext] %s(0x%p)\n", __func__, CrtExtBase));

    if (ProgCtxSlot == -1)
        ProgCtxSlot = AllocTaskStorageSlot();

    CrtExtBase->StdCBase    = AllocMem(sizeof(struct StdCIntBase), MEMF_PUBLIC | MEMF_CLEAR);
    CrtExtBase->PosixCBase  = AllocMem(sizeof(struct PosixCIntBase), MEMF_PUBLIC | MEMF_CLEAR);
    CrtExtBase->PosixCBase->internalpool = CreatePool(MEMF_PUBLIC|MEMF_CLEAR, 256, 256);
    CrtExtBase->fakevforkbase = NULL;

    return 1;
}

void __crtext_close(struct CrtExtIntBase *CrtExtBase)
{
    D(bug("[crtext] %s(0x%p)\n", __func__, CrtExtBase));

    DeletePool(CrtExtBase->PosixCBase->internalpool);
    FreeMem(CrtExtBase->PosixCBase, sizeof(struct PosixCIntBase));
    FreeMem(CrtExtBase->StdCBase, sizeof(struct StdCIntBase));

}

void __aros_setoffsettable(void *base);

void __aros_setbase_CrtExtBase(struct CrtExtIntBase *CrtExtBase)
{
    __aros_setoffsettable(CrtExtBase);
}

void __aros_setbase_fake_CrtExtBase(struct CrtExtIntBase *fCrtExtBase)
{
    struct CrtExtIntBase *CrtExtBase = (struct CrtExtIntBase *)__aros_getbase_CrtExtBase();
    CrtExtBase->fakevforkbase = fCrtExtBase;
}

struct StdCBase * __aros_getbase_StdCBase()
{
    struct CrtExtIntBase *CrtExtBase = (struct CrtExtIntBase *)__aros_getbase_CrtExtBase();

    return (struct StdCBase *)CrtExtBase->StdCBase;
}

struct PosixCBase * __aros_getbase_PosixCBase()
{
    struct CrtExtIntBase *CrtExtBase = (struct CrtExtIntBase *)__aros_getbase_CrtExtBase();

    if (CrtExtBase->fakevforkbase)
        return (struct PosixCBase *)CrtExtBase->fakevforkbase->PosixCBase;
    else
        return (struct PosixCBase *)CrtExtBase->PosixCBase;
}

struct CrtExtProgCtx * __aros_get_ProgCtx()
{
    return (struct CrtExtProgCtx *)GetTaskStorageSlot(ProgCtxSlot);
}

struct CrtExtProgCtx * __aros_create_ProgCtx()
{
    struct CrtExtProgCtx *ProgCtx = AllocMem(sizeof(struct CrtExtProgCtx), MEMF_PUBLIC | MEMF_CLEAR);
    __progonly_init_atexit(ProgCtx);
    ProgCtx->libbase = (struct CrtExtIntBase *)__aros_getbase_CrtExtBase();

    SetTaskStorageSlot(ProgCtxSlot, (IPTR)ProgCtx);
    return __aros_get_ProgCtx();
}

void __aros_delete_ProgCtx()
{
    struct CrtExtProgCtx *ProgCtx = __aros_get_ProgCtx();
    FreeMem(ProgCtx, sizeof(struct CrtExtProgCtx));
    SetTaskStorageSlot(ProgCtxSlot, (IPTR)NULL);
}

struct CrtExtProgCtx * __aros_get_Parent_ProgCtx()
{
    return (struct CrtExtProgCtx *)GetParentTaskStorageSlot(ProgCtxSlot);
}

ADD2OPENLIB(__crtext_open, -101);
ADD2CLOSELIB(__crtext_close, -101);
