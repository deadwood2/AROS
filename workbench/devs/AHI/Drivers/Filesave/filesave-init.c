
#include <config.h>

#include <dos/dos.h>
#include <graphics/gfxbase.h>
#include <libraries/asl.h>
#include <proto/exec.h>
#include <proto/datatypes.h>

#include "library.h"

#include "DriverData.h"

#ifdef __AROS__
#include <libraries/stdlib.h>

struct StdlibBase *StdlibBase = NULL;
#endif


/******************************************************************************
** Custom driver init *********************************************************
******************************************************************************/

BOOL
DriverInit( struct DriverBase* AHIsubBase )
{
  struct FilesaveBase* FilesaveBase = (struct FilesaveBase*) AHIsubBase;

  FilesaveBase->dosbase = (struct DosLibrary *)OpenLibrary( DOSNAME, 37 );
  FilesaveBase->gfxbase = (APTR)OpenLibrary( GRAPHICSNAME, 37 );
  FilesaveBase->aslbase = OpenLibrary( AslName, 37);
  FilesaveBase->dtsbase = OpenLibrary( "datatypes.library" ,39 );

  if( DOSBase == NULL )
  {
    Req( "Unable to open '" DOSNAME "' version 37.\n" );
    return FALSE;
  }

  if( GfxBase == NULL )
  {
    Req( "Unable to open '" GRAPHICSNAME "' version 37.\n" );
    return FALSE;
  }

// Don't fail if these ones don't open!

  if( AslBase == NULL )
  {
    Req( "Unable to open '" AslName "' version 37.\n" );
  }

#if 0
  if( DataTypesBase == NULL )
  {
    Req( "Unable to open 'datatypes.library' version 39.\n" );
  }
#endif

#ifdef __AMIGAOS4__
  if ((IDOS = (struct DOSIFace *) GetInterface((struct Library *) DOSBase, "main", 1, NULL)) == NULL)
  {
    Req("Couldn't open IDOS interface!\n");
    return FALSE;
  }

  if (AslBase != NULL) {
    if ((IAsl = (struct AslIFace *) GetInterface((struct Library *) AslBase, "main", 1, NULL)) == NULL)
    {
      Req("Couldn't open IAsl interface!\n");
      return FALSE;
    }
  }

  if (DataTypesBase != NULL) {
    if ((IDataTypes = (struct DataTypesIFace *) GetInterface((struct Library *) DataTypesBase, "main", 1, NULL)) == NULL)
    {
      Req("Couldn't open IDataTypes interface!\n");
      return FALSE;
    }
  }
#endif

#ifdef __AROS__
  StdlibBase = (struct StdlibBase *) OpenLibrary( "stdlib.library", 0 );

  if( StdlibBase == NULL )
  {
    Req( "Unable to open 'stdlib.library'.\n" );
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
  struct FilesaveBase* FilesaveBase = (struct FilesaveBase*) AHIsubBase;

#ifdef __AROS__
  CloseLibrary( (struct Library*) StdlibBase );
#endif

#ifdef __AMIGAOS4__
  DropInterface( (struct Interface *) IDOS);
  DropInterface( (struct Interface *) IAsl);
  DropInterface( (struct Interface *) IDataTypes);
#endif

  CloseLibrary( (APTR)FilesaveBase->dosbase );
  CloseLibrary( (APTR)FilesaveBase->gfxbase );
  CloseLibrary( (APTR)FilesaveBase->aslbase );
  CloseLibrary( (APTR)FilesaveBase->dtsbase );
}
