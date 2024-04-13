#define remove(a) __stdc_remove(a)
#include "../stdc/remove.c"
#undef remove
#include "../posixc/remove.c"
#include "../posixc/rmdir.c"

#define rename(a, b) __stdc_rename(a, b)
#include "../stdc/rename.c"
#undef rename
#include "../posixc/rename.c"

/* Needs to be included here, as posixc/strerror is already included in mmakefile.src and
   these two files overalap by name */
#include "../stdc/strerror.c"
