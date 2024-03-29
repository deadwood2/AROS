/*
    Copyright (C) 2009-2019, The AROS Development Team. All rights reserved.
*/

#define MUIMASTER_YES_INLINE_STDARG

#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <clib/alib_protos.h>
#include <libraries/mui.h>
#include <zune/systemprefswindow.h>

#include "locale.h"
#include "args.h"
#include "netpeditor.h"
#include "prefsdata.h"

#define VERSION "$VER: Network 1.23 (20.2.2024) AROS Dev Team"

int main(int argc, char **argv)
{
    Object *application,  *window;

    Locale_Initialize();

    ReadArguments(argc, argv);

    InitNetworkPrefs(
        (ARG(FROM) != (IPTR)NULL ? (STRPTR)ARG(FROM) : (STRPTR)PREFS_PATH_ENV),
        (ARG(USE) ? TRUE : FALSE),
        (ARG(SAVE) ? TRUE : FALSE));

    /* Show application unless SAVE or USE parameters were used */
    if (!((BOOL)ARG(SAVE)) && !((BOOL)ARG(USE)))
    {
        struct Screen *pScreen = NULL;

        if (ARG(PUBSCREEN))
            pScreen = LockPubScreen((CONST_STRPTR)ARG(PUBSCREEN));

        application = (Object *)ApplicationObject,
            MUIA_Application_Title,  __(MSG_NAME),
            MUIA_Application_Version, (IPTR)VERSION,
            MUIA_Application_Description,  __(MSG_DESCRIPTION),
            MUIA_Application_SingleTask, TRUE,
            MUIA_Application_Base, (IPTR)"NETPREF",
            SubWindow, (IPTR)(window = (Object *)SystemPrefsWindowObject,
                MUIA_Window_Screen, (IPTR)pScreen,
                MUIA_Window_ID, MAKE_ID('N', 'E', 'T', 'P'),
                WindowContents, (IPTR)NetPEditorObject,
                End,
            End),
        End;

        if (application != NULL)
        {
            SET(window, MUIA_Window_Open, TRUE);
            DoMethod(application, MUIM_Application_Execute);
            SET(window, MUIA_Window_Open, FALSE);

            MUI_DisposeObject(application);
        }
        if (pScreen)
            UnlockPubScreen(NULL, pScreen);
    }

    FreeArguments();

    Locale_Deinitialize();

    return 0;
}
