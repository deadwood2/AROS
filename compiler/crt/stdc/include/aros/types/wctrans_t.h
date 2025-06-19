#ifndef _AROS_TYPES_WCTRANS_T_H
#define _AROS_TYPES_WCTRANS_T_H

/*
    Copyright © 2020-2025, The AROS Development Team. All rights reserved.
    $Id$
*/

#ifndef __cplusplus

/* typedef int wctrans_t; */

#endif

#if (0)

#include <aros/types/wint_t.h>

struct __wctrans {
    const char *name;
    wint_t (*func)(wint_t);
};

#endif

#endif /* _AROS_TYPES_WCTRANS_T_H */
