/*
    Copyright (C) 2021, The AROS Development Team. All rights reserved.

*/

#include <proto/crt.h>

void abort (void)
{
    __modonly_abort();
}
