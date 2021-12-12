/*
    Copyright (C) 2021, The AROS Development Team. All rights reserved.

*/

#include <proto/crt.h>

void __assert(const char * expr, const char * file, unsigned int line)
{
    __modonly_assert(expr, file, line);
}
