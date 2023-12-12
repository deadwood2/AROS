/*
    Copyright © 2012-2013, The AROS Development Team. All rights reserved.
    $Id$

*/
#include <sys/arosc.h>

int *___geterrnoptr(void)
{
    return &(__get_arosc_userdata()->acud_errno);
}
