/* pci_aros.c - pci access abstraction for AROS by Chris Hodges
*/

#include <aros/symbolsets.h>
#include <exec/types.h>
#include <oop/oop.h>
#include <devices/timer.h>
#include <hidd/hidd.h>
#include <hidd/pci.h>
#include <hidd/usb.h>
#include <hidd/system.h>

#include <proto/oop.h>
#include <proto/utility.h>
#include <proto/exec.h>

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "uhwcmd.h"
#include "ohciproto.h"

#define NewList NEWLIST

#undef HiddPCIDeviceAttrBase
#undef HiddAttrBase
#undef HiddPCIDeviceBase

#define HiddPCIDeviceAttrBase (hd->hd_HiddPCIDeviceAB)
#define HiddAttrBase (hd->hd_HiddAB)
#define HiddPCIDeviceBase (hd->hd_HiddPCIDeviceMB)

static void handleQuirks(struct PCIController *hc)
{
    struct PCIDevice *hd = hc->hc_Device;
    IPTR vendorid, productid, revisionid, subvendorid, subproductid, memsize;

    hc->hc_Quirks = 0;
    if (hc->hc_HCIType == HCITYPE_EHCI)
        hc->hc_Quirks |= (HCQ_EHCI_OVERLAY_CTRL_FILL|HCQ_EHCI_OVERLAY_INT_FILL|HCQ_EHCI_OVERLAY_BULK_FILL);

    // Check for VirtualBox's EHCI (identify as precisely as possible to avoid
    // applying incorrect quirks to real Intel ICH6 EHCI)
    OOP_GetAttr(hc->hc_PCIDeviceObject, aHidd_PCIDevice_VendorID, &vendorid);
    OOP_GetAttr(hc->hc_PCIDeviceObject, aHidd_PCIDevice_ProductID, &productid);
    OOP_GetAttr(hc->hc_PCIDeviceObject, aHidd_PCIDevice_RevisionID, &revisionid);
    OOP_GetAttr(hc->hc_PCIDeviceObject, aHidd_PCIDevice_SubsystemVendorID, &subvendorid);
    OOP_GetAttr(hc->hc_PCIDeviceObject, aHidd_PCIDevice_SubsystemID, &subproductid);
    OOP_GetAttr(hc->hc_PCIDeviceObject, aHidd_PCIDevice_Size0, &memsize);
    if (vendorid == 0x8086 && productid == 0x265c && revisionid == 0
        && subvendorid == 0 && subproductid == 0 && memsize == 4096)
    {
        // This is needed for EHCI to work in VirtualBox
        hc->hc_Quirks &= ~(HCQ_EHCI_OVERLAY_CTRL_FILL|HCQ_EHCI_OVERLAY_INT_FILL|HCQ_EHCI_OVERLAY_BULK_FILL);

        // VirtualBox reports frame list size of 1024, but still issues interrupts at
        // speed of around 4 per second instead of every 1024 ms
        hc->hc_Quirks |= HCQ_EHCI_VBOX_FRAMEROOLOVER;
    }
    else if (vendorid == 0x9710)
    {
        /* Apply MosChip frame-counter register bug workaround */
        hc->hc_Quirks |= HCQ_EHCI_MOSC_FRAMECOUNTBUG;
    }   
}

