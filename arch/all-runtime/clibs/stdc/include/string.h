#ifndef AXRT_STRING_H
#define AXRT_STRING_H

#include "/usr/include/string.h"

/* BSD/other UNIX */
size_t strlcpy(char *dst, const char *src, size_t size);
size_t strlcat(char *dst, const char *src, size_t size);
char *strsep(char **, const char *);
char *strlwr(char *);
char *strupr(char *);

/* SAS/C */
char * strrev(char * s);
int stch_l(const char * in, long * lvalue);
int stcd_l(const char * in, long * lvalue);
int stco_l(const char * in, long * lvalue);
int stcl_d(char * out, long lvalue);
int stcl_h(char * out, long lvalue);
int stcl_o(char * out, long lvalue);
int stcu_d(char * out, unsigned uivalue);

/* Supplemental (not std C) */
size_t stccpy(char *str1_ptr, const char *str2_ptr, size_t max);
char *stpsym(char *str_ptr, char *dest_ptr, int dest_size);
char * stpblk( const char * str );
#define stpbrk(s,cs) strpbrk(s,cs)
#define stpchr(s,c)  strchr(s,c)

/* BSD */
char *strcasestr(const char * str, const char * search);

/* Extension */
#define stricmp strcasecmp
#define strnicmp strncasecmp

#endif
