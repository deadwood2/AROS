/*
 * Copyright (C) 2012, The AROS Development Team.  All rights reserved.
 */

#ifndef SINGLEBASE_H
#define SINGLEBASE_H

#include <exec/libraries.h>

struct SingleBase
{
    struct Library lib;
    LONG value;
};

struct Library *__aros_getbase_SingleBase();

#endif /* SINGLEBASE_H */
