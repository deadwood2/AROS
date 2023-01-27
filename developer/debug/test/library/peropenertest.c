/*
    Copyright (C) 2008-2009, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/peropener.h>

/* Block autopenening of PeropenerBase */
struct Library *PeropenerBase = NULL;

void testperopener_reg()
{
    struct Library *base1, *base2;

    FPuts(Output(), (STRPTR)"Testing peropener.library, reg calls\n");

    base1=OpenLibrary((STRPTR)"peropener.library",0);
    base2=OpenLibrary((STRPTR)"peropener.library",0);

    /* Set value for base1 */
    PeropenerBase = base1;
    PeropenerSetValueReg(1);

    /* Set value for base2 */
    PeropenerBase = base2;
    PeropenerSetValueReg(2);

    /* Check value for base2 */
    Printf((STRPTR)"Checking value for base2: 2 == %ld %s\n",
           PeropenerGetValueReg(), (PeropenerGetValueReg() == 2) ? "OK" : "FAIL!"
    );

    /* Check value for base1 */
    PeropenerBase = base1;
    Printf((STRPTR)"Checking value for base1: 1 == %ld %s\n",
           PeropenerGetValueReg(), (PeropenerGetValueReg() == 1) ? "OK" : "FAIL!"
    );

    FPrintf(Output(), (STRPTR)"base1=%lx, base2=%lx\n", base1, base2);

    if (base1 != NULL)
        CloseLibrary(base1);
    if (base2 != NULL)
        CloseLibrary(base2);
}

void testperopener_stack()
{
    struct Library *base1, *base2;

    FPuts(Output(), (STRPTR)"Testing peropener.library, stack calls\n");
    
    base1=OpenLibrary((STRPTR)"peropener.library",0);
    base2=OpenLibrary((STRPTR)"peropener.library",0);

    /* Set value for base1 */
    PeropenerBase = base1;
    PeropenerSetValueStack(1);

    /* Check .unusedlibbase option with base1 */
    if (PeropenerNoLib() != 1)
        Printf("Error calling PeropenerNoLib()\n");

    /* Set value for base2 */
    PeropenerBase = base2;
    PeropenerSetValueStack(2);

    /* Check .function option with base2 */
    if (PeropenerNameChange() != 1)
        Printf("Error calling PeropenerNameChange()\n");

    /* Check value for base2 */
    Printf((STRPTR)"Checking value for base2: 2 == %ld %s\n",
           PeropenerGetValueStack(), (PeropenerGetValueStack() == 2) ? "OK" : "FAIL!"
    );

    /* Check value for base1 */
    PeropenerBase = base1;
    Printf((STRPTR)"Checking value for base1: 1 == %ld %s\n",
           PeropenerGetValueStack(), (PeropenerGetValueStack() == 1) ? "OK" : "FAIL!"
    );

    FPrintf(Output(), (STRPTR)"base1=%lx, base2=%lx\n", base1, base2);
    
    if (base1 != NULL)
        CloseLibrary(base1);
    if (base2 != NULL)
        CloseLibrary(base2);

}

void testperopener_reg_peropenerglobal()
{
    struct Library *base1, *base2;

    FPuts(Output(), (STRPTR)"Testing peropener.library, reg calls, global per opener value \n");

    base1=OpenLibrary((STRPTR)"peropener.library",0);
    base2=OpenLibrary((STRPTR)"peropener.library",0);

    /* Check value for base2 */
    PeropenerBase = base2;
    Printf((STRPTR)"Checking initial value for base2: 8 == %ld %s\n",
           PeropenerGetGlobalPeropenerValueReg(), (PeropenerGetGlobalPeropenerValueReg() == 8) ? "OK" : "FAIL!"
    );

    /* Check value for base1 */
    PeropenerBase = base1;
    Printf((STRPTR)"Checking initial value for base1: 8 == %ld %s\n",
           PeropenerGetGlobalPeropenerValueReg(), (PeropenerGetGlobalPeropenerValueReg() == 8) ? "OK" : "FAIL!"
    );

    /* Set value for base1 */
    PeropenerBase = base1;
    PeropenerSetGlobalPeropenerValueReg(1);

    /* Set value for base2 */
    PeropenerBase = base2;
    PeropenerSetGlobalPeropenerValueReg(2);

    /* Check value for base2 */
    Printf((STRPTR)"Checking value for base2: 2 == %ld %s\n",
           PeropenerGetGlobalPeropenerValueReg(), (PeropenerGetGlobalPeropenerValueReg() == 2) ? "OK" : "FAIL!"
    );

    /* Check value for base1 */
    PeropenerBase = base1;
    Printf((STRPTR)"Checking value for base1: 1 == %ld %s\n",
           PeropenerGetGlobalPeropenerValueReg(), (PeropenerGetGlobalPeropenerValueReg() == 1) ? "OK" : "FAIL!"
    );

    FPrintf(Output(), (STRPTR)"base1=%lx, base2=%lx\n", base1, base2);

    if (base1 != NULL)
        CloseLibrary(base1);
    if (base2 != NULL)
        CloseLibrary(base2);
}

int main (int argc, char ** argv)
{
    testperopener_stack();
    testperopener_reg();

    testperopener_reg_peropenerglobal();

    return 0;
}
