/*
    Copyright (C) 2025, The AROS Development Team. All rights reserved.
*/

#include <math.h>

#include <math_private.h>

double acos(double x) { return __ieee754_acos(x); }
#if LDBL_MANT_DIG == DBL_MANT_DIG
AROS_MAKE_ASM_SYM(typeof(acosl), acosl, AROS_CSYM_FROM_ASM_NAME(acosl), AROS_CSYM_FROM_ASM_NAME(acos));
AROS_EXPORT_ASM_SYM(AROS_CSYM_FROM_ASM_NAME(acosl));
#else
long double acosl(long double x) { return __ieee754_acosl(x); }
#endif

double acosh(double x) { return __ieee754_acosh(x); }
#if (LDBL_MANT_DIG == DBL_MANT_DIG)
AROS_MAKE_ASM_SYM(typeof(acoshl), acoshl, AROS_CSYM_FROM_ASM_NAME(acoshl), AROS_CSYM_FROM_ASM_NAME(acosh));
AROS_EXPORT_ASM_SYM(AROS_CSYM_FROM_ASM_NAME(acoshl));
#else
long double acoshl(long double x){ return __ieee754_acoshl(x); }
#endif

double asin(double x) { return __ieee754_asin(x); }
#if LDBL_MANT_DIG == DBL_MANT_DIG
AROS_MAKE_ASM_SYM(typeof(asinl), asinl, AROS_CSYM_FROM_ASM_NAME(asinl), AROS_CSYM_FROM_ASM_NAME(asin));
AROS_EXPORT_ASM_SYM(AROS_CSYM_FROM_ASM_NAME(asinl));
#else
long double asinl(long double x) { return __ieee754_asinl(x); }
#endif

double atan2(double x, double y) { return __ieee754_atan2(x, y); }
#if LDBL_MANT_DIG == DBL_MANT_DIG
AROS_MAKE_ASM_SYM(typeof(atan2l), atan2l, AROS_CSYM_FROM_ASM_NAME(atan2l), AROS_CSYM_FROM_ASM_NAME(atan2));
AROS_EXPORT_ASM_SYM(AROS_CSYM_FROM_ASM_NAME(atan2l));
#else
long double atan2l(long double x, long double y) { return __ieee754_atan2l(x, y); }
#endif

double atanh(double x) { return __ieee754_atanh(x); }
#if LDBL_MANT_DIG == DBL_MANT_DIG
AROS_MAKE_ASM_SYM(typeof(atanhl), atanhl, AROS_CSYM_FROM_ASM_NAME(atanhl), AROS_CSYM_FROM_ASM_NAME(atanh));
AROS_EXPORT_ASM_SYM(AROS_CSYM_FROM_ASM_NAME(atanhl));
#else
long double atanhl(long double x) { return __ieee754_atanhl(x); }
#endif

double cosh(double x) { return __ieee754_cosh(x); }
#if LDBL_MANT_DIG == DBL_MANT_DIG
AROS_MAKE_ASM_SYM(typeof(coshl), coshl, AROS_CSYM_FROM_ASM_NAME(coshl), AROS_CSYM_FROM_ASM_NAME(cosh));
AROS_EXPORT_ASM_SYM(AROS_CSYM_FROM_ASM_NAME(coshl));
#else
long double coshl(long double x) { return __ieee754_coshl(x); }
#endif

double exp(double x) { return __ieee754_exp(x); }
#if LDBL_MANT_DIG == DBL_MANT_DIG
AROS_MAKE_ASM_SYM(typeof(expl), expl, AROS_CSYM_FROM_ASM_NAME(expl), AROS_CSYM_FROM_ASM_NAME(exp));
AROS_EXPORT_ASM_SYM(AROS_CSYM_FROM_ASM_NAME(expl));
#else
long double expl(long double x) { return __ieee754_expl(x); }
#endif

double fmod(double x, double y) { return __ieee754_fmod(x, y); }
#if LDBL_MANT_DIG == DBL_MANT_DIG
AROS_MAKE_ASM_SYM(typeof(fmodl), fmodl, AROS_CSYM_FROM_ASM_NAME(fmodl), AROS_CSYM_FROM_ASM_NAME(fmod));
AROS_EXPORT_ASM_SYM(AROS_CSYM_FROM_ASM_NAME(fmodl));
#else
long double fmodl(long double x, long double y) { return __ieee754_fmodl(x, y); }
#endif

double hypot(double x, double y) { return __ieee754_hypot(x, y); }
#if LDBL_MANT_DIG == DBL_MANT_DIG
AROS_MAKE_ASM_SYM(typeof(hypotl), hypotl, AROS_CSYM_FROM_ASM_NAME(hypotl), AROS_CSYM_FROM_ASM_NAME(hypot));
AROS_EXPORT_ASM_SYM(AROS_CSYM_FROM_ASM_NAME(hypotl));
#else
long double hypotl(long double x, long double y) { return __ieee754_hypotl(x, y); }
#endif

