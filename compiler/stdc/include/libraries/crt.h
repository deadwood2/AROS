#ifndef _LIBRARIES_CRT_H
#define _LIBRARIES_CRT_H

/*
    Copyright © 2021, The AROS Development Team. All rights reserved.
    $Id$

    Public part of Crt libbase.
    Take care of backwards compatibility when changing something in this file.
*/

#include <exec/libraries.h>

struct CrtBase
{
    struct Library lib;

    /* ctype.h */
    const unsigned short int * __ctype_b;
    const unsigned char * __ctype_toupper;
    const unsigned char * __ctype_tolower;
};

#endif /* _LIBRARIES_CRT_H */
