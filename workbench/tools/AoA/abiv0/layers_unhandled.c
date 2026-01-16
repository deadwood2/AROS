#include "include/aros/cpu.h"
#include "include/aros/proxy.h"
#include "include/exec/structures.h"

#include "support.h"

static void abiv0_unhandledLVO5() { unhandledLibraryFunction("layers.library", 5); }
MAKE_PROXY_ARG_1(unhandledLVO5)
static void abiv0_unhandledLVO6() { unhandledLibraryFunction("layers.library", 6); }
MAKE_PROXY_ARG_1(unhandledLVO6)
static void abiv0_unhandledLVO7() { unhandledLibraryFunction("layers.library", 7); }
MAKE_PROXY_ARG_1(unhandledLVO7)
static void abiv0_unhandledLVO8() { unhandledLibraryFunction("layers.library", 8); }
MAKE_PROXY_ARG_1(unhandledLVO8)
static void abiv0_unhandledLVO9() { unhandledLibraryFunction("layers.library", 9); }
MAKE_PROXY_ARG_1(unhandledLVO9)
static void abiv0_unhandledLVO10() { unhandledLibraryFunction("layers.library", 10); }
MAKE_PROXY_ARG_1(unhandledLVO10)
static void abiv0_unhandledLVO11() { unhandledLibraryFunction("layers.library", 11); }
MAKE_PROXY_ARG_1(unhandledLVO11)
static void abiv0_unhandledLVO12() { unhandledLibraryFunction("layers.library", 12); }
MAKE_PROXY_ARG_1(unhandledLVO12)
static void abiv0_unhandledLVO15() { unhandledLibraryFunction("layers.library", 15); }
MAKE_PROXY_ARG_1(unhandledLVO15)
static void abiv0_unhandledLVO18() { unhandledLibraryFunction("layers.library", 18); }
MAKE_PROXY_ARG_1(unhandledLVO18)
static void abiv0_unhandledLVO19() { unhandledLibraryFunction("layers.library", 19); }
MAKE_PROXY_ARG_1(unhandledLVO19)
static void abiv0_unhandledLVO21() { unhandledLibraryFunction("layers.library", 21); }
MAKE_PROXY_ARG_1(unhandledLVO21)
static void abiv0_unhandledLVO25() { unhandledLibraryFunction("layers.library", 25); }
MAKE_PROXY_ARG_1(unhandledLVO25)
static void abiv0_unhandledLVO26() { unhandledLibraryFunction("layers.library", 26); }
MAKE_PROXY_ARG_1(unhandledLVO26)
static void abiv0_unhandledLVO27() { unhandledLibraryFunction("layers.library", 27); }
MAKE_PROXY_ARG_1(unhandledLVO27)
static void abiv0_unhandledLVO28() { unhandledLibraryFunction("layers.library", 28); }
MAKE_PROXY_ARG_1(unhandledLVO28)
static void abiv0_unhandledLVO30() { unhandledLibraryFunction("layers.library", 30); }
MAKE_PROXY_ARG_1(unhandledLVO30)
static void abiv0_unhandledLVO31() { unhandledLibraryFunction("layers.library", 31); }
MAKE_PROXY_ARG_1(unhandledLVO31)
static void abiv0_unhandledLVO32() { unhandledLibraryFunction("layers.library", 32); }
MAKE_PROXY_ARG_1(unhandledLVO32)
static void abiv0_unhandledLVO33() { unhandledLibraryFunction("layers.library", 33); }
MAKE_PROXY_ARG_1(unhandledLVO33)
static void abiv0_unhandledLVO34() { unhandledLibraryFunction("layers.library", 34); }
MAKE_PROXY_ARG_1(unhandledLVO34)
static void abiv0_unhandledLVO35() { unhandledLibraryFunction("layers.library", 35); }
MAKE_PROXY_ARG_1(unhandledLVO35)
static void abiv0_unhandledLVO37() { unhandledLibraryFunction("layers.library", 37); }
MAKE_PROXY_ARG_1(unhandledLVO37)
static void abiv0_unhandledLVO38() { unhandledLibraryFunction("layers.library", 38); }
MAKE_PROXY_ARG_1(unhandledLVO38)
static void abiv0_unhandledLVO39() { unhandledLibraryFunction("layers.library", 39); }
MAKE_PROXY_ARG_1(unhandledLVO39)
static void abiv0_unhandledLVO40() { unhandledLibraryFunction("layers.library", 40); }
MAKE_PROXY_ARG_1(unhandledLVO40)
static void abiv0_unhandledLVO41() { unhandledLibraryFunction("layers.library", 41); }
MAKE_PROXY_ARG_1(unhandledLVO41)
static void abiv0_unhandledLVO42() { unhandledLibraryFunction("layers.library", 42); }
MAKE_PROXY_ARG_1(unhandledLVO42)
static void abiv0_unhandledLVO43() { unhandledLibraryFunction("layers.library", 43); }
MAKE_PROXY_ARG_1(unhandledLVO43)
static void abiv0_unhandledLVO44() { unhandledLibraryFunction("layers.library", 44); }
MAKE_PROXY_ARG_1(unhandledLVO44)
static void abiv0_unhandledLVO45() { unhandledLibraryFunction("layers.library", 45); }
MAKE_PROXY_ARG_1(unhandledLVO45)

