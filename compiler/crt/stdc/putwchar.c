/*
    Copyright (C) 1995-2025, The AROS Development Team. All rights reserved.

    C99 function putwchar().

*/

/*****************************************************************************

    NAME */
#include <stdio.h>
#include <wchar.h>

wint_t putwchar(
/*     SYNOPSIS    */
    wchar_t wc)
/*  FUNCTION
        Writes the wide character wc to the stream and advances the position indicator.

    INPUTS
        wc - The wide character to write.
        
    RESULT
        On success, the character written is returned.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
	return (fputwc(wc, stdout));
}