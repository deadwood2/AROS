#ifndef _BSDSOCKET_LIBDEFS_H
#define _BSDSOCKET_LIBDEFS_H

#include <exec/types.h>

#define GM_UNIQUENAME(n) BSDSocket_ ## n
#define LIBBASE          SocketBase
#define LIBBASETYPE      struct SocketBase
#define LIBBASETYPEPTR   struct SocketBase *
#define MOD_NAME_STRING  "bsdsocket.library"
#define MOD_DATE_STRING  "4.1.2024"
#define MOD_VERS_STRING  "4.60"
#define VERSION_NUMBER   4
#define MAJOR_VERSION    4
#define REVISION_NUMBER  60
#define MINOR_VERSION    60
#define VERSION_STRING   "$VER: bsdsocket.library 4.60 (4.1.2024)\r\n"
#define COPYRIGHT_STRING ""
#define LIBEND           GM_UNIQUENAME(End)
#define LIBFUNCTABLE     GM_UNIQUENAME(FuncTable)
#define RESIDENTPRI      0
#define RESIDENTFLAGS    RTF_COLDSTART|RTF_AUTOINIT
#define FUNCTIONS_COUNT  50
#include "socketbase.h"

#endif /* _BSDSOCKET_LIBDEFS_H */
