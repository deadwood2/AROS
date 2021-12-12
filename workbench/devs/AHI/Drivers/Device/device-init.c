
#include <config.h>

#include "library.h"
#include "DriverData.h"

#ifdef __AROS__
#include <libraries/crtutil.h>

struct CrtUtilBase *CrtUtilBase = NULL;
#endif


/******************************************************************************
** Custom driver init *********************************************************
******************************************************************************/

BOOL
DriverInit( struct DriverBase* AHIsubBase )
{
  struct DeviceBase* DeviceBase = (struct DeviceBase*) AHIsubBase;

  DOSBase = (struct DosLibrary*) OpenLibrary( "dos.library", 37 );

  if( DOSBase == NULL )
  {
    Req( "Unable to open 'dos.library' version 37.\n" );
    return FALSE;
  }

#ifdef __AMIGAOS4__
  if ((IDOS = (struct DOSIFace *) GetInterface((struct Library *) DOSBase, "main", 1, NULL)) == NULL)
  {
    Req("Couldn't open IDOS interface!\n");
    return FALSE;
  }
#endif

#ifdef __AROS__
  CrtUtilBase = (struct CrtUtilBase *) OpenLibrary( "crtutil.library", 0 );

  if( CrtUtilBase == NULL )
  {
    Req( "Unable to open 'crtutil.library'.\n" );
    return FALSE;
  }
#endif

  return TRUE;
}


/******************************************************************************
** Custom driver clean-up *****************************************************
******************************************************************************/

VOID
DriverCleanup( struct DriverBase* AHIsubBase )
{
  struct DeviceBase* DeviceBase = (struct DeviceBase*) AHIsubBase;

#ifdef __AROS__
  CloseLibrary( (struct Library*) CrtUtilBase );
#endif

#ifdef __AMIGAOS4__
  DropInterface( (struct Interface *) IDOS);
#endif

  CloseLibrary( (struct Library*) DOSBase );
}
