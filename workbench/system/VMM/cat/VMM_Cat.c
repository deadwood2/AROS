/****************************************************************
   This file was created automatically by `FlexCat 1.5'
   from "cat/VMM-Handler.cd".

   Do NOT edit by hand!
****************************************************************/

#include <clib/locale_protos.h>

static LONG VMM_Version = 1;
static const STRPTR VMM_BuiltInLanguage = (STRPTR) "english";

struct FC_Type
{   LONG   ID;
    STRPTR Str;
};


const struct FC_Type _msgNotEnoughMem = { 0, "Not enough memory" };
const struct FC_Type _msgNoPagingFile = { 1, "Couldn't open paging file" };
const struct FC_Type _msgNoSpace = { 2, "Not enough space on disk" };
const struct FC_Type _msgFailedIo = { 3, "IO to paging device failed" };
const struct FC_Type _msgFileIsDir = { 4, "Pagefile is a directory" };
const struct FC_Type _msgInternal = { 5, "Internal error.\n"\
	"Contact author: apel@tecmath.de" };
const struct FC_Type _msgInhibitFailed = { 6, "Couldn't inhibit DOS from using partition" };
const struct FC_Type _msgWrongBlocksize = { 7, "FFS partition with blocksize != 512 bytes" };
const struct FC_Type _msgNoDiskinfo = { 8, "Couldn't get information about disk" };
const struct FC_Type _msgNotValidated = { 9, "Partition not validated" };
const struct FC_Type _msgOutOfBounds = { 10, "Internal error:\nAccessing page out of partition bounds\n"\
	"Contact author: apel@tecmath.de" };
const struct FC_Type _msgNoFFS = { 11, "Pseudo partition not on a FFS disk" };
const struct FC_Type _msgNoConfigFile = { 12, "Couldn't find configuration file.\n"\
	"Use the 'Use' or 'Save' button in the GUI" };
const struct FC_Type _msgCorruptCfgFile = { 13, "Either VMM.prefs or\nVMM_MMU.config is corrupted." };
const struct FC_Type _msgVolumeNotFound = { 14, "Invalid location of paging file\n(Volume not found)" };
const struct FC_Type _msgLowVersion = { 15, "Need at least V37 of OS to run" };
const struct FC_Type _msgNoDOS = { 16, "Couldn't open dos.library" };
const struct FC_Type _msgNoIntuition = { 17, "Couldn't open intuition.library" };
const struct FC_Type _msgNoGfx = { 18, "Couldn't open graphics.library" };
const struct FC_Type _msgNoUtility = { 19, "Couldn't open utility.library" };
const struct FC_Type _msgNoExpansion = { 20, "Couldn't open expansion.library" };
const struct FC_Type _msgNoCommodities = { 21, "Couldn't open commodities.library" };
const struct FC_Type _msgWrongCPU = { 22, "Need a MMU to run this program" };
const struct FC_Type _msgNoAddrSpace = { 23, "Couldn't allocate address space\nfor virtual memory" };
const struct FC_Type _msgNoPrefs = { 24, "Couldn't locate VMM main program" };
const struct FC_Type _msgNoStatwindow = { 25, "Couldn't open statistics window" };
const struct FC_Type _msgNoRexx = { 26, "Couldn't open rexxsyslib.library" };
const struct FC_Type _msgGenericError = { 27, "%s from task %s:\n%s" };
const struct FC_Type _msgMessage = { 28, "Message" };
const struct FC_Type _msgError = { 29, "Error" };
const struct FC_Type _msgOK = { 30, "OK" };
const struct FC_Type _msgReboot = { 31, "Reboot" };
const struct FC_Type _msgNoDevice = { 32, "Couldn't open %s" };
const struct FC_Type _msgNoTimer = { 33, "Couldn't initialize timer" };
const struct FC_Type _msgPPTooLarge = { 34, "Not enough space for pseudo-partition.\n"\
	"Space left on device is %ld MB" };
const struct FC_Type _msgTooFragmented = { 35, "Your harddisk is too fragmented\nLargest contiguous chunk is %ld MB.\n"\
	"Resize pseudo-partition or reorganize your disk" };
const struct FC_Type _msgVMMLibStillOpen = { 36, "Can't quit. VMM.library is still opened" };
const struct FC_Type _msgNotEnoughFrames = { 37, "Can't quit. Not enough page frames\nto store still allocated memory in." };
const struct FC_Type _msgStillVMAllocated = { 38, "There is still virtual memory allocated\n"\
	"These chunks are loaded into physical memory\nand VMM will quit." };
const struct FC_Type _msgUnusedPartition = { 39, "Partition %s has not been used\nfor paging yet. Overwrite it ?" };
const struct FC_Type _msgUseCancel = { 40, "Use|Cancel" };
const struct FC_Type _msgDeleteFile = { 41, "File %s is not\ncontiguous or has wrong size.\nDelete it ?" };
const struct FC_Type _msgDeleteCancel = { 42, "Delete|Cancel" };
const struct FC_Type _msgPageDevChanged = { 43, "You cannot change parameter of the paging device\n"\
	"while VMM is running. Your changes will only take\n"\
	"effect after the next start of VMM." };
const struct FC_Type _msgVMFree = { 44, "Virtual memory free:" };
const struct FC_Type _msgPublicFastFree = { 45, "Public Fast Mem free:" };
const struct FC_Type _msgNumPF = { 46, "Number of pagefaults:" };
const struct FC_Type _msgNumRead = { 47, "Number of pages read:" };
const struct FC_Type _msgNumWritten = { 48, "Number of pages written:" };
const struct FC_Type _msgNumFrames = { 49, "Number of page frames:" };
const struct FC_Type _msgPagesUsed = { 50, "Pages used on device:" };
const struct FC_Type _msgDynMapFailed = { 51, "Could not install pagetable for dynamic MMU configuration.\n"\
	"Task stopped." };

extern struct LocaleBase *LocaleBase;
extern UWORD VMD_NestCnt;

static struct Catalog *VMM_Catalog = NULL;

/******************************************************************************************/

void OpenVMMCatalog (void)
{ 
VMD_NestCnt++;
if (LocaleBase != NULL)
  VMM_Catalog = OpenCatalog(NULL, (STRPTR) "VMM-Handler.catalog",
                           OC_BuiltInLanguage, VMM_BuiltInLanguage,
                           OC_Version, VMM_Version,
                           TAG_DONE);
VMD_NestCnt--;
}

/******************************************************************************************/

void CloseVMMCatalog (void)
{ 
VMD_NestCnt++;
if (LocaleBase != NULL)
  CloseCatalog(VMM_Catalog);
VMM_Catalog = NULL;
VMD_NestCnt--;
}

/******************************************************************************************/

STRPTR GetVMMString(APTR fcstr)
{ 
STRPTR defaultstr;
LONG strnum;
STRPTR tmp;

strnum = ((struct FC_Type *) fcstr)->ID;
defaultstr = ((struct FC_Type *) fcstr)->Str;

if (LocaleBase != NULL)
  {
  VMD_NestCnt++;
  tmp = GetCatalogStr(VMM_Catalog, strnum, defaultstr);
  VMD_NestCnt--;
  return (tmp);
  }

return (defaultstr);
}
