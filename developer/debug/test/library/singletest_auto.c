/*
    Copyright (C) 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/single.h>

int main (int argc, char ** argv)
{
    RegSetValue(5);

    const LONG e1 = 15;
    LONG r1 = RegAdd4(1, 2, 3, 4);
    Printf((STRPTR)"RegAdd4: %ld == %ld %s\n", e1, r1, (r1 == e1) ? "OK" : "FAIL!");

    const int e5 = 26;
    int r5 = StackAdd4OrMore(6, 1, 2, 3, 4, 5, 6);
    Printf((STRPTR)"StackAdd4OrMore: %ld == %ld %s\n", e5, r5, (r5 == e5) ? "OK" : "FAIL!");

    Flush (Output ());
    
    return 0;
}
