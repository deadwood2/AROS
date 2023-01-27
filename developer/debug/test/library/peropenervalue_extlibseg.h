/*
    Copyright (C) 2023, The AROS Development Team. All rights reserved.
*/

struct ExtLibSegJumpTable
{
    void (*SetGlobalValue)(int);
    int (*GetGlobalValue)();
};
