/*
    Copyright (C) 2023, The AROS Development Team. All rights reserved.

    crt.library 1.0 backwards compatibility
*/

#include <errno.h>
#include <stdio.h>
#include <stdint.h>

int __fgetpos_fpos_t_32(FILE *stream, uint32_t *pos)
{
    if ( pos == NULL )
    {
        errno = EINVAL;
        return -1;
    }

    *pos = (uint32_t)ftell (stream);

    if ( *pos < 0L )
    {
        return -1;
    }

    return 0;
}

int __fsetpos_fpos_t_32(FILE *stream, const uint32_t *pos)
{
    if ( pos == NULL )
    {
        errno = EINVAL;
        return -1;
    }

    return fseek (stream, *pos, SEEK_SET);
}

