/*
    Copyright (C) 2012-2013, The AROS Development Team. All rights reserved.

    Get pointer to errno variable in stdc.library libbase.
    This function is in both the static linklib and stdc.library.
*/

int *__stdc_geterrnoptr(void)
{
    return &(__aros_getbase_StdCBase()->_errno);
}
