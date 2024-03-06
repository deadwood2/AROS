#include <dos/bptr.h>
#include <exec/types.h>
#include <proto/dos.h>


BPTR LoadSeg32 (CONST_STRPTR name, struct DosLibrary *DOSBase);

int main()
{
    BPTR seg = LoadSeg32("SYS:Calculator32", DOSBase);
    return 0;
}

