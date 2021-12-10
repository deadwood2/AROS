#ifndef _LIBRARIES_CRTUTIL_H
#define _LIBRARIES_CRTUTIL_H

/*
    Copyright © 2021, The AROS Development Team. All rights reserved.
    $Id$

    Public part of CrtUtil libbase.
    Take care of backwards compatibility when changing something in this file.
*/

#include <exec/libraries.h>

struct CrtUtilBase
{
    struct Library lib;

    /* ctype.h */
    const unsigned short int * __ctype_b;
    const unsigned char * __ctype_toupper;
    const unsigned char * __ctype_tolower;
};

#endif /* _LIBRARIES_CRTUTIL_H */
