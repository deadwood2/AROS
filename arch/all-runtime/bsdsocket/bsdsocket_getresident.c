#include <proto/exec.h>
#include "bsdsocket_libdefs.h"

#undef SysBase
extern struct Resident GM_UNIQUENAME(ROMTag);
__attribute__((visibility("default"))) APTR __get_resident()
{
    return &GM_UNIQUENAME(ROMTag);
}
