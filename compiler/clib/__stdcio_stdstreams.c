/*
    Copyright © 2012-2013, The AROS Development Team. All rights reserved.
    $Id$

    Get pointer to standard IO streams.
*/
#include <stdio.h>

#include <sys/arosc.h>

FILE *__stdio_getstdin(void)
{
    return __get_arosc_userdata()->acud_stdin;
}

FILE *__stdio_getstdout(void)
{
    return __get_arosc_userdata()->acud_stdout;
}

FILE *__stdio_getstderr(void)
{
    return __get_arosc_userdata()->acud_stderr;
}
