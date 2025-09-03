#ifndef _POSIXC_LOCALE_H_
#define _POSIXC_LOCALE_H_

/*
    Copyright � 1995-2025, The AROS Development Team. All rights reserved.
    $Id$

    POSIX.1-2008 header file: locale.h
*/

#include <aros/features.h>
#include <aros/stdc/locale.h> /* C99 and earlier locale definitions */

#ifdef __cplusplus
extern "C" {
#endif

/* Additional POSIX LC_ constants */
#define LC_MESSAGES         6
#define _LC_LAST            7 /* Marks end of LC_ constants */

/* Category mask values for newlocale() */
#define LC_COLLATE_MASK     (1 << LC_COLLATE)
#define LC_CTYPE_MASK       (1 << LC_CTYPE)
#define LC_MONETARY_MASK    (1 << LC_MONETARY)
#define LC_NUMERIC_MASK     (1 << LC_NUMERIC)
#define LC_TIME_MASK        (1 << LC_TIME)
#define LC_MESSAGES_MASK    (1 << LC_MESSAGES)
/* NOTIMPL #define LC_ALL_MASK         (LC_COLLATE_MASK | LC_CTYPE_MASK | LC_MONETARY_MASK | LC_NUMERIC_MASK | LC_TIME_MASK | LC_MESSAGES_MASK) */

/* POSIX.1-2008 extended locale functions */
#if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200809L
/* NOTIMPL locale_t      duplocale(locale_t); */
/* NOTIMPL void          freelocale(locale_t); */
/* NOTIMPL locale_t      newlocale(int, const char *, locale_t); */
/* NOTIMPL locale_t      uselocale (locale_t); */
/* NOTIMPL: int strcoll_l(const char *s1, const char *s2, locale_t loc); */
/* NOTIMPL: size_t strxfrm_l(char *dest, const char *src, size_t n, locale_t loc);  */
#endif /* _POSIX_C_SOURCE >= 200809L */

#ifdef __cplusplus
}
#endif

#endif /* _POSIXC_LOCALE_H_ */
