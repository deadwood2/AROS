/*
    Copyright © 2012-2013, The AROS Development Team. All rights reserved.
    $Id$

    Get the MB_CUR_MAX value
*/
#include <sys/arosc.h>

int __stdc_mb_cur_max(void)
{
    return __get_arosc_userdata()->acud_mb_cur_max;
}
