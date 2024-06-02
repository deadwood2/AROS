/*
    Copyright (C) 1995-2020, The AROS Development Team. All rights reserved.

    Desc: Build a library or device from a resident structure.
*/

#include <aros/debug.h>

#include "../include/exec/functions.h"
#include "../include/aros/asm.h"


APTR abiv0_InitResident(struct ResidentV0 *resident, BPTR segList, struct ExecBaseV0 *SysBaseV0)
{
    struct LibraryV0 *library = NULL;
    
    D(bug("InitResident begin 0x%p (\"%s\")", resident, resident->rt_Name));

    /* Check for validity */
    if(resident->rt_MatchWord != RTC_MATCHWORD ||
       resident->rt_MatchTag != (APTR32)(IPTR)resident)
        return NULL;

    /* Depending on the autoinit flag... */
    if(resident->rt_Flags & RTF_AUTOINIT)
    {
        /* ...initialize automatically... */
        struct init
        {
            ULONG dSize;
            APTR32 vectors;
            APTR32 structure;
            APTR32 init;
        };
        struct init *init = (struct init *)(IPTR)resident->rt_Init;

        D(bug("InitResident RTF_AUTOINIT"));

        /*
            Make the library. Don't call the Init routine yet, but delay
            that until we can copy stuff from the tag to the libbase.
        */
        library = abiv0_MakeLibrary(init->vectors, init->structure,
                              (APTR32)(IPTR)NULL, init->dSize, segList, SysBaseV0);

        if(library != NULL)
        {
            /*
                Copy over the interesting stuff from the ROMtag, and set the
                library state to indicate that this lib has changed and
                should be checksummed at the next opportunity.

                Don't copy the priority, because a tag's priority doesn't
                mean the same as a lib's priority.
            */
            library->lib_Node.ln_Type = resident->rt_Type;
            library->lib_Node.ln_Name = resident->rt_Name;
/*          Even if this is a resource, it was created using MakeLibrary(), this assumes
            that it has struct Library in the beginning - sonic
            if (resident->rt_Type != NT_RESOURCE)
            {*/
                library->lib_Version      = resident->rt_Version;
                library->lib_IdString     = resident->rt_IdString;
                library->lib_Flags        = LIBF_SUMUSED|LIBF_CHANGED;
                
                if (resident->rt_Flags & RTF_EXTENDED)
                {
                    library->lib_Revision = resident->rt_Revision;
                }
/*          }*/

            /*
                Call the library init vector, if set.
            */
            if(init->init)
            {
                __asm__ volatile (
                    "subq $12, %%rsp\n"
                    "movl %2, %%eax\n"
                    "movl %%eax, 8(%%rsp)\n"
                    "movl %3, %%eax\n"
                    "movl %%eax, 4(%%rsp)\n"
                    "movl %0, %%eax\n"
                    "movl %%eax, (%%rsp)\n"
                    "movl %1, %%eax\n"
                    ENTER32
                    "call *%%eax\n"
                    ENTER64
                    "addq $12, %%rsp\n"
                    "movl %%eax, %0\n"
                :"+m" (library):"m"(init->init), "m"(SysBaseV0), "m"(segList)
                : "%rax", "%rbx", "%rdi", "%rsi", "rdx", "%rcx", "%r8", "%r9" );
            }

            /*
                Test the library base, in case the init routine failed in
                some way.
            */
            if(library != NULL)
            {
                /*
                    Add the initialized module to the system.
                */
                switch(resident->rt_Type)
                {
                    case NT_DEVICE:
                    asm("int3");
                        // AddDevice((struct Device *)library);
                        break;
                    case NT_LIBRARY:
                    case NT_HIDD:   /* XXX Remove when new Hidd system ok. */
                        abiv0_AddLibrary(library, SysBaseV0);
                        break;
                    case NT_RESOURCE:
                    asm("int3");
                        // AddResource(library);
                        break;
                }
            }
        }
    }
    else
    {
        D(bug("InitResident !RTF_AUTOINIT"));
        /* ...or let the library do it. */
        if (resident->rt_Init) {
            __asm__ volatile (
                "subq $12, %%rsp\n"
                "movl %2, %%eax\n"
                "movl %%eax, 8(%%rsp)\n"
                "movl %3, %%eax\n"
                "movl %%eax, 4(%%rsp)\n"
                "movl $0, %%eax\n"
                "movl %%eax, (%%rsp)\n"
                "movl %1, %%eax\n"
                ENTER32
                "call *%%eax\n"
                ENTER64
                "addq $12, %%rsp\n"
                "movl %%eax, %0\n"
            :"=m" (library):"m"(resident->rt_Init), "m"(SysBaseV0), "m"(segList)
            : "%rax", "%rbx", "%rdi", "%rsi", "rdx", "%rcx", "%r8", "%r9" );
        }
    }

    D(bug("InitResident end 0x%p (\"%s\"), result 0x%p", resident, resident->rt_Name, library));
        
    return library;
} /* InitResident */
