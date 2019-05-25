/*
    Copyright © 1995-2018, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Implements AROS's generic/amiga-like boot sequence.
    Lang: english
*/

#include <aros/debug.h>
#include <exec/alerts.h>
#include <exec/libraries.h>
#include <exec/devices.h>
#include <exec/execbase.h>
#include <aros/libcall.h>
#include <aros/asmcall.h>
#include <dos/dosextens.h>
#include <dos/cliinit.h>
#include <dos/stdio.h>
#include <utility/tagitem.h>
#include <libraries/expansionbase.h>
#include <proto/exec.h>
#include <proto/dos.h>

#include "dos_intern.h"
#include "../dosboot/bootflags.h"

#ifdef __mc68000
/*
 * Load DEVS:system-configuration only on m68k.
 * Setup pre-2.0 boot disk colors and mouse cursors (for example)
 */
#define USE_SYSTEM_CONFIGURATION

#endif

#ifdef USE_SYSTEM_CONFIGURATION

#include <proto/intuition.h>

static void load_system_configuration(struct DosLibrary *DOSBase)
{
    BPTR fh;
    ULONG len;
    struct Preferences prefs;
    struct Library *IntuitionBase;

    fh = Open("DEVS:system-configuration", MODE_OLDFILE);
    if (!fh)
        return;
    len = Read(fh, &prefs, sizeof prefs);
    Close(fh);
    if (len != sizeof prefs)
        return;
    IntuitionBase = TaggedOpenLibrary(TAGGEDOPEN_INTUITION);
    if (IntuitionBase)
        SetPrefs(&prefs, len, FALSE);
    CloseLibrary(IntuitionBase);
}

#else

#define load_system_configuration(DOSBase) do { } while (0)

#endif

extern void BCPL_cliInit(void);

struct ARPSMsg
{
    struct Message arps_Msg;
    VOID (*arps_Target)(APTR, APTR);
    /* Private fields follow */
};

static VOID __program_trampoline()
{
    struct MsgPort *startup = CreateMsgPort();
    startup->mp_Node.ln_Name = "ARPS"; /* AxRuntime Program Startup */
    AddPort(startup);

    WaitPort(startup);
    struct ARPSMsg *msg = (struct ARPSMsg *)GetMsg(startup);
    msg->arps_Target(SysBase, msg);
}

extern const struct Resident Dos_resident;
__attribute__((visibility("default"))) APTR __get_resident()
{
    return (struct Resident *)&Dos_resident;
}

void __dos_Boot(struct DosLibrary *DOSBase, ULONG BootFlags, UBYTE Flags)
{
    BPTR cis = BNULL;

    /*  We have been created as a process by DOS, we should now
        try and boot the system. */

    D(
        bug("[DOS] %s: ** starting generic boot sequence\n", __func__);
        bug("[DOS] %s: BootFlags 0x%08X Flags 0x%02X\n", __func__, BootFlags, Flags);
        bug("[DOS] %s: DOSBase @ 0x%p\n", __func__, DOSBase);
      )

    /* m68000 uses this to get the default colors and
     * cursors for Workbench
     */
    load_system_configuration(DOSBase);

    D(bug("[DOS] %s: system config loaded\n", __func__);)

    /*
     * If needed, run the display drivers loader.
     * In fact the system must have at least one resident driver,
     * which will be used for bootmenu etc. However, it we somehow happen
     * not to have it, this will be our last chance.
     */
    if ((BootFlags & (BF_NO_DISPLAY_DRIVERS | BF_NO_COMPOSITION)) != (BF_NO_DISPLAY_DRIVERS | BF_NO_COMPOSITION))
    {
        /* Check that it exists first... */
        BPTR seg;

        D(bug("[DOS] %s: initialising displays\n", __func__);)

        if ((seg = LoadSeg("C:AROSMonDrvs")) != BNULL)
        {
            STRPTR args = "";
            BPTR oldin, oldout;

            /*
             * Argument strings MUST contain terminating LF because of ReadItem() bugs.
             * Their absence causes ReadArgs() crash.
             */
            if (BootFlags & BF_NO_COMPOSITION)
                args = "NOCOMPOSITION\n";
            else if (BootFlags & BF_NO_DISPLAY_DRIVERS)
                args = "ONLYCOMPOSITION\n";

            D(bug("[DOS] %s: Running AROSMonDrvs %s\n", __func__, args);)

            /* RunCommand needs a valid Input() handle
             * for passing in its arguments.
             */
            oldin = SelectInput(Open("NIL:", MODE_OLDFILE));
            oldout= SelectOutput(Open("NIL:", MODE_NEWFILE));
            RunCommand(seg, AROS_STACKSIZE, args, strlen(args));
            SelectInput(oldin);
            SelectOutput(oldout);

            /* We don't care about the return code */
            UnLoadSeg(seg);
        }
    }

    D(bug("[DOS] %s: preparing console\n", __func__);)

    if (BootFlags & BF_EMERGENCY_CONSOLE) {
        D(bug("[DOS] %s:     (emergency console)\n", __func__);)
        BootFlags |= BF_NO_STARTUP_SEQUENCE;
        cis = Open("ECON:", MODE_OLDFILE);
    }

    (VOID)cis;
    CreateNewProcTags(
                        NP_Entry,       (IPTR)__program_trampoline,
                        NP_Name,        (IPTR)"AxRuntime Program",
                        TAG_DONE);

}
