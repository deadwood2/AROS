/*
    Copyright (C) 2025-2026, The AROS Development Team. All rights reserved.
*/

#ifndef ABIV0_EXEC_FUNCTIONS_H
#define ABIV0_EXEC_FUNCTIONS_H

#include <dos/bptr.h>

#include "./structures.h"

/* These functions can be called only in 64-bit mode but with either 64-bit or 32-bit stack */
APTR abiv0_InitResident(struct ResidentV0 *resident, BPTR segList, struct ExecBaseV0 *SysBaseV0);
struct LibraryV0 * abiv0_MakeLibrary(APTR32 funcInit, APTR32 structInit, APTR32 libInit, ULONG dataSize,
    BPTR segList, struct ExecBaseV0 * SysBaseV0);
ULONG  abiv0_MakeFunctions(APTR target, APTR32 functionArray, APTR32 funcDispBase, struct ExecBaseV0 *SysBaseV0);
void abiv0_AddLibrary(struct LibraryV0 *library, struct ExecBaseV0 *SysBaseV0);
void abiv0_Enqueue(struct ListV0 *list, struct NodeV0 *node, struct ExecBaseV0 *SysBaseV0);
struct NodeV0 * abiv0_FindName(struct ListV0 *list, CONST_STRPTR name, struct ExecBaseV0 *SysBaseV0);
struct LibraryV0 * abiv0_OpenLibrary(CONST_STRPTR libName, ULONG version, struct ExecBaseV0 *SysBaseV0);
void abiv0_InitSemaphore(struct SignalSemaphoreV0 *sigSem, struct ExecBaseV0 *SysBaseV0);
IPTR abiv0_NewStackSwap(struct StackSwapStructV0 *sss, LONG_FUNC entry, struct StackSwapArgsV0 *args, struct ExecBaseV0 *SysBaseV0);

APTR abiv0_AllocMem(ULONG byteSize, ULONG requirements, struct ExecBaseV0 *SysBaseV0);
void abiv0_FreeMem(APTR memoryBlock, ULONG byteSize, struct ExecBaseV0 *SysBaseV0);
APTR abiv0_CreatePool(ULONG requirements, ULONG puddleSize, ULONG threshSize, struct ExecBaseV0 *SysBaseV0);
void abiv0_DeletePool(APTR poolHeader, struct ExecBaseV0 *SysBaseV0);
APTR abiv0_AllocPooled(APTR poolHeader, ULONG memSize, struct ExecBaseV0 *SysBaseV0);
void abiv0_FreePooled(APTR poolHeader, APTR memory, ULONG memSize, struct ExecBaseV0 *SysBaseV0);

#endif
