#include "include/aros/cpu.h"
#include "include/aros/proxy.h"
#include "include/exec/structures.h"

#include "support.h"

static void abiv0_unhandledLVO5() { unhandledLibraryFunction("cybergraphics.library", 5); }
MAKE_PROXY_ARG_1(unhandledLVO5)
static void abiv0_unhandledLVO6() { unhandledLibraryFunction("cybergraphics.library", 6); }
MAKE_PROXY_ARG_1(unhandledLVO6)
static void abiv0_unhandledLVO7() { unhandledLibraryFunction("cybergraphics.library", 7); }
MAKE_PROXY_ARG_1(unhandledLVO7)
static void abiv0_unhandledLVO8() { unhandledLibraryFunction("cybergraphics.library", 8); }
MAKE_PROXY_ARG_1(unhandledLVO8)
static void abiv0_unhandledLVO11() { unhandledLibraryFunction("cybergraphics.library", 11); }
MAKE_PROXY_ARG_1(unhandledLVO11)
static void abiv0_unhandledLVO12() { unhandledLibraryFunction("cybergraphics.library", 12); }
MAKE_PROXY_ARG_1(unhandledLVO12)
static void abiv0_unhandledLVO13() { unhandledLibraryFunction("cybergraphics.library", 13); }
MAKE_PROXY_ARG_1(unhandledLVO13)
static void abiv0_unhandledLVO14() { unhandledLibraryFunction("cybergraphics.library", 14); }
MAKE_PROXY_ARG_1(unhandledLVO14)
static void abiv0_unhandledLVO15() { unhandledLibraryFunction("cybergraphics.library", 15); }
MAKE_PROXY_ARG_1(unhandledLVO15)
static void abiv0_unhandledLVO18() { unhandledLibraryFunction("cybergraphics.library", 18); }
MAKE_PROXY_ARG_1(unhandledLVO18)
static void abiv0_unhandledLVO22() { unhandledLibraryFunction("cybergraphics.library", 22); }
MAKE_PROXY_ARG_1(unhandledLVO22)
static void abiv0_unhandledLVO23() { unhandledLibraryFunction("cybergraphics.library", 23); }
MAKE_PROXY_ARG_1(unhandledLVO23)
static void abiv0_unhandledLVO24() { unhandledLibraryFunction("cybergraphics.library", 24); }
MAKE_PROXY_ARG_1(unhandledLVO24)
static void abiv0_unhandledLVO26() { unhandledLibraryFunction("cybergraphics.library", 26); }
MAKE_PROXY_ARG_1(unhandledLVO26)
static void abiv0_unhandledLVO27() { unhandledLibraryFunction("cybergraphics.library", 27); }
MAKE_PROXY_ARG_1(unhandledLVO27)
static void abiv0_unhandledLVO28() { unhandledLibraryFunction("cybergraphics.library", 28); }
MAKE_PROXY_ARG_1(unhandledLVO28)
static void abiv0_unhandledLVO29() { unhandledLibraryFunction("cybergraphics.library", 29); }
MAKE_PROXY_ARG_1(unhandledLVO29)
static void abiv0_unhandledLVO30() { unhandledLibraryFunction("cybergraphics.library", 30); }
MAKE_PROXY_ARG_1(unhandledLVO30)
static void abiv0_unhandledLVO31() { unhandledLibraryFunction("cybergraphics.library", 31); }
MAKE_PROXY_ARG_1(unhandledLVO31)
static void abiv0_unhandledLVO32() { unhandledLibraryFunction("cybergraphics.library", 32); }
MAKE_PROXY_ARG_1(unhandledLVO32)
static void abiv0_unhandledLVO34() { unhandledLibraryFunction("cybergraphics.library", 34); }
MAKE_PROXY_ARG_1(unhandledLVO34)
static void abiv0_unhandledLVO35() { unhandledLibraryFunction("cybergraphics.library", 35); }
MAKE_PROXY_ARG_1(unhandledLVO35)
static void abiv0_unhandledLVO37() { unhandledLibraryFunction("cybergraphics.library", 37); }
MAKE_PROXY_ARG_1(unhandledLVO37)

void CyberGfx_Unhandled_init(struct LibraryV0 *abiv0CyberGfxBase)
{
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  5, (APTR32)(IPTR)proxy_unhandledLVO5);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  6, (APTR32)(IPTR)proxy_unhandledLVO6);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  7, (APTR32)(IPTR)proxy_unhandledLVO7);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  8, (APTR32)(IPTR)proxy_unhandledLVO8);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  11, (APTR32)(IPTR)proxy_unhandledLVO11);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  12, (APTR32)(IPTR)proxy_unhandledLVO12);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  13, (APTR32)(IPTR)proxy_unhandledLVO13);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  14, (APTR32)(IPTR)proxy_unhandledLVO14);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  15, (APTR32)(IPTR)proxy_unhandledLVO15);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  18, (APTR32)(IPTR)proxy_unhandledLVO18);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  22, (APTR32)(IPTR)proxy_unhandledLVO22);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  23, (APTR32)(IPTR)proxy_unhandledLVO23);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  24, (APTR32)(IPTR)proxy_unhandledLVO24);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  26, (APTR32)(IPTR)proxy_unhandledLVO26);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  27, (APTR32)(IPTR)proxy_unhandledLVO27);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  28, (APTR32)(IPTR)proxy_unhandledLVO28);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  29, (APTR32)(IPTR)proxy_unhandledLVO29);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  30, (APTR32)(IPTR)proxy_unhandledLVO30);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  31, (APTR32)(IPTR)proxy_unhandledLVO31);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  32, (APTR32)(IPTR)proxy_unhandledLVO32);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  34, (APTR32)(IPTR)proxy_unhandledLVO34);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  35, (APTR32)(IPTR)proxy_unhandledLVO35);
    __AROS_SETVECADDRV0(abiv0CyberGfxBase,  37, (APTR32)(IPTR)proxy_unhandledLVO37);
}

