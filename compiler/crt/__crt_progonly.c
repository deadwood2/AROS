/*
    Copyright (C) 2021, The AROS Development Team. All rights reserved.

    This file contains support for functions that can only be called from
    a program, not from another library.
*/
#include <dos/stdio.h>
#include <exec/alerts.h>
#include <proto/exec.h>

#include <assert.h>
#include <setjmp.h>

#include <aros/debug.h>

#include "__crt_intbase.h"
#include "../stdc/__exitfunc.h"

void __progonly_program_startup_internal(struct CrtProgCtx *ProgCtx, jmp_buf exitjmp, int *errorptr)
{
    ProgCtx->startup_errorptr = errorptr;
    *ProgCtx->exit_jmpbuf = *exitjmp;
}

/*****************************************************************************

    NAME */
        void __progonly_program_startup(

/*  SYNOPSIS */
        jmp_buf exitjmp,
        int *errorptr)

/*  FUNCTION
        This is called during program startup and before calling main.
        This is to allow stdc.library to do some initialization that couldn't
        be done when opening the library.

    INPUTS
        exitjmp - jmp_buf to jump to to exit the program
        errorptr - pointer to store return value of program

    RESULT
        -

    NOTES
        This function is normally called by the startup code so one
        should not need to do it oneself.

        TODO: Maybe this function should be implemented using Tags so that
        functionality can be extended in the future without breaking backwards
        compatibility.

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
    struct CrtProgCtx *ProgCtx = __aros_create_ProgCtx();

    D(bug("[CRTPROG] %s: ProgCtx 0x%p\n", __func__, ProgCtx));

    __progonly_program_startup_internal(ProgCtx, exitjmp, errorptr);
}

/*****************************************************************************

    NAME */
        void __progonly_program_end(

/*  SYNOPSIS */
        void)

/*  FUNCTION
        This function is to be called when main() has returned or after
        program has exited. This allows to stdc.library to do some
        cleanup that can't be done during closing of the library.

    INPUTS
        -

    RESULT
        -

    NOTES
        This function is normally called by the startup code so one
        should not need to do it oneself.

        TODO: Maybe this function should be implemented using Tags so that
        functionality can be extended in the future without breaking backwards
        compatibility.

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS


******************************************************************************/
{
    struct CrtProgCtx *ProgCtx = __aros_get_ProgCtx();
    D(bug("[CRTPROG] %s()\n", __func__));

    struct ETask *etask = GetETask(FindTask(NULL));
    if (etask)
        etask->et_Result1 = *ProgCtx->startup_errorptr;

    if (!(ProgCtx->exitflags & ABNORMAL_EXIT))
        __progonly_callexitfuncs();

    __aros_delete_ProgCtx();
}

/*****************************************************************************

    NAME */
        int *__progonly_set_errorptr(

/*  SYNOPSIS */
        int *errorptr)

/*  FUNCTION
        This function sets the pointer to store error return value for
        program exit.

    INPUTS
        errorptr - new pointer to return value

    RESULT
        old pointer to return value

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
    struct CrtProgCtx *ProgCtx = __aros_get_ProgCtx();
    int *old = ProgCtx->startup_errorptr;

    ProgCtx->startup_errorptr = errorptr;

    return old;
}

/*****************************************************************************

    NAME */
        int *__progonly_get_errorptr(

/*  SYNOPSIS */
        void)

/*  FUNCTION
        This function gets the pointer to store error return value for
        program exit.

    INPUTS
        -

    RESULT
        pointer to return value

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
    struct CrtProgCtx *ProgCtx = __aros_get_ProgCtx();
    return ProgCtx->startup_errorptr;
}

/*****************************************************************************

    NAME */
         void __progonly_set_exitjmp(

/*  SYNOPSIS */
        jmp_buf exitjmp,
        jmp_buf previousjmp)

/*  FUNCTION
        This function set the jmp_buf to use for directly exiting current
        program.

    INPUTS
        exitjmp - new jmp_buf for exiting

    RESULT
        previous jmp_buf for exiting

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
    struct CrtProgCtx *ProgCtx = __aros_get_ProgCtx();
   
    *previousjmp = *ProgCtx->exit_jmpbuf;
    *ProgCtx->exit_jmpbuf = *exitjmp;
}

/*****************************************************************************

    NAME */
        void __progonly_jmp2exit(

/*  SYNOPSIS */
        int normal,
        int retcode)

/*  FUNCTION
        This function directly jumps to the exit of a program.

    INPUTS
        normal - Indicates if exit is normal or not. When it is abnormal no
                 atexit functions will be called.
        retcode - the return code for the program.

    RESULT
        -

    NOTES
        In normal operation this function does not return.
        If this function returns it means that this function was called in a
        context where jmp_buf for exit was not initialized. Likely cause is
        a module that opened stdc.library.
        Be sure to capture this situation.

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
    struct CrtProgCtx *ProgCtx = __aros_get_ProgCtx();

    /* No __stdc_program_startup() called; Alert()
    */
    if (!ProgCtx || ProgCtx->startup_errorptr == NULL)
    {
        kprintf("[CRTPROG] %s: Trying to exit without proper initialization\n", __func__);
        Alert(AT_DeadEnd | AG_BadParm);
    }

    if (!normal)
        ProgCtx->exitflags |= ABNORMAL_EXIT;

    *ProgCtx->startup_errorptr = retcode;

    longjmp(ProgCtx->exit_jmpbuf, 1);

    assert(0); /* Not reached */
}
