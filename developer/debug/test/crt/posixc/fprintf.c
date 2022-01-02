/*
    Copyright (C) 2021, The AROS Development Team. All rights reserved.
*/

#include <stdio.h>
#include "test.h"

int __fprintf_CrtBase_stack_varargs_cnt = 18;

void args2()
{
    fprintf(stderr,
        "%d %d\n",
        1, 2);
}

void args4()
{
    fprintf(stderr,
        "%d %d %d %d\n",
        1, 2, 3, 4);
}

void args9()
{
    fprintf(stderr,
        "%d %d %d %d %d %d %d %d %d\n",
        1, 2, 3, 4, 5, 6, 7, 8, 9);
}

void args20()
{
    fprintf(stderr,
        "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20);
}

void args22()
{
    fprintf(stderr,
        "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22);
}

void argsAll()
{
    fprintf(stderr,
        "%d %d\n",
        1, 2);

    fprintf(stderr,
        "%d %d %d %d\n",
        1, 2, 3, 4);

    fprintf(stderr,
        "%d %d %d %d %d %d %d %d %d\n",
        1, 2, 3, 4, 5, 6, 7, 8, 9);

    fprintf(stderr,
        "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20);

    fprintf(stderr,
        "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22);
}

int main()
{
    args2();
    args4();
    args9();
    args20();
    args22();

    argsAll();

    return OK;
}

void cleanup()
{
}
