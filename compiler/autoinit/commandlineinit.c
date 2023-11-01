/*
    Copyright © 1995-2014, The AROS Development Team. All rights reserved.
    $Id$
*/
#include <aros/debug.h>
#include "autoinit_intern.h"

static void process_cmdline(int *argc, char *args, char *argv[]);

#define __argv      (*__argvp)
#define __argc      (*__argcp)
#define __args      (*__argsp)
#define __argmax    (*__argmaxp)

/*****************************************************************************

    NAME */
#include <proto/autoinit.h>

        AROS_LH8(BOOL, CommandLineInit,

/*  SYNOPSIS */
        AROS_LHA(CONST_STRPTR, __argstr, A1),
        AROS_LHA(ULONG, __argsize, D1),
        AROS_LHA(STRPTR *, __argsp, A2),
        AROS_LHA(LONG *, __argmaxp, A3),
        AROS_LHA(STRPTR **, __argvp, A4),
        AROS_LHA(LONG *, __argcp, A5),
        AROS_LHA(APTR, WBenchMsg, D5),
        AROS_LHA(LONG, __nocommandline, D6),

/*  LOCATION */
        struct Library *, AutoinitBase, 8, Autoinit)

/*  FUNCTION

    INPUTS

    RESULT
        FALSE if there was a problem with processing command line.

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

    char *ptr = NULL;

    if (WBenchMsg || __nocommandline)
        return TRUE; /* Continue with processing */

    if (__argsize)
    {
        ULONG size;

        /* Copy args into buffer */
        if (!(__args = AllocMem(__argsize+1, MEMF_ANY)))
            return FALSE;

        ptr = __args;
        size= __argsize;
        while (size--)
            *ptr++ = *__argstr++;
        *ptr = 0;

        /* Find out how many arguments we have */
        process_cmdline(&__argmax, __args, NULL);

        if (!(__argv = AllocMem (sizeof (char *) * (__argmax+1), MEMF_ANY | MEMF_CLEAR)) )
            return FALSE;

        D(bug("arg(%d)=\"%s\", argmax=%d\n", __argsize, __args, __argmax));

        /* create argv */
        process_cmdline(&__argc, __args, (char **)__argv);
    }
    else
    {
        __argmax = 1;
        __argc = 1;
        if (!(__argv = AllocMem (sizeof (char *)*2, MEMF_CLEAR | MEMF_ANY)))
            return FALSE;
    }

    /*
     * get program name
     */
     __argv[0] = __get_command_name();

#if DEBUG /* Debug argument parsing */

    kprintf("argc = %d\n", __argc);
    {
    int t;
    for (t=0; t<__argc; t++)
        kprintf("argv[%d] = \"%s\"\n", t, __argv[t]);
    }

#endif

    return TRUE;

    AROS_LIBFUNC_EXIT
} /* CommandLineInit */

static BOOL is_space(char c)
{
    switch(c)
    {
    case ' ':
    case '\n':
    case '\t':
    case '\v':
    case '\f':
    case '\r':
        return TRUE;
    default:
        return FALSE;
    }
}

static BOOL is_escape(char c)
{
    if(c == '*')
        return TRUE;
    else
        return FALSE;
}

static BOOL is_final_quote(char *ptr)
{
    if(*ptr == '\"' && (ptr[1] == '\0' || is_space(ptr[1])))
        return TRUE;
    else
        return FALSE;
}

static void process_cmdline(int *pargc, char *args, char *argv[])
{
    char *ptr = args;
    char *arg;
    int argc = 1;

    while(TRUE)
    {
        /* skip leading white spaces */
        while(is_space(*ptr))
            ptr++;

        if(*ptr == '\0')
            break;

        argc++;
        if(*ptr == '\"')
        {
            /* quoted parameter starts here */
            ptr++;

            /* store pointer to the parameter */
            if(argv)
                argv[argc-1] = ptr;

            /* unescape quoted parameter */
            arg = ptr;
            while(!(*ptr == '\0' || is_final_quote(ptr)))
            {
                if(argv)
                {
                    /* unescaping */
                    if(is_escape(*ptr))
                    {
                        ptr++;
                        switch(*ptr)
                        {
                        case 'e':
                        case 'E':
                            *arg++ = '\033';
                            break;
                        case 'N':
                        case 'n':
                            *arg++ = '\n';
                            break;
                        case '\0':
                            break;
                        default:
                            *arg++ = *ptr;
                        }
                        ptr++;
                    }
                    else
                        *arg++ = *ptr++;
                }
                else
                {
                    /* don't touch anything, just skip escapes */
                    if(is_escape(*ptr))
                    {
                        ptr++;
                        if(*ptr != '\0')
                            ptr++;
                    }
                    else
                        ptr++;
                }
            }
            /* skip final quote */
            if(*ptr != '\0')
                ptr++;
            /* quoted parameter ends here */
            if(argv)
                *arg = '\0';
        }
        else
        {
            /* unquoted parameter starts here */

            /* store pointer to the parameter */
            if(argv)
                argv[argc-1] = ptr;

            /* no escaping, just find the end */
            while(!(*ptr == '\0' || is_space(*ptr)))
                ptr++;

            /* stop processing if we reached the end of argument string */
            if(*ptr == '\0')
                break;

            /* unquoted parameter ends here */
            if(argv)
                *ptr++ = '\0';
        }
    }
    /* store the number of arguments */
    *pargc = argc;
}
