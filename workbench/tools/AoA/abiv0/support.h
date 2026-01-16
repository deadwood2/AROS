/*
    Copyright (C) 2026, The AROS Development Team. All rights reserved.
*/

#ifndef _SUPPORT_H
#define _SUPPORT_H

#include <exec/types.h>

void unhandledCodePath(CONST_STRPTR function, STRPTR codepath, ULONG valDec, ULONG valHex);
void unhandledLibraryFunction(STRPTR libname, ULONG lvo);

#endif
