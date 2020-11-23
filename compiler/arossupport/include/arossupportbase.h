#ifndef AROS_AROSSUPPORTBASE_H
#define AROS_AROSSUPPORTBASE_H

#ifndef EXEC_LISTS_H
#include <exec/lists.h>
#endif

/*
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <stdarg.h>

struct AROSSupportBase
{
    IPTR    	    _pad;
    int     	    (*kprintf)(const char *, ...);
    int     	    (*rkprintf)(const char *, const char *, int, const char *, ...);
    int     	    (*vkprintf)(const char *, va_list);
};

#endif /* AROS_AROSSUPPORTBASE_H */
