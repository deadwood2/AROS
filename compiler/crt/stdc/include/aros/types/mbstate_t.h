#ifndef _AROS_TYPES_MBSTATE_T_H
#define _AROS_TYPES_MBSTATE_T_H

/*
    Copyright © 2010-2025, The AROS Development Team. All rights reserved.
    $Id$
*/

typedef int mbstate_t;

#if 0
#include <aros/types/wchar_t.h>

/*
 * mbstate_t: Conversion state object for multibyte to wide character conversions.
 * Internal fields used by the runtime to track conversion state.
 */
typedef struct {
    int           __state;   /* internal use */
    unsigned int  __count;
    wchar_t       __value;
} mbstate_t;

#endif

#endif /* _AROS_TYPES_MBSTATE_T_H */
