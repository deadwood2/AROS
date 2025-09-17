/*
    Copyright (C) 1995-2001, The AROS Development Team. All rights reserved.
*/

#include "__exitfunc.h"

/*****************************************************************************

    NAME */
#include <stdlib.h>

        int __progonly_atexit(
        
/*  SYNOPSIS */
        void (*func)(void))

/*  FUNCTION
        Registers the given function to be called at normal
        process termination.
        
    INPUTS
        func - function to be called.

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
        exit()

    INTERNALS

******************************************************************************/
{
    struct AtExitNode *aen = malloc(sizeof(*aen));

    if (!aen) return -1;

    aen->node.ln_Type = AEN_VOID;
    aen->func.fn = func;

    return __progonly_addexitfunc(aen);
}