void Layers_Unhandled_init(struct LibraryV0 *abiv0LayersBase)
{
    __AROS_SETVECADDRV0(abiv0LayersBase,  5, (APTR32)(IPTR)proxy_unhandledLVO5);
    __AROS_SETVECADDRV0(abiv0LayersBase,  6, (APTR32)(IPTR)proxy_unhandledLVO6);
    __AROS_SETVECADDRV0(abiv0LayersBase,  7, (APTR32)(IPTR)proxy_unhandledLVO7);
    __AROS_SETVECADDRV0(abiv0LayersBase,  8, (APTR32)(IPTR)proxy_unhandledLVO8);
    __AROS_SETVECADDRV0(abiv0LayersBase,  9, (APTR32)(IPTR)proxy_unhandledLVO9);
    __AROS_SETVECADDRV0(abiv0LayersBase,  10, (APTR32)(IPTR)proxy_unhandledLVO10);
    __AROS_SETVECADDRV0(abiv0LayersBase,  11, (APTR32)(IPTR)proxy_unhandledLVO11);
    __AROS_SETVECADDRV0(abiv0LayersBase,  12, (APTR32)(IPTR)proxy_unhandledLVO12);
    __AROS_SETVECADDRV0(abiv0LayersBase,  15, (APTR32)(IPTR)proxy_unhandledLVO15);
    __AROS_SETVECADDRV0(abiv0LayersBase,  18, (APTR32)(IPTR)proxy_unhandledLVO18);
    __AROS_SETVECADDRV0(abiv0LayersBase,  19, (APTR32)(IPTR)proxy_unhandledLVO19);
    __AROS_SETVECADDRV0(abiv0LayersBase,  21, (APTR32)(IPTR)proxy_unhandledLVO21);
    __AROS_SETVECADDRV0(abiv0LayersBase,  25, (APTR32)(IPTR)proxy_unhandledLVO25);
    __AROS_SETVECADDRV0(abiv0LayersBase,  26, (APTR32)(IPTR)proxy_unhandledLVO26);
    __AROS_SETVECADDRV0(abiv0LayersBase,  27, (APTR32)(IPTR)proxy_unhandledLVO27);
    __AROS_SETVECADDRV0(abiv0LayersBase,  28, (APTR32)(IPTR)proxy_unhandledLVO28);
    __AROS_SETVECADDRV0(abiv0LayersBase,  30, (APTR32)(IPTR)proxy_unhandledLVO30);
    __AROS_SETVECADDRV0(abiv0LayersBase,  31, (APTR32)(IPTR)proxy_unhandledLVO31);
    __AROS_SETVECADDRV0(abiv0LayersBase,  32, (APTR32)(IPTR)proxy_unhandledLVO32);
    __AROS_SETVECADDRV0(abiv0LayersBase,  33, (APTR32)(IPTR)proxy_unhandledLVO33);
    __AROS_SETVECADDRV0(abiv0LayersBase,  34, (APTR32)(IPTR)proxy_unhandledLVO34);
    __AROS_SETVECADDRV0(abiv0LayersBase,  35, (APTR32)(IPTR)proxy_unhandledLVO35);
    __AROS_SETVECADDRV0(abiv0LayersBase,  37, (APTR32)(IPTR)proxy_unhandledLVO37);
    __AROS_SETVECADDRV0(abiv0LayersBase,  38, (APTR32)(IPTR)proxy_unhandledLVO38);
    __AROS_SETVECADDRV0(abiv0LayersBase,  39, (APTR32)(IPTR)proxy_unhandledLVO39);
    __AROS_SETVECADDRV0(abiv0LayersBase,  40, (APTR32)(IPTR)proxy_unhandledLVO40);
    __AROS_SETVECADDRV0(abiv0LayersBase,  41, (APTR32)(IPTR)proxy_unhandledLVO41);
    __AROS_SETVECADDRV0(abiv0LayersBase,  42, (APTR32)(IPTR)proxy_unhandledLVO42);
    __AROS_SETVECADDRV0(abiv0LayersBase,  43, (APTR32)(IPTR)proxy_unhandledLVO43);
    __AROS_SETVECADDRV0(abiv0LayersBase,  44, (APTR32)(IPTR)proxy_unhandledLVO44);
    __AROS_SETVECADDRV0(abiv0LayersBase,  45, (APTR32)(IPTR)proxy_unhandledLVO45);
}
