/*
    Copyright (C) 2012, The AROS Development Team. All rights reserved.

    Get pointer to standard IO streams.
    These function is both in static linklib as in posixc.library.
*/

FILE *__stdio_getstdin(void)
{
    return __aros_getbase_PosixCBase()->_stdin;
}

FILE *__stdio_getstdout(void)
{
    return __aros_getbase_PosixCBase()->_stdout;
}

FILE *__stdio_getstderr(void)
{
    return __aros_getbase_PosixCBase()->_stderr;
}
