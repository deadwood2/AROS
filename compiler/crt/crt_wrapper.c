#define remove(a) __stdc_remove(a)
#include "./stdc/remove.c"
#undef remove
#include "./posixc/remove.c"
#include "./posixc/rmdir.c"

#define rename(a, b) __stdc_rename(a, b)
#include "./stdc/rename.c"
#undef rename
#include "./posixc/rename.c"

/* Needs to be included here, as posixc/strerror is already included in mmakefile.src and
   these two files overalap by name */
#include "./stdc/strerror.c"

#define __stdc_geterrnoptr(a) ___geterrnoptr(a)
#include "./stdc/__stdc_geterrnoptr.c"

#define __stdc_getsigngamptr(a) ___getsigngamptr(a)
#include "./stdc/__stdc_getsigngamptr.c"

#define __stdc_mb_cur_max(a) ___mb_cur_max(a)
#include "./stdc/__stdc_mb_cur_max.c"

#define __stdc_gmtoffset(a) ___gmtoffset(a)
#include "./posixc/gettimeofday.c"
#include "./posixc/clock_gettime.c"
