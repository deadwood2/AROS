#ifndef AROSC_BUILD
#ifndef STDC_DEBUG_H
#define STDC_DEBUG_H

#ifndef STDCNAME
#if defined(STDC_IO)
#define STDCNAME        "STDCIO"
#else
#if defined(STDC_STATIC)
#define STDCNAME        "STDC:lib"
#else
#define STDCNAME        "STDC:Shared"
#endif
#endif
#endif

#endif
#else
#include "internal/debug.h"
#endif