AROS_UFH3(void, pciEnumerator,
          AROS_UFHA(struct Hook *, hook, A0),
          AROS_UFHA(OOP_Object *, pciDevice, A2),
          AROS_UFHA(APTR, message, A1))
{
    AROS_USERFUNC_INIT

    struct PCIDevice *hd = (struct PCIDevice *) hook->h_Data;
    struct PCIController *hc;
    IPTR hcitype;
    IPTR bus;
    IPTR dev;
    IPTR sub;
    IPTR intline;
    ULONG devid;

    OOP_GetAttr(pciDevice, aHidd_PCIDevice_Interface, &hcitype);
    OOP_GetAttr(pciDevice, aHidd_PCIDevice_Bus, &bus);
    OOP_GetAttr(pciDevice, aHidd_PCIDevice_Dev, &dev);
    OOP_GetAttr(pciDevice, aHidd_PCIDevice_Sub, &sub);
    OOP_GetAttr(pciDevice, aHidd_PCIDevice_INTLine, &intline);

    devid = (bus<<16)|dev;

    KPRINTF(10, ("Found PCI device 0x%lx of type %ld, Intline=%ld\n", devid, hcitype, intline));

    if(intline == 255)
    {
        // we can't work without the correct interrupt line
        // BIOS needs plug & play os option disabled. Alternatively AROS must support APIC reconfiguration
        KPRINTF(200, ("ERROR: PCI card has no interrupt line assigned by BIOS, disable Plug & Play OS!\n"));
    }
    else
    {
        switch (hcitype)
        {
        case HCITYPE_OHCI:
        case HCITYPE_EHCI:
        case HCITYPE_UHCI:
            KPRINTF(10, ("Setting up device...\n"));

            hc = AllocPooled(hd->hd_MemPool, sizeof(struct PCIController));
            if (hc)
            {
                hc->hc_Device = hd;
                hc->hc_DevID = devid;
                hc->hc_FunctionNum = sub;
                hc->hc_HCIType = hcitype;
                hc->hc_PCIDeviceObject = pciDevice;
                hc->hc_PCIIntLine = intline;

                OOP_GetAttr(pciDevice, aHidd_PCIDevice_Driver, (IPTR *) &hc->hc_PCIDriverObject);

                NewList(&hc->hc_CtrlXFerQueue);
                NewList(&hc->hc_IntXFerQueue);
                NewList(&hc->hc_IsoXFerQueue);
                NewList(&hc->hc_BulkXFerQueue);
                NewList(&hc->hc_TDQueue);
                NewList(&hc->hc_AbortQueue);
                NewList(&hc->hc_PeriodicTDQueue);
                NewList(&hc->hc_OhciRetireQueue);
                AddTail(&hd->hd_TempHCIList, &hc->hc_Node);

                handleQuirks(hc);
            }
            break;

        default:
            KPRINTF(10, ("Unsupported HCI type %ld\n", hcitype));
        }
    }

    AROS_USERFUNC_EXIT
}

