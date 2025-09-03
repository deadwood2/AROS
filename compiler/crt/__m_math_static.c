/*
    Copyright (C) 2025, The AROS Development Team. All rights reserved.
*/

#include <math.h>

#include <math_private.h>

double log10(double x) { return __ieee754_log10(x); }

double pow(double x, double y) { return __ieee754_pow(x, y); }

double sqrt(double x) { return __ieee754_sqrt(x); }