double j0(double x) { return __ieee754_j0(x); }
double j1(double x) { return __ieee754_j1(x); }
double jn(int n, double x) { return __ieee754_jn(n, x); }
double y0(double x) { return __ieee754_y0(x); }
double y1(double x) { return __ieee754_y1(x); }
double yn(int n, double x) { return __ieee754_yn(n, x); }

double log(double x) { return __ieee754_log(x); }
#if LDBL_MANT_DIG == DBL_MANT_DIG
AROS_MAKE_ASM_SYM(typeof(logl), logl, AROS_CSYM_FROM_ASM_NAME(logl), AROS_CSYM_FROM_ASM_NAME(log));
AROS_EXPORT_ASM_SYM(AROS_CSYM_FROM_ASM_NAME(logl));
#else
long double logl(long double x) { return __ieee754_logl(x); }
#endif

double log10(double x) { return __ieee754_log10(x); }
#if LDBL_MANT_DIG == DBL_MANT_DIG
AROS_MAKE_ASM_SYM(typeof(log10l), log10l, AROS_CSYM_FROM_ASM_NAME(log10l), AROS_CSYM_FROM_ASM_NAME(log10));
AROS_EXPORT_ASM_SYM(AROS_CSYM_FROM_ASM_NAME(log10l));
#else
long double log10l(long double x) { return __ieee754_log10l(x); }
#endif

double log1p(double x) { return __ieee754_log1p(x); }
#if LDBL_MANT_DIG == DBL_MANT_DIG
AROS_MAKE_ASM_SYM(typeof(log1pl), log1pl, AROS_CSYM_FROM_ASM_NAME(log1pl), AROS_CSYM_FROM_ASM_NAME(log1p));
AROS_EXPORT_ASM_SYM(AROS_CSYM_FROM_ASM_NAME(log1pl));
#else
long double log1pl(long double x) { return __ieee754_log1pl(x); }
#endif

double log2(double x) { return __ieee754_log2(x); }
#if LDBL_MANT_DIG == DBL_MANT_DIG
AROS_MAKE_ASM_SYM(typeof(log2l), log2l, AROS_CSYM_FROM_ASM_NAME(log2l), AROS_CSYM_FROM_ASM_NAME(log2));
AROS_EXPORT_ASM_SYM(AROS_CSYM_FROM_ASM_NAME(log2l));
#else
long double log2l(long double x) { return __ieee754_log2l(x); }
#endif

double modf(double x, double *iptr) { return __ieee754_modf(x, iptr); }
#if LDBL_MANT_DIG == DBL_MANT_DIG
AROS_MAKE_ASM_SYM(typeof(modfl), modfl, AROS_CSYM_FROM_ASM_NAME(modfl), AROS_CSYM_FROM_ASM_NAME(modf));
AROS_EXPORT_ASM_SYM(AROS_CSYM_FROM_ASM_NAME(modfl));
#else
long double modfl(long double x, long double *iptr) { return __ieee754_modfl(x, iptr); }
#endif

double pow(double x, double y) { return __ieee754_pow(x, y); }
#if LDBL_MANT_DIG == DBL_MANT_DIG
AROS_MAKE_ASM_SYM(typeof(powl), powl, AROS_CSYM_FROM_ASM_NAME(powl), AROS_CSYM_FROM_ASM_NAME(pow));
AROS_EXPORT_ASM_SYM(AROS_CSYM_FROM_ASM_NAME(powl));
#else
long double powl(long double x, long double y) { return __ieee754_powl(x, y); }
#endif

double remquo(double x, double y, int *quo) { return __ieee754_remquo(x, y, quo); }
#if LDBL_MANT_DIG == DBL_MANT_DIG
AROS_MAKE_ASM_SYM(typeof(remquol), remquol, AROS_CSYM_FROM_ASM_NAME(remquol), AROS_CSYM_FROM_ASM_NAME(remquo));
AROS_EXPORT_ASM_SYM(AROS_CSYM_FROM_ASM_NAME(remquol));
#else
long double remquol(long double x, long double y, int *quo) { return __ieee754_remquol(x, y, quo); }
#endif

double sqrt(double x) { return __ieee754_sqrt(x); }
#if LDBL_MANT_DIG == DBL_MANT_DIG
AROS_MAKE_ASM_SYM(typeof(sqrtl), sqrtl, AROS_CSYM_FROM_ASM_NAME(sqrtl), AROS_CSYM_FROM_ASM_NAME(sqrt));
AROS_EXPORT_ASM_SYM(AROS_CSYM_FROM_ASM_NAME(sqrtl));
#else
long double sqrtl(long double x) { return __ieee754_sqrtl(x); }
#endif

double tgamma(double x) { return __ieee754_tgamma(x); }
#if LDBL_MANT_DIG == DBL_MANT_DIG
AROS_MAKE_ASM_SYM(typeof(tgammal), tgammal, AROS_CSYM_FROM_ASM_NAME(tgammal), AROS_CSYM_FROM_ASM_NAME(tgamma));
AROS_EXPORT_ASM_SYM(AROS_CSYM_FROM_ASM_NAME(tgammal));
#else
long double tgammal(long double x) { return __ieee754_tgammal(x); }
#endif