/* /// "pciInit()" */
BOOL pciInit(struct PCIDevice *hd)
{
    OOP_Object                  *root;
    OOP_Class	       *usbContrClass;
    struct PCIController *hc;
    struct PCIController *nexthc;
    struct PCIUnit *hu;
    ULONG unitno = 0;

    KPRINTF(10, ("*** pciInit(%p) ***\n", hd));

    NewList(&hd->hd_TempHCIList);

    if((hd->hd_PCIHidd = OOP_NewObject(NULL, (STRPTR) CLID_Hidd_PCI, NULL)))
    {
        struct TagItem tags[] =
        {
            { tHidd_PCI_Class,      (PCI_CLASS_SERIAL_USB>>8) & 0xff },
            { tHidd_PCI_SubClass,   (PCI_CLASS_SERIAL_USB & 0xff) },
            { TAG_DONE, 0UL }
        };

        struct OOP_ABDescr attrbases[] =
        {
            { (STRPTR) IID_Hidd,            &hd->hd_HiddAB },
            { (STRPTR) IID_Hidd_PCIDevice,  &hd->hd_HiddPCIDeviceAB },
            { NULL, NULL }
        };

        struct Hook findHook =
        {
             h_Entry:        (IPTR (*)()) pciEnumerator,
             h_Data:         hd,
        };

        OOP_ObtainAttrBases(attrbases);
        hd->hd_HiddPCIDeviceMB = OOP_GetMethodID(IID_Hidd_PCIDevice, 0);

        KPRINTF(20, ("Searching for devices...\n"));

        HIDD_PCI_EnumDevices(hd->hd_PCIHidd, &findHook, (struct TagItem *) &tags);
    } else {
        KPRINTF(20, ("Unable to create PCIHidd object!\n"));
        return FALSE;
    }

    root = OOP_NewObject(NULL, CLID_Hidd_System, NULL);
    if (!root)
        root = OOP_NewObject(NULL, CLID_HW_Root, NULL);
    KPRINTF(20, ("HW Root @  0x%p\n", root));
    usbContrClass = OOP_FindClass(CLID_Hidd_USBController);
    KPRINTF(20, ("USB Controller class @  0x%p\n", usbContrClass));

    // Create units with a list of host controllers having the same bus and device number.
    while(hd->hd_TempHCIList.lh_Head->ln_Succ)
    {
        BOOL    unitdone = FALSE, unithasv1 = FALSE, unithasv2 = FALSE;
#if defined(TMPXHCICODE)
        BOOL    unithasv3 = FALSE;
#endif

        hu = AllocPooled(hd->hd_MemPool, sizeof(struct PCIUnit));
        if(!hu)
        {
            // actually, we should get rid of the allocated memory first, but I don't care as DeletePool() will take care of this eventually
            return FALSE;
        }
        hu->hu_Device = hd;
        hu->hu_UnitNo = unitno;

        NewList(&hu->hu_Controllers);
        NewList(&hu->hu_RHIOQueue);

        hu->hu_DevID = (ULONG)-1;
        ForeachNodeSafe(&hd->hd_TempHCIList, hc, nexthc)
        {
            if (hu->hu_DevID == (ULONG)-1)
                hu->hu_DevID = hc->hc_DevID;

            if (hc->hc_DevID == hu->hu_DevID)
            {
                BOOL ignore = FALSE;
                switch (hc->hc_HCIType)
                {
                    case HCITYPE_UHCI:
                    case HCITYPE_OHCI:
                        if (unithasv2)
                            unitdone = TRUE;
                        unithasv1 = TRUE;
                        break;
                    case HCITYPE_EHCI:
                        if (unithasv2)
                            unitdone = TRUE;
                        unithasv2 = TRUE;
                        break;
 #if defined(TMPXHCICODE)
                    case HCITYPE_XHCI:
                        if ((unithasv1) || (unithasv2))
                            unitdone = TRUE;
                        if ((!(hd->hd_Flags & HDF_ENABLEXHCI)) || unithasv3)
                            ignore = TRUE;
                        unithasv3 = TRUE;
                        break;
 #endif
                }
                if (unitdone)
                    break;

                if (ignore)
                    continue;

                Remove(&hc->hc_Node);

                if ((usbContrClass) && (root))
                {
                    struct TagItem usbc_tags[] =
                    {
                        {aHidd_Name,                0       },
                        {aHidd_HardwareName,        0       },
                        {aHidd_Producer,            0       },
                #define USB_TAG_VEND 2
                        {aHidd_Product,             0       },
                #define USB_TAG_PROD 3
                        {aHidd_DriverData,          0       },
                #define USB_TAG_DATA 4
                        {TAG_DONE,                  0       }
                    };

                    hc->hc_Node.ln_Name = AllocVec(16, MEMF_CLEAR);
                    sprintf(hc->hc_Node.ln_Name, "pciusb.device/%u", hu->hu_UnitNo);
                    usbc_tags[0].ti_Data = (IPTR)hc->hc_Node.ln_Name;

                    usbc_tags[USB_TAG_VEND].ti_Data = 0;
                    usbc_tags[USB_TAG_PROD].ti_Data = hu->hu_DevID;

                    switch (hc->hc_HCIType)
                    {
                    case HCITYPE_UHCI:
                        {
                            usbc_tags[1].ti_Data = (IPTR)"PCI USB 1.x UHCI Host controller";
                            break;
                        }

                    case HCITYPE_OHCI:
                        {
                            usbc_tags[1].ti_Data = (IPTR)"PCI USB 1.1 OHCI Host controller";
                            break;
                        }

                    case HCITYPE_EHCI:
                        {
                            usbc_tags[1].ti_Data = (IPTR)"PCI USB 2.0 EHCI Host controller";
                            break;
                        }
                    }
                    HW_AddDriver(root, usbContrClass, usbc_tags);
                }
                hc->hc_Unit = hu;
                AddTail(&hu->hu_Controllers, &hc->hc_Node);
            }
        }
        AddTail(&hd->hd_Units, (struct Node *) hu);
        unitno++;
    }
    return TRUE;
}
/* \\\ */

/* /// "PCIXReadConfigByte()" */
UBYTE PCIXReadConfigByte(struct PCIController *hc, UBYTE offset)
{
    struct PCIDevice *hd = hc->hc_Device;

    return HIDD_PCIDevice_ReadConfigByte(hc->hc_PCIDeviceObject, offset);
}
/* \\\ */

/* /// "PCIXReadConfigWord()" */
UWORD PCIXReadConfigWord(struct PCIController *hc, UBYTE offset)
{
    struct PCIDevice *hd = hc->hc_Device;

    return HIDD_PCIDevice_ReadConfigWord(hc->hc_PCIDeviceObject, offset);
}
/* \\\ */

