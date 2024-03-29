#ifndef I8042_INTERN_H
#define I8042_INTERN_H

#include <dos/bptr.h>
#include <exec/libraries.h>
#include <oop/oop.h>

#include <hidd/hidd.h>
#include <hidd/mouse.h>

/***** Common static data *******************/

struct kbd_staticdata
{
    OOP_Class          *kbdclass;
    OOP_Object         *kbdhidd;

    OOP_Class          *mouseclass;
    OOP_Object         *mousehidd;

    OOP_AttrBase        hiddAttrBase;
    OOP_AttrBase        hiddKbdAB;
    OOP_AttrBase        hiddMouseAB;
    OOP_MethodID        hwMethodBase;

    BPTR                cs_SegList;
    APTR                cs_KernelBase;
    struct Library     *cs_OOPBase;
    struct Library     *cs_UtilityBase;
    struct Interrupt    cs_ResetInt;
};

struct kbdbase
{
    struct Library library;    
    struct kbd_staticdata ksd;
};

/****************************************************************************************/

#define XSD(cl) (&((struct kbdbase *)cl->UserData)->ksd)

#undef HiddAttrBase
#undef HiddKbdAB
#undef HiddMouseAB
#undef HWBase
#define HiddAttrBase (XSD(cl)->hiddAttrBase)
#define HiddKbdAB    (XSD(cl)->hiddKbdAB)
#define HiddMouseAB  (XSD(cl)->hiddMouseAB)
#define HWBase       (XSD(cl)->hwMethodBase)

#define KernelBase  (XSD(cl)->cs_KernelBase)
#define OOPBase     (XSD(cl)->cs_OOPBase)
#define UtilityBase (XSD(cl)->cs_UtilityBase)

#endif
