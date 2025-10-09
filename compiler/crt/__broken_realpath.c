/*
    Copyright (C) 1995-2025, The AROS Development Team. All rights reserved.
*/

/*
    realpath function was not exposed in stdlib.h but was present in libcrt.a. This means it could have
    been used for software, however since there was not declaration, compiler was assuming it is returning
    'int'. This means a new implemented 'realpath' function needs to be moved to another LVO as there is
    code now is binaries which assumes 'int' and expands it to full pointer on client side, destroying the
    value of correct pointer.
*/

int __broken_realpath(const char *path, char *resolved_path)
{
    return 0;
}
