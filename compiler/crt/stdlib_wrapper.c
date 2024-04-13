#define STDC_STATIC

/* strto(l)/(ul)/(ll)/(ull) family sets errno, which is not supported
   in this library. Certain other function, which don't set errno,
   use the family. In order not to expose broken "strto" functions
   this wrapper renames them
*/

#define strtoll(a, b, c)    __no_errno_strtoll(a, b, c)
#define strtoull(a, b, c)   __no_errno_strtoull(a, b, c)

#include "../stdc/strtoull.c"
#include "../stdc/strtoll.c"

#include "../stdc/atoll.c"

#define strtol(a, b, c)     __no_errno_strtol(a, b, c)
#define strtoul(a, b, c)    __no_errno_strtoul(a, b, c)

#include "../stdc/strtoul.c"
#include "../stdc/strtol.c"

#include "../stdc/atoi.c"
#include "../stdc/atol.c"
#include "../stdc/stcd_l.c"
#include "../stdc/stch_l.c"
#include "../stdc/stco_l.c"

#define __stdc_gmtoffset(a) ___gmtoffset(a)
#include "../stdc/localtime_r.c"

