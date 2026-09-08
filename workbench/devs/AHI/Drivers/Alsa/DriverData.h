#ifndef AHI_Drivers_Alsa_DriverData_h
#define AHI_Drivers_Alsa_DriverData_h

#include <exec/libraries.h>
#include <dos/dos.h>
#include <proto/dos.h>

#define DRIVER_NEEDS_GLOBAL_EXECBASE
#include "DriverBase.h"

/* Max sample frames delivered per record-hook call. The record buffer is
   stereo 16-bit, so its byte size is RECORD_BUFFER_SAMPLES * 2ch * 2bytes. */
#define RECORD_BUFFER_SAMPLES  2048

struct AlsaBase
{
    struct DriverBase driverbase;
    struct DosLibrary*   dosbase;

    /* Mixer properties */
    APTR    al_MixerHandle;
    APTR    al_MixerElem;
    LONG    al_MinVolume;
    LONG    al_MaxVolume;
};

#define DRIVERBASE_SIZEOF (sizeof (struct AlsaBase))

#define DOSBase (*(struct DosLibrary**) &AlsaBase->dosbase)

struct AlsaData
{
    struct DriverData   driverdata;
    UBYTE               flags;
    UBYTE               pad1;
    BYTE                mastersignal;
    BYTE                slavesignal;
    struct Process*     mastertask;
    struct Process*     slavetask;
    struct AlsaBase*    ahisubbase;
    APTR                mixbuffer;

    APTR                alsahandle;

    /* --- record side (added for capture support) --- */
    struct Process*     recordslavetask;
    BYTE                recordmastersignal;   /* master<->record-slave handshake */
    BYTE                recordslavesignal;    /* record-slave kill signal        */
    UBYTE               pad2;
    UBYTE               pad3;
    APTR                capturehandle;        /* ALSA capture PCM handle         */
    APTR                recordbuffer[2];      /* double-buffered stereo 16-bit capture: the
                                                 slave fills one while the master drains the
                                                 other, so a block is never overwritten while
                                                 FeedReaders is still copying it out.        */
};


#endif /* AHI_Drivers_Alsa_DriverData_h */
