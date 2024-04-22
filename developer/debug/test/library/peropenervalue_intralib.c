/*
    Copyright (C) 2024, The AROS Development Team. All rights reserved.
*/

#include <proto/peropener.h>

/* This file contains logic for testing library calls from withint itself */

void PeropenerSetValueStackThroughInternalReg(int value)
{
    struct Library *PeropenerBase = __aros_getbase_PeropenerBase();

    PeropenerSetValueReg(value);
}
