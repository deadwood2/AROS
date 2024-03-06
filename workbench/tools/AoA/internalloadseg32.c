/*
    Copyright (C) 1995-2020, The AROS Development Team. All rights reserved.

    Desc: DOS function InternalLoadSeg()
*/

#include <aros/debug.h>

#include <proto/dos.h>

#include <dos/dos.h>
#include <dos/dosextens.h>

#include "dos_intern.h"
#include "internalloadseg.h"

BPTR InternalLoadSeg32(BPTR fh, BPTR table, LONG_FUNC *funcarray, LONG *stack, struct DosLibrary *DOSBase)
{
    typedef struct _segfunc_t
    {
        ULONG id;
        BPTR (*func)(BPTR, BPTR, SIPTR *, LONG *, struct DosLibrary *);
        D(CONST_STRPTR format;)
    } segfunc_t;

    #define SEGFUNC(id, format) {id, InternalLoadSeg_##format D(, (STRPTR)#format)}
    
    static const segfunc_t funcs[] =
    {
        SEGFUNC(0x7f454c46, ELF)
    };
  
    BPTR segs = 0;

    if (fh)
    {
        UBYTE i;
        const UBYTE num_funcs = sizeof(funcs) / sizeof(funcs[0]);
        ULONG id;
        LONG len;

        len = ilsRead(fh, &id, sizeof(id));
        if (len == sizeof(id)) {
            id = AROS_BE2LONG(id);
            for (i = 0; i < num_funcs; i++) {
                if (funcs[i].id == id) {
                    segs = (*funcs[i].func)(fh, BNULL, (SIPTR *)funcarray,
                        stack, DOSBase);
                    D(bug("[InternalLoadSeg] %s loading %p as an %s object.\n",
                        segs ? "Succeeded" : "FAILED", fh, funcs[i].format));
                    if (segs)
                        return segs;
                    else
                        break;
                }
            }
        }
    }

    /* This routine can be called from partition.library, when
     * DOSBase is NULL, from HDToolbox, on m68k, when adding
     * a new device to HDToolbox.
     *
     * Luckily, we're not trying to read ELF, which has a
     * mess of SetIoErr() calls in it.
     */
    if (DOSBase != NULL)
        SetIoErr(ERROR_NOT_EXECUTABLE);

    return BNULL;
  
} /* InternalLoadSeg */

int read_block(BPTR file, APTR buffer, ULONG size, SIPTR * funcarray, struct DosLibrary * DOSBase)
{
  LONG subsize;
  UBYTE *buf=(UBYTE *)buffer;

  while(size)
  {
    subsize = ilsRead(file, buf, size);
    if(subsize==0)
    {
      if (DOSBase) {
          struct Process *me = (struct Process *)FindTask(NULL);
          ASSERT_VALID_PROCESS(me);
          me->pr_Result2=ERROR_BAD_HUNK;
      }
      return 1;
    }

    if(subsize<0)
      return 1;
    buf  +=subsize;
    size -=subsize;
  }
  return 0;
}

APTR _ilsAllocVec(SIPTR *funcarray, ULONG size, ULONG req)
{
    UBYTE *p = ilsAllocMem(size, req);

    D(bug("allocmem %p %d\n", p, size));
    if (!p)
        return NULL;
        
    /* Note that the result is ULONG-aligned even on 64 bits! */
    *((ULONG*)p) = (ULONG)size;
    return p + sizeof(ULONG);
}

void _ilsFreeVec(SIPTR *funcarray, void *buf)
{
    UBYTE *p = (UBYTE*)buf;
    ULONG size;
    if (!buf)
        return;
    p -= sizeof(ULONG);
    size = ((ULONG*)p)[0];
    D(bug("freemem %p %d\n", p, size));
    if (!size)
        return;

    ilsFreeMem(p, size);
}
