#include <proto/dos.h>

#include "autoinit_intern.h"

/*****************************************************************************

    NAME */
#include <proto/autoinit.h>

        AROS_LH1(BOOL, ProgramNameInit,

/*  SYNOPSIS */
        AROS_LHA(STRPTR *, _ProgramName, A1),

/*  LOCATION */
        struct Library *, AutoinitBase, 7, Autoinit)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES
        If the startup code has to change in backwards incompatible way
        do not change this function. Create a new one and keep this compatible
        so that already compiled executatables continue running.

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    char *cmd = __get_command_name();

    (*_ProgramName) = FilePart(cmd);

    return TRUE;

    AROS_LIBFUNC_EXIT
} /* ProgramNameInit */
