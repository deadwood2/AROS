/****************************************************************
   This file was created automatically by `FlexCat 1.5'
   from "cat/VMM-Handler.cd".

   Do NOT edit by hand!
****************************************************************/

#ifndef VMM_CAT_H
#define VMM_CAT_H


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif	/*  !EXEC_TYPES_H	    */
#ifndef LIBRARIES_LOCALE_H
#include <libraries/locale.h>
#endif	/*  !LIBRARIES_LOCALE_H     */


/*  Prototypes	*/
void OpenVMMCatalog(void);
void CloseVMMCatalog(void);
STRPTR GetVMMString(APTR);

/*  Definitions */
extern const APTR _msgNotEnoughMem;
#define msgNotEnoughMem ((APTR) &_msgNotEnoughMem)
extern const APTR _msgNoPagingFile;
#define msgNoPagingFile ((APTR) &_msgNoPagingFile)
extern const APTR _msgNoSpace;
#define msgNoSpace ((APTR) &_msgNoSpace)
extern const APTR _msgFailedIo;
#define msgFailedIo ((APTR) &_msgFailedIo)
extern const APTR _msgFileIsDir;
#define msgFileIsDir ((APTR) &_msgFileIsDir)
extern const APTR _msgInternal;
#define msgInternal ((APTR) &_msgInternal)
extern const APTR _msgInhibitFailed;
#define msgInhibitFailed ((APTR) &_msgInhibitFailed)
extern const APTR _msgWrongBlocksize;
#define msgWrongBlocksize ((APTR) &_msgWrongBlocksize)
extern const APTR _msgNoDiskinfo;
#define msgNoDiskinfo ((APTR) &_msgNoDiskinfo)
extern const APTR _msgNotValidated;
#define msgNotValidated ((APTR) &_msgNotValidated)
extern const APTR _msgOutOfBounds;
#define msgOutOfBounds ((APTR) &_msgOutOfBounds)
extern const APTR _msgNoFFS;
#define msgNoFFS ((APTR) &_msgNoFFS)
extern const APTR _msgNoConfigFile;
#define msgNoConfigFile ((APTR) &_msgNoConfigFile)
extern const APTR _msgCorruptCfgFile;
#define msgCorruptCfgFile ((APTR) &_msgCorruptCfgFile)
extern const APTR _msgVolumeNotFound;
#define msgVolumeNotFound ((APTR) &_msgVolumeNotFound)
extern const APTR _msgLowVersion;
#define msgLowVersion ((APTR) &_msgLowVersion)
extern const APTR _msgNoDOS;
#define msgNoDOS ((APTR) &_msgNoDOS)
extern const APTR _msgNoIntuition;
#define msgNoIntuition ((APTR) &_msgNoIntuition)
extern const APTR _msgNoGfx;
#define msgNoGfx ((APTR) &_msgNoGfx)
extern const APTR _msgNoUtility;
#define msgNoUtility ((APTR) &_msgNoUtility)
extern const APTR _msgNoExpansion;
#define msgNoExpansion ((APTR) &_msgNoExpansion)
extern const APTR _msgNoCommodities;
#define msgNoCommodities ((APTR) &_msgNoCommodities)
extern const APTR _msgWrongCPU;
#define msgWrongCPU ((APTR) &_msgWrongCPU)
extern const APTR _msgNoAddrSpace;
#define msgNoAddrSpace ((APTR) &_msgNoAddrSpace)
extern const APTR _msgNoPrefs;
#define msgNoPrefs ((APTR) &_msgNoPrefs)
extern const APTR _msgNoStatwindow;
#define msgNoStatwindow ((APTR) &_msgNoStatwindow)
extern const APTR _msgNoRexx;
#define msgNoRexx ((APTR) &_msgNoRexx)
extern const APTR _msgGenericError;
#define msgGenericError ((APTR) &_msgGenericError)
extern const APTR _msgMessage;
#define msgMessage ((APTR) &_msgMessage)
extern const APTR _msgError;
#define msgError ((APTR) &_msgError)
extern const APTR _msgOK;
#define msgOK ((APTR) &_msgOK)
extern const APTR _msgReboot;
#define msgReboot ((APTR) &_msgReboot)
extern const APTR _msgNoDevice;
#define msgNoDevice ((APTR) &_msgNoDevice)
extern const APTR _msgNoTimer;
#define msgNoTimer ((APTR) &_msgNoTimer)
extern const APTR _msgPPTooLarge;
#define msgPPTooLarge ((APTR) &_msgPPTooLarge)
extern const APTR _msgTooFragmented;
#define msgTooFragmented ((APTR) &_msgTooFragmented)
extern const APTR _msgVMMLibStillOpen;
#define msgVMMLibStillOpen ((APTR) &_msgVMMLibStillOpen)
extern const APTR _msgNotEnoughFrames;
#define msgNotEnoughFrames ((APTR) &_msgNotEnoughFrames)
extern const APTR _msgStillVMAllocated;
#define msgStillVMAllocated ((APTR) &_msgStillVMAllocated)
extern const APTR _msgUnusedPartition;
#define msgUnusedPartition ((APTR) &_msgUnusedPartition)
extern const APTR _msgUseCancel;
#define msgUseCancel ((APTR) &_msgUseCancel)
extern const APTR _msgDeleteFile;
#define msgDeleteFile ((APTR) &_msgDeleteFile)
extern const APTR _msgDeleteCancel;
#define msgDeleteCancel ((APTR) &_msgDeleteCancel)
extern const APTR _msgPageDevChanged;
#define msgPageDevChanged ((APTR) &_msgPageDevChanged)
extern const APTR _msgVMFree;
#define msgVMFree ((APTR) &_msgVMFree)
extern const APTR _msgPublicFastFree;
#define msgPublicFastFree ((APTR) &_msgPublicFastFree)
extern const APTR _msgNumPF;
#define msgNumPF ((APTR) &_msgNumPF)
extern const APTR _msgNumRead;
#define msgNumRead ((APTR) &_msgNumRead)
extern const APTR _msgNumWritten;
#define msgNumWritten ((APTR) &_msgNumWritten)
extern const APTR _msgNumFrames;
#define msgNumFrames ((APTR) &_msgNumFrames)
extern const APTR _msgPagesUsed;
#define msgPagesUsed ((APTR) &_msgPagesUsed)
extern const APTR _msgDynMapFailed;
#define msgDynMapFailed ((APTR) &_msgDynMapFailed)

#endif /*   !VMM_CAT_H  */
