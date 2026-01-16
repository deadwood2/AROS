/*
    Copyright (C) 2026, The AROS Development Team. All rights reserved.
*/

#include <exec/rawfmt.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/alib.h>
#include <aros/debug.h>

static LONG SafeEasyRequest(struct EasyStruct *es, struct IntuitionBase *IntuitionBase)
{
    LONG result;
    APTR req = BuildEasyRequestArgs(NULL, es, 0, NULL);

    if (!req)
    {
        /* Return 0 if requester creation failed. This makes us suspend. */
        NewRawDoFmt("*** Alert requester creation failed. Logged alert:\n%s\n", RAWFMTFUNC_SERIAL, NULL, es->es_TextFormat);
        return 0;
    }

    do
    {
        result = SysReqHandler(req, NULL, TRUE);

        switch (result)
        {
        case 1:
            NewRawDoFmt("*** Logged alert:\n%s\n", RAWFMTFUNC_SERIAL, NULL, es->es_TextFormat);
            result = -2;
            break;
        }
    } while (result == -2);

    FreeSysRequest(req);
    return result;
}

static LONG Alert_AskSuspend(char *title, char *buffer)
{
    LONG choice = -1;

    if (buffer)
    {
        struct EasyStruct es = {
            sizeof (struct EasyStruct),
            0,
            NULL,
            buffer,
            NULL,
        };

        es.es_Title = title;

        /* Determine set of buttons */
        es.es_GadgetFormat = "Log|Suspend";

        choice = SafeEasyRequest(&es, IntuitionBase);

        if (choice == 0)
            Wait(0); /* Suspend */
    }

    /* Should not be reached */
    return choice;
}

void unhandledCodePath(CONST_STRPTR function, STRPTR codepath, ULONG valDec, ULONG valHex)
{
    TEXT buffer[1024];

    __sprintf(buffer, "Function:  %s\nCode Path: %s\nValue:     %ld\nValue:     0x%lx\n", function, codepath, valDec, valHex);

    Alert_AskSuspend("EmuV0: Unhandled code path", buffer);

    asm("int3");
}

void unhandledLibraryFunction(STRPTR libname, ULONG lvo)
{
    TEXT buffer[1024];

    __sprintf(buffer, "Library: %s, LVO: %ld\n", libname, lvo);

    Alert_AskSuspend("EmuV0: Unhandled library function", buffer);

    asm("int3");
}