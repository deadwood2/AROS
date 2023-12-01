#include <dos/dos.h>
#include <exec/libraries.h>

struct UseRelBase
{
    struct Library  lib;
    struct Library  *peropenerBase;
    BOOL            peropenerBaseClose;
};
