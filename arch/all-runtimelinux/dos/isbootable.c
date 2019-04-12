/*
    Copyright © 1995-2016, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Can this disk boot on your architecture?
    Lang: english
*/

#include <aros/debug.h>
#include <exec/alerts.h>
#include <proto/exec.h>
#include <proto/dos.h>

#include "dos_intern.h"

BOOL __dos_IsBootable(struct DosLibrary * DOSBase, BPTR lock)
{
    return TRUE;
}
