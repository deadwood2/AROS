/*
    Copyright (C) 1995-2025, The AROS Development Team. All rights reserved.

    C99 function fputwc().
    
    Originally part of libnix.
*/

/*****************************************************************************

    NAME */
#include <wchar.h>
#include <stdio.h>

#undef putwc

wint_t fputwc(
/*     SYNOPSIS    */    
    wchar_t wc, 
    FILE * fp) 
/*  FUNCTION
        Writes the wide character wc to the stream and advances the position indicator.

    INPUTS
        wc - The wide character to write.
        fp - Pointer to a FILE object that identifies an output stream.
        
    RESULT
        On success, the character written is returned.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
	return fwrite(&wc, 1, sizeof(wchar_t), fp);
}