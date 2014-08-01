#ifndef _LIBRARIES_STDLIB_H
#define _LIBRARIES_STDLIB_H

/*
    Copyright © 2021, The AROS Development Team. All rights reserved.
    $Id$

    Public part of Stdlib libbase.
    Take care of backwards compatibility when changing something in this file.
*/

#include <exec/libraries.h>

struct StdlibBase
{
    struct Library lib;

    /* ctype.h */
    const unsigned short int * __ctype_b;
    const int * __ctype_toupper;
    const int * __ctype_tolower;
};

#endif /* _LIBRARIES_STDLIB_H */
