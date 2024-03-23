#ifndef ABIV0_EXEC_FUNCTIONS_H
#define ABIV0_EXEC_FUNCTIONS_H

#include <dos/bptr.h>

#include "./structures.h"

APTR abiv0_InitResident(struct ResidentV0 *resident, BPTR segList, struct ExecBaseV0 *SysBaseV0);
APTR abiv0_AllocMem(ULONG byteSize, ULONG requirements, struct ExecBaseV0 *SysBaseV0);
void abiv0_FreeMem(APTR memoryBlock, ULONG byteSize, struct ExecBaseV0 *SysBaseV0);
struct LibraryV0 * abiv0_MakeLibrary(APTR32 funcInit, APTR32 structInit, APTR32 libInit, ULONG dataSize,
    BPTR segList, struct ExecBaseV0 * SysBaseV0);
ULONG  abiv0_MakeFunctions(APTR target, APTR32 functionArray, APTR32 funcDispBase, struct ExecBaseV0 *SysBaseV0);
void abiv0_AddLibrary(struct LibraryV0 *library, struct ExecBaseV0 *SysBaseV0);
void abiv0_Enqueue(struct ListV0 *list, struct NodeV0 *node, struct ExecBaseV0 *SysBaseV0);
struct NodeV0 * abiv0_FindName(struct ListV0 *list, CONST_STRPTR name, struct ExecBaseV0 *SysBaseV0);
struct LibraryV0 * abiv0_OpenLibrary(CONST_STRPTR libName, ULONG version, struct ExecBaseV0 *SysBaseV0);

#endif
