#ifndef _AROS_CPU_H
#define _AROS_CPU_H

struct JumpVecV0
{
    int vec;
};

/* Use these to access a vector table */
#define LIB_VECTSIZEV0                  (sizeof (struct JumpVecV0))
#define __AROS_GETJUMPVECV0(lib,n)      (&((struct JumpVecV0 *)lib)[-(n)])
#define __AROS_GETVECADDRV0(lib,n)      (__AROS_GETJUMPVECV0(lib,n)->vec)
#define __AROS_SETVECADDRV0(lib,n,addr) (__AROS_GETJUMPVECV0(lib,n)->vec = (addr))
#define __AROS_INITVECV0(lib,n)         __AROS_SETVECADDRV0(lib,n,(APTR32)(IPTR)_aros_not_implemented)

#endif
