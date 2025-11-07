#ifndef INTUITION_CLASSALIGN_H
#define INTUITION_CLASSALIGN_H

/*
    Copyright  2025, The AROS Development Team. All rights reserved.
    $Id$

    Alignment rules for class data
*/

#ifndef AROS_CPU_H
#include <aros/cpu.h>
#endif

#if 0
// Instance data _must_ be aligned on SMP capable platforms, incase spinlocks are used.
#ifdef AROS_WORSTALIGN
#define CLASS_INSTANCE_ALIGN __attribute__((aligned(AROS_WORSTALIGN)))
#endif
#endif

#ifndef CLASS_INSTANCE_ALIGN 
#define CLASS_INSTANCE_ALIGN
#endif

#endif /* INTUITION_CLASSALIGN_H */