/* /// "PCIXReadConfigLong()" */
ULONG PCIXReadConfigLong(struct PCIController *hc, UBYTE offset)
{
    struct PCIDevice *hd = hc->hc_Device;

    return HIDD_PCIDevice_ReadConfigLong(hc->hc_PCIDeviceObject, offset);
}
/* \\\ */

/* /// "PCIXWriteConfigByte()" */
void PCIXWriteConfigByte(struct PCIController *hc, ULONG offset, UBYTE value)
{
    struct PCIDevice *hd = hc->hc_Device;

    HIDD_PCIDevice_WriteConfigByte(hc->hc_PCIDeviceObject, offset, value);
}
/* \\\ */

/* /// "PCIXWriteConfigWord()" */
void PCIXWriteConfigWord(struct PCIController *hc, ULONG offset, UWORD value)
{
    struct PCIDevice *hd = hc->hc_Device;

    HIDD_PCIDevice_WriteConfigWord(hc->hc_PCIDeviceObject, offset, value);
}
/* \\\ */

/* /// "PCIXWriteConfigLong()" */
void PCIXWriteConfigLong(struct PCIController *hc, ULONG offset, ULONG value)
{
    struct PCIDevice *hd = hc->hc_Device;

    HIDD_PCIDevice_WriteConfigLong(hc->hc_PCIDeviceObject, offset, value);
}
/* \\\ */

BOOL PCIXAddInterrupt(struct PCIController *hc, struct Interrupt *interrupt)
{
    struct PCIDevice *hd = hc->hc_Device;

    return HIDD_PCIDevice_AddInterrupt(hc->hc_PCIDeviceObject, interrupt);
}

/* /// "pciStrcat()" */
void pciStrcat(STRPTR d, STRPTR s)
{
    while(*d) d++;
    while((*d++ = *s++));
}
/* \\\ */

