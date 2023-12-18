#ifndef _AROSC_LIBDEFS_H
#define _AROSC_LIBDEFS_H

#include <exec/types.h>

#define GM_UNIQUENAME(n) Arosc_ ## n
#define LIBBASE          aroscbase
#define LIBBASETYPE      struct aroscbase
#define LIBBASETYPEPTR   struct aroscbase *
#define MOD_NAME_STRING  "arosc.library"
#define MOD_DATE_STRING  "18.12.2023"
#define MOD_VERS_STRING  "44.0"
#define VERSION_NUMBER   44
#define MAJOR_VERSION    44
#define REVISION_NUMBER  0
#define MINOR_VERSION    0
#define VERSION_STRING   "$VER: arosc.library 44.0 (18.12.2023)\r\n"
#define COPYRIGHT_STRING ""
#define LIBEND           GM_UNIQUENAME(End)
#define LIBFUNCTABLE     GM_UNIQUENAME(FuncTable)
#define RESIDENTPRI      0
#define RESIDENTFLAGS    RTF_COLDSTART|RTF_AUTOINIT
#define FUNCTIONS_COUNT  310
#include "arosc_gcc.h"

/* Thus must be externally visible for stackcall libs */
char *__aros_getoffsettable(void);


LIBBASETYPEPTR __GM_GetBaseParent(LIBBASETYPEPTR);

#endif /* _AROSC_LIBDEFS_H */
