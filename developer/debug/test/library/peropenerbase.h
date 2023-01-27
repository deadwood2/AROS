#include <dos/dos.h>
#include <exec/libraries.h>

struct PeropenerBase
{
    struct Library lib;
    int value;
    BPTR extlibseg;
    APTR jumptable;
};
