/****************************************************************
   This file was created automatically by `FlexCat 1.5'
   from "cat/VMM.cd".

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
extern void OpenVMMCatalog(struct Locale *, STRPTR);
extern void CloseVMMCatalog(void);
extern STRPTR GetVMMString(APTR);

/*  Definitions */
extern const APTR _msgDontUseVM;
#define msgDontUseVM ((APTR) &_msgDontUseVM)
extern const APTR _msgUseVM;
#define msgUseVM ((APTR) &_msgUseVM)
extern const APTR _msgAdvancedOptions;
#define msgAdvancedOptions ((APTR) &_msgAdvancedOptions)
extern const APTR _msgAddProg;
#define msgAddProg ((APTR) &_msgAddProg)
extern const APTR _msgAddProgShort;
#define msgAddProgShort ((APTR) &_msgAddProgShort)
extern const APTR _msgAddTask;
#define msgAddTask ((APTR) &_msgAddTask)
extern const APTR _msgAddTaskShort;
#define msgAddTaskShort ((APTR) &_msgAddTaskShort)
extern const APTR _msgAdd;
#define msgAdd ((APTR) &_msgAdd)
extern const APTR _msgAddShort;
#define msgAddShort ((APTR) &_msgAddShort)
extern const APTR _msgTop;
#define msgTop ((APTR) &_msgTop)
extern const APTR _msgTopShort;
#define msgTopShort ((APTR) &_msgTopShort)
extern const APTR _msgUp;
#define msgUp ((APTR) &_msgUp)
extern const APTR _msgUpShort;
#define msgUpShort ((APTR) &_msgUpShort)
extern const APTR _msgDown;
#define msgDown ((APTR) &_msgDown)
extern const APTR _msgDownShort;
#define msgDownShort ((APTR) &_msgDownShort)
extern const APTR _msgBottom;
#define msgBottom ((APTR) &_msgBottom)
extern const APTR _msgBottomShort;
#define msgBottomShort ((APTR) &_msgBottomShort)
extern const APTR _msgDelete;
#define msgDelete ((APTR) &_msgDelete)
extern const APTR _msgDeleteShort;
#define msgDeleteShort ((APTR) &_msgDeleteShort)
extern const APTR _msgSort;
#define msgSort ((APTR) &_msgSort)
extern const APTR _msgSortShort;
#define msgSortShort ((APTR) &_msgSortShort)
extern const APTR _msgCode;
#define msgCode ((APTR) &_msgCode)
extern const APTR _msgData;
#define msgData ((APTR) &_msgData)
extern const APTR _msgMinPublic;
#define msgMinPublic ((APTR) &_msgMinPublic)
extern const APTR _msgMinNonPublic;
#define msgMinNonPublic ((APTR) &_msgMinNonPublic)
extern const APTR _msgSelectTask;
#define msgSelectTask ((APTR) &_msgSelectTask)
extern const APTR _msgFixedSize;
#define msgFixedSize ((APTR) &_msgFixedSize)
extern const APTR _msgDynamic;
#define msgDynamic ((APTR) &_msgDynamic)
extern const APTR _msgRestrictedDynamic;
#define msgRestrictedDynamic ((APTR) &_msgRestrictedDynamic)
extern const APTR _msgDevice;
#define msgDevice ((APTR) &_msgDevice)
extern const APTR _msgFile;
#define msgFile ((APTR) &_msgFile)
extern const APTR _msgPseudoPart;
#define msgPseudoPart ((APTR) &_msgPseudoPart)
extern const APTR _msgAny;
#define msgAny ((APTR) &_msgAny)
extern const APTR _msgChip;
#define msgChip ((APTR) &_msgChip)
extern const APTR _msgFast;
#define msgFast ((APTR) &_msgFast)
extern const APTR _msgSelectFile;
#define msgSelectFile ((APTR) &_msgSelectFile)
extern const APTR _msgPagingMemory;
#define msgPagingMemory ((APTR) &_msgPagingMemory)
extern const APTR _msgMemType;
#define msgMemType ((APTR) &_msgMemType)
extern const APTR _msgMinMem;
#define msgMinMem ((APTR) &_msgMinMem)
extern const APTR _msgMaxMem;
#define msgMaxMem ((APTR) &_msgMaxMem)
extern const APTR _msgWriteBuffer;
#define msgWriteBuffer ((APTR) &_msgWriteBuffer)
extern const APTR _msgPrio;
#define msgPrio ((APTR) &_msgPrio)
extern const APTR _msgFileSize;
#define msgFileSize ((APTR) &_msgFileSize)
extern const APTR _msgZoomed;
#define msgZoomed ((APTR) &_msgZoomed)
extern const APTR _msgPosUnzoomed;
#define msgPosUnzoomed ((APTR) &_msgPosUnzoomed)
extern const APTR _msgLeftEdge;
#define msgLeftEdge ((APTR) &_msgLeftEdge)
extern const APTR _msgTopEdge;
#define msgTopEdge ((APTR) &_msgTopEdge)
extern const APTR _msgPosZoomed;
#define msgPosZoomed ((APTR) &_msgPosZoomed)
extern const APTR _msgStatistics;
#define msgStatistics ((APTR) &_msgStatistics)
extern const APTR _msgCacheZ2RAM;
#define msgCacheZ2RAM ((APTR) &_msgCacheZ2RAM)
extern const APTR _msgPatchWB;
#define msgPatchWB ((APTR) &_msgPatchWB)
extern const APTR _msgMemTracking;
#define msgMemTracking ((APTR) &_msgMemTracking)
extern const APTR _msgMinVMAlloc;
#define msgMinVMAlloc ((APTR) &_msgMinVMAlloc)
extern const APTR _msgEnableHotkey;
#define msgEnableHotkey ((APTR) &_msgEnableHotkey)
extern const APTR _msgDisableHotkey;
#define msgDisableHotkey ((APTR) &_msgDisableHotkey)
extern const APTR _msgProject;
#define msgProject ((APTR) &_msgProject)
extern const APTR _msgOpen;
#define msgOpen ((APTR) &_msgOpen)
extern const APTR _msgOpenShort;
#define msgOpenShort ((APTR) &_msgOpenShort)
extern const APTR _msgSaveAs;
#define msgSaveAs ((APTR) &_msgSaveAs)
extern const APTR _msgSaveAsShort;
#define msgSaveAsShort ((APTR) &_msgSaveAsShort)
extern const APTR _msgSaveWindow;
#define msgSaveWindow ((APTR) &_msgSaveWindow)
extern const APTR _msgAbout;
#define msgAbout ((APTR) &_msgAbout)
extern const APTR _msgHide;
#define msgHide ((APTR) &_msgHide)
extern const APTR _msgHideShort;
#define msgHideShort ((APTR) &_msgHideShort)
extern const APTR _msgQuit;
#define msgQuit ((APTR) &_msgQuit)
extern const APTR _msgQuitShort;
#define msgQuitShort ((APTR) &_msgQuitShort)
extern const APTR _msgEdit;
#define msgEdit ((APTR) &_msgEdit)
extern const APTR _msgDefaults;
#define msgDefaults ((APTR) &_msgDefaults)
extern const APTR _msgDefaultsShort;
#define msgDefaultsShort ((APTR) &_msgDefaultsShort)
extern const APTR _msgLastSaved;
#define msgLastSaved ((APTR) &_msgLastSaved)
extern const APTR _msgLastSavedShort;
#define msgLastSavedShort ((APTR) &_msgLastSavedShort)
extern const APTR _msgRestore;
#define msgRestore ((APTR) &_msgRestore)
extern const APTR _msgRestoreShort;
#define msgRestoreShort ((APTR) &_msgRestoreShort)
extern const APTR _msgSave;
#define msgSave ((APTR) &_msgSave)
extern const APTR _msgSaveShort;
#define msgSaveShort ((APTR) &_msgSaveShort)
extern const APTR _msgUse;
#define msgUse ((APTR) &_msgUse)
extern const APTR _msgUseShort;
#define msgUseShort ((APTR) &_msgUseShort)
extern const APTR _msgCancel;
#define msgCancel ((APTR) &_msgCancel)
extern const APTR _msgCancelShort;
#define msgCancelShort ((APTR) &_msgCancelShort)
extern const APTR _msgNameMissing;
#define msgNameMissing ((APTR) &_msgNameMissing)
extern const APTR _msgVMMDescr;
#define msgVMMDescr ((APTR) &_msgVMMDescr)
extern const APTR _msgCopyright;
#define msgCopyright ((APTR) &_msgCopyright)
extern const APTR _msgStartVMMError;
#define msgStartVMMError ((APTR) &_msgStartVMMError)
extern const APTR _msgOutOfMem;
#define msgOutOfMem ((APTR) &_msgOutOfMem)
extern const APTR _msgNew;
#define msgNew ((APTR) &_msgNew)
extern const APTR _msgDefault;
#define msgDefault ((APTR) &_msgDefault)
extern const APTR _msgLoadPrefs;
#define msgLoadPrefs ((APTR) &_msgLoadPrefs)
extern const APTR _msgSavePrefs;
#define msgSavePrefs ((APTR) &_msgSavePrefs)
extern const APTR _msgInvalidPrefs;
#define msgInvalidPrefs ((APTR) &_msgInvalidPrefs)
extern const APTR _msgAboutString;
#define msgAboutString ((APTR) &_msgAboutString)
extern const APTR _msgTasksProgs;
#define msgTasksProgs ((APTR) &_msgTasksProgs)
extern const APTR _msgMemory;
#define msgMemory ((APTR) &_msgMemory)
extern const APTR _msgMisc;
#define msgMisc ((APTR) &_msgMisc)
extern const APTR _msgSelectProg;
#define msgSelectProg ((APTR) &_msgSelectProg)
extern const APTR _msgUseCodeShort;
#define msgUseCodeShort ((APTR) &_msgUseCodeShort)
extern const APTR _msgNoUseCodeShort;
#define msgNoUseCodeShort ((APTR) &_msgNoUseCodeShort)
extern const APTR _msgUseDataShort;
#define msgUseDataShort ((APTR) &_msgUseDataShort)
extern const APTR _msgNoUseDataShort;
#define msgNoUseDataShort ((APTR) &_msgNoUseDataShort)
extern const APTR _msgAdvancedDataShort;
#define msgAdvancedDataShort ((APTR) &_msgAdvancedDataShort)
extern const APTR _msgSaveWinError;
#define msgSaveWinError ((APTR) &_msgSaveWinError)
extern const APTR _msgNoCfgFile;
#define msgNoCfgFile ((APTR) &_msgNoCfgFile)
extern const APTR _msgCorruptCfgFile;
#define msgCorruptCfgFile ((APTR) &_msgCorruptCfgFile)
extern const APTR _msgFastROM;
#define msgFastROM ((APTR) &_msgFastROM)
extern const APTR _msgAddDir;
#define msgAddDir ((APTR) &_msgAddDir)
extern const APTR _msgAddDirShort;
#define msgAddDirShort ((APTR) &_msgAddDirShort)
extern const APTR _msgSelectDir;
#define msgSelectDir ((APTR) &_msgSelectDir)

#endif /*   !VMM_CAT_H  */
