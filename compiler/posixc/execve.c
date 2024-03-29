/*
    Copyright (C) 2008-2021, The AROS Development Team. All rights reserved.

    POSIX.1-2008 function execve().
*/

#include <aros/debug.h>

#include <assert.h>

#include "../crt/__exec.h"

/*****************************************************************************

    NAME */
#include <unistd.h>

        int __progonly_execve(

/*  SYNOPSIS */
        const char *filename,
        char *const argv[],
        char *const envp[])
        
/*  FUNCTION
        Executes a file with given name.

    INPUTS
        filename - Name of the file to execute.
        argv - Array of arguments provided to main() function of the executed
        file.
        envp - Array of environment variables passed as environment to the
        executed program.

    RESULT
        Returns -1 and sets errno appropriately in case of error, otherwise
        doesn't return.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
        
    INTERNALS

******************************************************************************/
{
    APTR id = __progonly_exec_prepare(filename, 0, argv, envp);
    if(!id)
        return -1;
    
    __progonly_exec_do(id);
    
    assert(0); /* Should not be reached */
    return -1;
} /* execve() */
