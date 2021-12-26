/*
    Copyright (C) 1995-2014, The AROS Development Team. All rights reserved.
*/

/*
 * This function is in the static link lib.
 * It uses the genmodule provided __aros_getbase_PeropenerBase() function so it can be
 * used both in a library that uses peropener_rel.a or a program that just uses
 * peropener.a.
 * It does not call a function in peropener.library so that a good optimizing
 * compiler with link time function inlining can optimize this well.
 */
#include "peropenerbase.h"

struct Library *__aros_getbase_PeropenerBase();

int *__peropener_getvalueptr(void)
{
    return &(((struct PeropenerBase *)__aros_getbase_PeropenerBase())->value);
}
