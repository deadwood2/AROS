/*
    Copyright (C) 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/userel.h>
#include <proto/peropener.h>

#include "peropenervalue.h"

int main (int argc, char ** argv)
{
    /* Test forwarding to single.library */
    const int e1 = 11;
    int r1 = UseRelAdd2(8, 1, 2);
    Printf((STRPTR)"UseRelAdd2: %ld == %ld %s\n", e1, r1, (r1 == e1) ? "OK" : "FAIL!");

    const int e2 = 23;
    int r2 = UseRelAdd5(8, 1, 2, 3, 4, 5);
    Printf((STRPTR)"UseRelAdd5: %ld == %ld %s\n", e2, r2, (r2 == e2) ? "OK" : "FAIL!");

    /* Test operating on a "global" variable exposed from peropener.library */
    const int e3 = 2;
    PeropenerSetValueStack(e3);
    int r3 = PeropenerGetValueStack();
    Printf((STRPTR)"PeropenerGetValueStack: %ld == %ld %s\n", e3, r3, (r3 == e3) ? "OK" : "FAIL!");
    Printf((STRPTR)"peropenervalue: %ld == %ld %s\n", e3, peropenervalue, (peropenervalue == e3) ? "OK" : "FAIL!");

    /* Test operating on a "global" variable exposed from peropener.library through userel.library */
    const int e4 = 5;
    SetPeropenerLibraryValue(e4);
    int r4 = GetPeropenerLibraryValue();
    Printf((STRPTR)"GetPeropenerLibraryValue: %ld == %ld %s\n", e4, r4, (r4 == e4) ? "OK" : "FAIL!");

    /* Future: Test shaing a "global" variable exposed from peropener.library with userel.library */
    /* TODO: bind this programs PeropenerBase into userel.library */
    const int e5 = 7;
    SetPeropenerLibraryValue(e5);
    int r5 = GetPeropenerLibraryValue();
    Printf((STRPTR)"GetPeropenerLibraryValue: %ld == %ld %s\n", e5, r5, (r5 == e5) ? "OK" : "FAIL!");
    Printf((STRPTR)"peropenervalue: %ld == %ld %s\n", e5, peropenervalue, (peropenervalue == e5) ? "OK" : "FAIL!"); // <- This fails now

    Flush (Output ());
    
    return 0;
}
