/*
    Copyright (C) 1995-2013, The AROS Development Team. All rights reserved.

    Desc: special main function for code which has to use special *nix features
*/

#include <aros/symbolsets.h>
#include <proto/crt.h>

AROS_IMPORT_ASM_SYM(int, dummy, __includecrtprogramstartuphandling);

extern int main(int argc, char *argv[]);

/* FIXME: This solution for -nix flag is now specific for posixc.library
   Could a more general approach be provided so other libs can add their own
   flags without needing to do something in compiler/startup
   Is this wanted ?
 */

int __nixmain(int argc, char *argv[])
{
    return __progonly_nixmain(main, argc, argv);
}

int (*__main_function_ptr)(int argc, char *argv[]) = __nixmain;

