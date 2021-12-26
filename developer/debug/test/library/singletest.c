/*
    Copyright (C) 2008-2009, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/single.h>

/* Block autopenening of SingleBase */
struct Library *SingleBase = NULL;

void testsingle_reg()
{
    FPuts(Output(), (STRPTR)"Testing single.library, reg calls\n");

    SingleBase = OpenLibrary((STRPTR)"single.library",0);

    /* Set initial value */
    RegSetValue(6);

    /* Check fixed number of argument functions */
    const LONG e1 = 16;
    LONG r1 = RegAdd4(1, 2, 3, 4);
    Printf((STRPTR)"RegAdd4: %ld == %ld %s\n", e1, r1, (r1 == e1) ? "OK" : "FAIL!");

    const LONG e2 = 27;
    LONG r2 = RegAdd6(1, 2, 3, 4, 5, 6);
    Printf((STRPTR)"RegAdd6: %ld == %ld %s\n", e2, r2, (r2 == e2) ? "OK" : "FAIL!");

    const LONG e3 = 51;
    LONG r3 = RegAdd9(1, 2, 3, 4, 5, 6, 7, 8, 9);
    Printf((STRPTR)"RegAdd9: %ld == %ld %s\n", e3, r3, (r3 == e3) ? "OK" : "FAIL!");

    CloseLibrary(SingleBase);
    SingleBase = NULL;
}

void testsingle_stack()
{
    FPuts(Output(), (STRPTR)"Testing single.library, stack calls\n");

    SingleBase = OpenLibrary((STRPTR)"single.library",0);

    /* Set initial value */
    StackSetValue(5);

    /* Check fixed number of argument functions */
    const int e1 = 15;
    int r1 = StackAdd4(1, 2, 3, 4);
    Printf((STRPTR)"StackAdd4: %ld == %ld %s\n", e1, r1, (r1 == e1) ? "OK" : "FAIL!");

    const int e2 = 26;
    int r2 = StackAdd6(1, 2, 3, 4, 5, 6);
    Printf((STRPTR)"StackAdd6: %ld == %ld %s\n", e2, r2, (r2 == e2) ? "OK" : "FAIL!");

    const int e3 = 50;
    int r3 = StackAdd9(1, 2, 3, 4, 5, 6, 7, 8, 9);
    Printf((STRPTR)"StackAdd9: %ld == %ld %s\n", e3, r3, (r3 == e3) ? "OK" : "FAIL!");

    /* Check variable number of argument functions */

    const int e4 = 15;
    int r4 = StackAdd4OrMore(4, 1, 2, 3, 4);
    Printf((STRPTR)"StackAdd4OrMore: %ld == %ld %s\n", e4, r4, (r4 == e4) ? "OK" : "FAIL!");

    const int e5 = 26;
    int r5 = StackAdd4OrMore(6, 1, 2, 3, 4, 5, 6);
    Printf((STRPTR)"StackAdd4OrMore: %ld == %ld %s\n", e5, r5, (r5 == e5) ? "OK" : "FAIL!");

    const int e6 = 50;
    int r6 = StackAdd4OrMore(9, 1, 2, 3, 4, 5, 6, 7, 8, 9);
    Printf((STRPTR)"StackAdd4OrMore: %ld == %ld %s\n", e6, r6, (r6 == e6) ? "OK" : "FAIL!");

    CloseLibrary(SingleBase);
    SingleBase = NULL;
}

int main (int argc, char ** argv)
{
    testsingle_stack();
    testsingle_reg();

    return 0;
}