/* /// "pciAllocUnit()" */
BOOL pciAllocUnit(struct PCIUnit *hu)
{
    struct PCIDevice *hd = hu->hu_Device;
    struct PCIController *hc;

    BOOL allocgood = TRUE;
    ULONG usb11ports = 0;
    ULONG usb20ports = 0;
    ULONG cnt;

    ULONG ohcicnt = 0;
    ULONG uhcicnt = 0;
    ULONG ehcicnt = 0;

    STRPTR prodname;

    KPRINTF(10, ("*** pciAllocUnit(%p) ***\n", hu));

    hc = (struct PCIController *) hu->hu_Controllers.lh_Head;
    while(hc->hc_Node.ln_Succ)
    {
        CONST_STRPTR owner;
        
        owner = HIDD_PCIDevice_Obtain(hc->hc_PCIDeviceObject, hd->hd_Library.lib_Node.ln_Name);
        if (!owner)
            hc->hc_Flags |= HCF_ALLOCATED;
        else
        {
            KPRINTF(20, ("Couldn't allocate board, already allocated by %s\n", owner));
            allocgood = FALSE;
        }

        hc = (struct PCIController *) hc->hc_Node.ln_Succ;
    }

    if(allocgood)
    {
        // allocate necessary memory
        hc = (struct PCIController *) hu->hu_Controllers.lh_Head;
        while(hc->hc_Node.ln_Succ)
        {
            switch(hc->hc_HCIType)
            {
                case HCITYPE_UHCI:
                {
                    allocgood = uhciInit(hc,hu);
                    if(allocgood) {
                        uhcicnt++;
                    }
                    break;
                }

                case HCITYPE_OHCI:
                {
                    allocgood = ohciInit(hc,hu);
                    if(allocgood) {
                        ohcicnt++;
                    }
                    break;
                }

                case HCITYPE_EHCI:
                {
                    allocgood = ehciInit(hc,hu);
                    if(allocgood) {
                        ehcicnt++;
                        if(usb20ports) {
                            KPRINTF(200, ("WARNING: More than one EHCI controller per board?!?\n"));
                        }
                        usb20ports = hc->hc_NumPorts;

                        for(cnt = 0; cnt < usb20ports; cnt++) {
                            hu->hu_PortMap20[cnt] = hc;
                            hc->hc_PortNum20[cnt] = cnt;
                        }
                    }
                    break;
                }
            }
            hc = (struct PCIController *) hc->hc_Node.ln_Succ;
        }
    }

    if(!allocgood)
    {
        // free previously allocated boards
        hc = (struct PCIController *) hu->hu_Controllers.lh_Head;
        while(hc->hc_Node.ln_Succ)
        {
            if (hc->hc_Flags & HCF_ALLOCATED)
            {
                hc->hc_Flags &= ~HCF_ALLOCATED;
                HIDD_PCIDevice_Release(hc->hc_PCIDeviceObject);
            }

            hc = (struct PCIController *) hc->hc_Node.ln_Succ;
        }
        return FALSE;
    }

    hc = (struct PCIController *) hu->hu_Controllers.lh_Head;
    while(hc->hc_Node.ln_Succ)
    {
        if((hc->hc_HCIType == HCITYPE_UHCI) || (hc->hc_HCIType == HCITYPE_OHCI))
        {
            if(hc->hc_complexrouting)
            {
                ULONG locport = 0;
                for(cnt = 0; cnt < usb20ports; cnt++)
                {
                    if(((hc->hc_portroute >> (cnt<<2)) & 0xf) == hc->hc_FunctionNum)
                    {
                        KPRINTF(10, ("CHC %ld Port %ld assigned to global Port %ld\n", hc->hc_FunctionNum, locport, cnt));
                        hu->hu_PortMap11[cnt] = hc;
                        hu->hu_PortNum11[cnt] = locport;
                        hc->hc_PortNum20[locport] = cnt;
                        locport++;
                    }
                }
            } else {
                for(cnt = usb11ports; cnt < usb11ports + hc->hc_NumPorts; cnt++)
                {
                    hu->hu_PortMap11[cnt] = hc;
                    hu->hu_PortNum11[cnt] = cnt - usb11ports;
                    hc->hc_PortNum20[cnt - usb11ports] = cnt;
                }
            }
            usb11ports += hc->hc_NumPorts;
        }
        hc = (struct PCIController *) hc->hc_Node.ln_Succ;
    }
    if((usb11ports != usb20ports) && usb20ports)
    {
        KPRINTF(20, ("Warning! #EHCI Ports (%ld) does not match USB 1.1 Ports (%ld)!\n", usb20ports, usb11ports));
    }

    hu->hu_RootHub11Ports = usb11ports;
    hu->hu_RootHub20Ports = usb20ports;
    hu->hu_RootHubPorts = (usb11ports > usb20ports) ? usb11ports : usb20ports;

    for(cnt = 0; cnt < hu->hu_RootHubPorts; cnt++)
    {
        hu->hu_EhciOwned[cnt] = hu->hu_PortMap20[cnt] ? TRUE : FALSE;
    }

    KPRINTF(10, ("Unit %ld: USB Board %08lx has %ld USB1.1 and %ld USB2.0 ports!\n", hu->hu_UnitNo, hu->hu_DevID, hu->hu_RootHub11Ports, hu->hu_RootHub20Ports));

    hu->hu_FrameCounter = 1;
    hu->hu_RootHubAddr = 0;

    // put em online
    hc = (struct PCIController *) hu->hu_Controllers.lh_Head;
    while(hc->hc_Node.ln_Succ)
    {
        hc->hc_Flags |= HCF_ONLINE;
        hc = (struct PCIController *) hc->hc_Node.ln_Succ;
    }

    // create product name of device
    prodname = hu->hu_ProductName;
    *prodname = 0;
    pciStrcat(prodname, "PCI ");
    if(ohcicnt + uhcicnt)
    {
        if(ohcicnt + uhcicnt >1)
        {
            prodname[4] = ohcicnt + uhcicnt + '0';
            prodname[5] = 'x';
            prodname[6] = 0;
        }
        pciStrcat(prodname, ohcicnt ? "OHCI" : "UHCI");
        if(ehcicnt)
        {
            pciStrcat(prodname, " +");
        } else{
            pciStrcat(prodname, " USB 1.1");
        }
    }
    if(ehcicnt)
    {
        pciStrcat(prodname, " EHCI USB 2.0");
    }
#if 0 // user can use pcitool to check what the chipset is and not guess it from this
    pciStrcat(prodname, " Host Controller (");
    if(ohcicnt + uhcicnt)
    {
        pciStrcat(prodname, ohcicnt ? "NEC)" : "VIA, Intel, ALI, etc.)");
    } else {
                pciStrcat(prodname, "Emulated?)");
        }
#else
    pciStrcat(prodname, " Host Controller");
#endif
    KPRINTF(10, ("Unit allocated!\n"));

    return TRUE;
}
/* \\\ */

