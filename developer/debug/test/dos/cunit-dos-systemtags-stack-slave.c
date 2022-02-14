/*
    Copyright (C) 2022, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dos.h>

int main(int argc, char *argv[])
{
    char buff[64];
    BPTR file;
    LONG stacksize;
    struct Task *me = FindTask(NULL);

    file = Open("T:systemtags-stack-slave.txt", MODE_NEWFILE);
    stacksize = (LONG)(((UBYTE *)me->tc_SPUpper) - ((UBYTE *)me->tc_SPLower));

    FPrintf(file, "%ld", stacksize);

    Close(file);

    return 0;
}
