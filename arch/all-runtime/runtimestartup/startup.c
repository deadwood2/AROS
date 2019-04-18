/*
    Copyright © 2019, The AROS Development Team. All rights reserved.
    $Id$
*/

extern void __init_runtime();
const void * const __init_runtime_symbol __attribute__((__section__(".init_array"))) __attribute__((used)) = (void *)&__init_runtime;