/* /// "pciFreeUnit()" */
void pciFreeUnit(struct PCIUnit *hu)
{
    struct PCIDevice *hd = hu->hu_Device;
    struct PCIController *hc;

    struct TagItem pciDeactivate[] =
    {
            { aHidd_PCIDevice_isIO,     FALSE },
            { aHidd_PCIDevice_isMEM,    FALSE },
            { aHidd_PCIDevice_isMaster, FALSE },
            { TAG_DONE, 0UL },
    };

    KPRINTF(10, ("*** pciFreeUnit(%p) ***\n", hu));

    // put em offline
    hc = (struct PCIController *) hu->hu_Controllers.lh_Head;
    while(hc->hc_Node.ln_Succ)
    {
        hc->hc_Flags &= ~HCF_ONLINE;
        hc = (struct PCIController *) hc->hc_Node.ln_Succ;
    }

    // doing this in three steps to avoid these damn host errors
    ehciFree(hc, hu);
    ohciFree(hc, hu);
    uhciFree(hc, hu);

    //FIXME: (x/e/o/u)hciFree routines actually ONLY stops the chip NOT free anything as below...
    hc = (struct PCIController *) hu->hu_Controllers.lh_Head;
    while(hc->hc_Node.ln_Succ) {
        if(hc->hc_PCIMem) {
            HIDD_PCIDriver_FreePCIMem(hc->hc_PCIDriverObject, hc->hc_PCIMem);
            hc->hc_PCIMem = NULL;
        }
        hc = (struct PCIController *) hc->hc_Node.ln_Succ;
    }

    // disable and free board
    hc = (struct PCIController *) hu->hu_Controllers.lh_Head;
    while(hc->hc_Node.ln_Succ)
    {
        OOP_SetAttrs(hc->hc_PCIDeviceObject, (struct TagItem *) pciDeactivate); // deactivate busmaster and IO/Mem
        if(hc->hc_PCIIntHandler.is_Node.ln_Name)
        {
            HIDD_PCIDevice_RemoveInterrupt(hc->hc_PCIDeviceObject, &hc->hc_PCIIntHandler);
            hc->hc_PCIIntHandler.is_Node.ln_Name = NULL;
        }

        hc->hc_Flags &= ~HCF_ALLOCATED;
        HIDD_PCIDevice_Release(hc->hc_PCIDeviceObject);
        hc = (struct PCIController *) hc->hc_Node.ln_Succ;
    }
}
/* \\\ */

/* /// "pciExpunge()" */
void pciExpunge(struct PCIDevice *hd)
{
    struct PCIController *hc;
    struct PCIUnit *hu;

    KPRINTF(10, ("*** pciExpunge(%p) ***\n", hd));

    hu = (struct PCIUnit *) hd->hd_Units.lh_Head;
    while(((struct Node *) hu)->ln_Succ)
    {
        Remove((struct Node *) hu);
        hc = (struct PCIController *) hu->hu_Controllers.lh_Head;
        while(hc->hc_Node.ln_Succ)
        {
            Remove(&hc->hc_Node);
            FreePooled(hd->hd_MemPool, hc, sizeof(struct PCIController));
            hc = (struct PCIController *) hu->hu_Controllers.lh_Head;
        }
        FreePooled(hd->hd_MemPool, hu, sizeof(struct PCIUnit));
        hu = (struct PCIUnit *) hd->hd_Units.lh_Head;
    }
    if(hd->hd_PCIHidd)
    {
        struct OOP_ABDescr attrbases[] =
        {
            { (STRPTR) IID_Hidd,            &hd->hd_HiddAB },
            { (STRPTR) IID_Hidd_PCIDevice,  &hd->hd_HiddPCIDeviceAB },
            { NULL, NULL }
        };

        OOP_ReleaseAttrBases(attrbases);

        OOP_DisposeObject(hd->hd_PCIHidd);
    }
}
/* \\\ */

/* /// "pciGetPhysical()" */
APTR pciGetPhysical(struct PCIController *hc, APTR virtaddr)
{
    //struct PCIDevice *hd = hc->hc_Device;
    return(HIDD_PCIDriver_CPUtoPCI(hc->hc_PCIDriverObject, virtaddr));
}
/* \\\ */
