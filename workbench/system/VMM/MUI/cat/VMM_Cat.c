/****************************************************************
   This file was created automatically by `FlexCat 1.5'
   from "cat/VMM.cd".

   Do NOT edit by hand!
****************************************************************/

#include <libraries/locale.h>
#include <clib/locale_protos.h>




static LONG VMM_Version = 3;
static const STRPTR VMM_BuiltInLanguage = (STRPTR) "english";

struct FC_Type
{   LONG   ID;
    STRPTR Str;
};


const struct FC_Type _msgDontUseVM = { 0, "Don't use VM" };
const struct FC_Type _msgUseVM = { 1, "Use VM" };
const struct FC_Type _msgAdvancedOptions = { 2, "Advanced options" };
const struct FC_Type _msgAddProg = { 3, "Add Program" };
const struct FC_Type _msgAddProgShort = { 4, "p" };
const struct FC_Type _msgAddTask = { 5, "Add Task" };
const struct FC_Type _msgAddTaskShort = { 6, "t" };
const struct FC_Type _msgAdd = { 7, "Add" };
const struct FC_Type _msgAddShort = { 8, "a" };
const struct FC_Type _msgTop = { 9, "Top" };
const struct FC_Type _msgTopShort = { 10, "t" };
const struct FC_Type _msgUp = { 11, "Up" };
const struct FC_Type _msgUpShort = { 12, "u" };
const struct FC_Type _msgDown = { 13, "Down" };
const struct FC_Type _msgDownShort = { 14, "w" };
const struct FC_Type _msgBottom = { 15, "Bottom" };
const struct FC_Type _msgBottomShort = { 16, "b" };
const struct FC_Type _msgDelete = { 17, "Delete" };
const struct FC_Type _msgDeleteShort = { 18, "d" };
const struct FC_Type _msgSort = { 19, "Sort" };
const struct FC_Type _msgSortShort = { 20, "o" };
const struct FC_Type _msgCode = { 21, "Code" };
const struct FC_Type _msgData = { 22, "Data" };
const struct FC_Type _msgMinPublic = { 23, "Min. PUBLIC alloc." };
const struct FC_Type _msgMinNonPublic = { 24, "Min. non-PUBLIC alloc." };
const struct FC_Type _msgSelectTask = { 25, "Select Task" };
const struct FC_Type _msgFixedSize = { 26, "Fixed size" };
const struct FC_Type _msgDynamic = { 27, "Dynamic" };
const struct FC_Type _msgRestrictedDynamic = { 28, "Restricted dynamic" };
const struct FC_Type _msgDevice = { 29, "Partition" };
const struct FC_Type _msgFile = { 30, "File" };
const struct FC_Type _msgPseudoPart = { 31, "Pseudo-partition" };
const struct FC_Type _msgAny = { 32, "ANY" };
const struct FC_Type _msgChip = { 33, "CHIP" };
const struct FC_Type _msgFast = { 34, "FAST" };
const struct FC_Type _msgSelectFile = { 35, "Please select file" };
const struct FC_Type _msgPagingMemory = { 36, "\033cPaging memory" };
const struct FC_Type _msgMemType = { 37, "\033cMemory type" };
const struct FC_Type _msgMinMem = { 38, "\033cMinimum memory" };
const struct FC_Type _msgMaxMem = { 39, "\033cMaximum memory" };
const struct FC_Type _msgWriteBuffer = { 40, "\033cWrite buffer" };
const struct FC_Type _msgPrio = { 41, "\033cVM Priority" };
const struct FC_Type _msgFileSize = { 42, "\033cFile size" };
const struct FC_Type _msgZoomed = { 43, "Titlebar only" };
const struct FC_Type _msgPosUnzoomed = { 44, "\033cPosition in normal state" };
const struct FC_Type _msgLeftEdge = { 45, "Left" };
const struct FC_Type _msgTopEdge = { 46, "Top" };
const struct FC_Type _msgPosZoomed = { 47, "\033cPosition as titlebar" };
const struct FC_Type _msgStatistics = { 48, "Statistics" };
const struct FC_Type _msgCacheZ2RAM = { 49, "Cache Zorro II RAM" };
const struct FC_Type _msgPatchWB = { 50, "Show VM in WB title" };
const struct FC_Type _msgMemTracking = { 51, "Memory tracking" };
const struct FC_Type _msgMinVMAlloc = { 52, "Min. VM allocation" };
const struct FC_Type _msgEnableHotkey = { 53, "Enable VM hotkey" };
const struct FC_Type _msgDisableHotkey = { 54, "Disable VM hotkey" };
const struct FC_Type _msgProject = { 55, "Project" };
const struct FC_Type _msgOpen = { 56, "Open..." };
const struct FC_Type _msgOpenShort = { 57, "O" };
const struct FC_Type _msgSaveAs = { 58, "Save As..." };
const struct FC_Type _msgSaveAsShort = { 59, "A" };
const struct FC_Type _msgSaveWindow = { 60, "Save Window" };
const struct FC_Type _msgAbout = { 61, "About..." };
const struct FC_Type _msgHide = { 62, "Hide" };
const struct FC_Type _msgHideShort = { 63, "H" };
const struct FC_Type _msgQuit = { 64, "Quit" };
const struct FC_Type _msgQuitShort = { 65, "Q" };
const struct FC_Type _msgEdit = { 66, "Edit" };
const struct FC_Type _msgDefaults = { 67, "Reset To Defaults" };
const struct FC_Type _msgDefaultsShort = { 68, "D" };
const struct FC_Type _msgLastSaved = { 69, "Last Saved" };
const struct FC_Type _msgLastSavedShort = { 70, "L" };
const struct FC_Type _msgRestore = { 71, "Restore" };
const struct FC_Type _msgRestoreShort = { 72, "R" };
const struct FC_Type _msgSave = { 73, "Save" };
const struct FC_Type _msgSaveShort = { 74, "s" };
const struct FC_Type _msgUse = { 75, "Use" };
const struct FC_Type _msgUseShort = { 76, "u" };
const struct FC_Type _msgCancel = { 77, "Cancel" };
const struct FC_Type _msgCancelShort = { 78, "c" };
const struct FC_Type _msgNameMissing = { 79, "Name of partition of pagefile is missing" };
const struct FC_Type _msgVMMDescr = { 80, "Virtual memory manager" };
const struct FC_Type _msgCopyright = { 81, "Copyright 1993-95 by Martin Apel" };
const struct FC_Type _msgStartVMMError = { 82, "Couldn't start VMM\n" };
const struct FC_Type _msgOutOfMem = { 83, "Not enough memory\n" };
const struct FC_Type _msgNew = { 84, ">> NEW <<" };
const struct FC_Type _msgDefault = { 85, "----- D E F A U L T -----" };
const struct FC_Type _msgLoadPrefs = { 86, "Load VMM Preferences" };
const struct FC_Type _msgSavePrefs = { 87, "Save VMM Preferences" };
const struct FC_Type _msgInvalidPrefs = { 88, "%s is not a valid preferences file\n"\
	"Resetting to defaults" };
