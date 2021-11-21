/*
    Copyright (C) 2012-2013, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>

int *__stdc_geterrnoptr(void) { asm("int3");return NULL; }

