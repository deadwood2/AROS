/*
    Copyright (C) 2021, The AROS Development Team. All rights reserved.

*/

#include <proto/crt.h>

void exit (int code)
{
    __modonly_exit(code);
}