const struct FC_Type _msgAboutString = { 89, "\033c\033bVMM V3.3\033n\n"\
	"\033cVirtual memory for Amigas\n"\
	"\033cwith MMU\n\n"\
	"\033cCopyright 1993-95 by Martin Apel" };
const struct FC_Type _msgTasksProgs = { 90, "Tasks/Programs" };
const struct FC_Type _msgMemory = { 91, "Memory" };
const struct FC_Type _msgMisc = { 92, "Miscellaneous" };
const struct FC_Type _msgSelectProg = { 93, "Select program file" };
const struct FC_Type _msgUseCodeShort = { 94, "Y" };
const struct FC_Type _msgNoUseCodeShort = { 95, "N" };
const struct FC_Type _msgUseDataShort = { 96, "Y" };
const struct FC_Type _msgNoUseDataShort = { 97, "N" };
const struct FC_Type _msgAdvancedDataShort = { 98, "A" };
const struct FC_Type _msgSaveWinError = { 99, "Error saving window position" };
const struct FC_Type _msgNoCfgFile = { 100, "Couldn't open config file %s" };
const struct FC_Type _msgCorruptCfgFile = { 101, "File %s is not a valid config file" };
const struct FC_Type _msgFastROM = { 102, "FastROM" };
const struct FC_Type _msgAddDir = { 103, "Add Dir" };
const struct FC_Type _msgAddDirShort = { 104, "i" };
const struct FC_Type _msgSelectDir = { 105, "Select directory" };

extern struct Library *LocaleBase;

static struct Catalog *VMM_Catalog = NULL;

void OpenVMMCatalog(struct Locale *loc, STRPTR language)
{ LONG tag, tagarg;
  extern struct Library *LocaleBase;
  extern void CloseVMMCatalog(void);

  CloseVMMCatalog(); /* Not needed if the programmer pairs OpenVMMCatalog
		       and CloseVMMCatalog right, but does no harm.  */

  if (LocaleBase != NULL  &&  VMM_Catalog == NULL)
  { if (language == NULL)
    { tag = TAG_IGNORE;
    }
    else
    { tag = OC_Language;
      tagarg = (LONG) language;
    }
    VMM_Catalog = OpenCatalog(loc, (STRPTR) "VMM.catalog",
				OC_BuiltInLanguage, VMM_BuiltInLanguage,
				tag, tagarg,
				OC_Version, VMM_Version,
				TAG_DONE);
  }
}

void CloseVMMCatalog(void)
{ if (LocaleBase != NULL)
  { CloseCatalog(VMM_Catalog);
  }
  VMM_Catalog = NULL;
}

STRPTR GetVMMString(APTR fcstr)
{ STRPTR defaultstr;
  LONG strnum;

  strnum = ((struct FC_Type *) fcstr)->ID;
  defaultstr = ((struct FC_Type *) fcstr)->Str;

  return(VMM_Catalog ? GetCatalogStr(VMM_Catalog, strnum, defaultstr) :
		      defaultstr);
}
