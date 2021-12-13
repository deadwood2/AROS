#include "../stdc/strerror.c"

#define remove(a) __stdc_remove(a)
#include "../stdc/remove.c"
#include "../posixc/remove.c"

#define rename(a, b) __stdc_rename(a, b)
#include "../stdc/rename.c"
#include "../posixc/rename.c"
#include "../posixc/rmdir.c"
