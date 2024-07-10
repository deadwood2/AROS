/*
    Copyright (C) 2024, The AROS Development Team. All rights reserved.

    Synchronizes AROS and host clipboard. Based on debug/test/misc/hostcb.c
*/


#include <dos/dos.h>
#include <datatypes/textclass.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/iffparse.h>
#include <string.h>

/****************************************************************************************/

#define CMD(msg)        ((msg)->mn_Node.ln_Pri)
#define PARAM(msg)      ((msg)->mn_Node.ln_Name)
#define RETVAL(msg)     ((msg)->mn_Node.ln_Name)
#define SUCCESS(msg)    ((msg)->mn_Node.ln_Pri)

/****************************************************************************************/

struct Library  *IFFParseBase;
struct MsgPort  *replyport;
struct Message  msg;
STRPTR          synchronizedstate;


/****************************************************************************************/

static void cleanup()
{
    FreeVec(synchronizedstate);
    if (IFFParseBase) CloseLibrary(IFFParseBase);
    if (replyport) DeleteMsgPort(replyport);
}

/****************************************************************************************/

static void init(void)
{
    replyport = CreateMsgPort();

    msg.mn_ReplyPort = replyport;
    
    IFFParseBase = OpenLibrary("iffparse.library", 36);

    synchronizedstate = StrDup("");
}

/****************************************************************************************/

static BOOL sendclipboardmsg(struct Message *msg, char command, void *param)
{
    struct MsgPort *port;

    Forbid();
    port = FindPort("HOST_CLIPBOARD");
    if (port)
    {
        CMD(msg) = command;
        PARAM(msg) = param;

        PutMsg(port, msg);
    }
    Permit();
    
    return port ? TRUE : FALSE;
}

/****************************************************************************************/

static BOOL hostclipboardchanged()
{
    BOOL    sent;

    sent = sendclipboardmsg(&msg, 'R', NULL);
    if (sent)
    {
        WaitPort(replyport);
        GetMsg(replyport);

        if (SUCCESS(&msg) && RETVAL(&msg))
        {
            BOOL _changed = TRUE;
            if (strcmp(synchronizedstate, RETVAL(&msg)) == 0)
                _changed = FALSE;

            if (_changed)
            {
                FreeVec(synchronizedstate);
                ULONG len = strlen(RETVAL(&msg));
                synchronizedstate = AllocVec(len + 1, MEMF_ANY);
                CopyMem(RETVAL(&msg), synchronizedstate, len + 1);
            }

            FreeVec(RETVAL(&msg));
            return _changed;
        }
        else
        {
            /* In case there was clipboard string contents, but it was overwritten on host
               by for example an image, make sure runtime clipboard does not contain the
               old value */
            if (strcmp(synchronizedstate, "") != 0)
            {
                FreeVec(synchronizedstate);
                synchronizedstate = StrDup("");
                return TRUE;
            }
        }
    }

    return FALSE;

}

/****************************************************************************************/
static void writetoarosclipboard(STRPTR contents)
{
    struct IFFHandle *iff;
    BOOL ok = FALSE;

    iff = AllocIFF();
    if (iff)
    {
        if ((iff->iff_Stream = (IPTR)OpenClipboard(0)))
        {
            InitIFFasClip(iff);
            
            if (!OpenIFF(iff, IFFF_WRITE))
            {
                if (!PushChunk(iff, ID_FTXT, ID_FORM, IFFSIZE_UNKNOWN))
                {
                    if (!PushChunk(iff, 0, ID_CHRS, IFFSIZE_UNKNOWN))
                    {
                        ULONG len = strlen(contents);
                        
                        if (WriteChunkBytes(iff, contents, len) == len)
                        {
                            ok = TRUE;
                        }
                        PopChunk(iff);
                        
                    } /* if (!PushChunk(iff, 0, ID_CHRS, IFFSIZE_UNKNOWN)) */
                    
                    PopChunk(iff);
                    
                } /* if (!PushChunk(iff, ID_FTXT, ID_FORM, IFFSIZE_UNKNOWN)) */
                
                CloseIFF(iff);
                
            } /* if (!OpenIFF(iff, IFFF_WRITE)) */
            
            CloseClipboard((struct ClipboardHandle*)iff->iff_Stream);
            
        } /* if ((iff->iff_Stream = (IPTR)OpenClipboard(arosclipunit))) */
        
        FreeIFF(iff);
        
    } /* if (iff) */
}

/****************************************************************************************/

static BOOL arosclipboardchanged(void)
{
    struct IFFHandle    *iff;
    BOOL                ok = FALSE;
    STRPTR              filebuffer = NULL;

    iff = AllocIFF();
    if (iff)
    {
        if ((iff->iff_Stream = (IPTR)OpenClipboard(0)))
        {
            InitIFFasClip(iff);

            if (!OpenIFF(iff, IFFF_READ))
            {
                if (!StopChunk(iff, ID_FTXT, ID_CHRS))
                {
                    ULONG filebuffer_size = 0;
                    
                    for(;;)
                    {
                        struct ContextNode *cn;
                        LONG                error;
                        
                        error = ParseIFF(iff, IFFPARSE_SCAN);
                        if ((error != 0) && (error != IFFERR_EOC)) break;
                        
                        cn = CurrentChunk(iff);
                        if (!cn)
                        {
                            continue;
                        }
                        
                        if ((cn->cn_Type == ID_FTXT) && (cn->cn_ID == ID_CHRS))
                        {
                            if (!filebuffer)
                            {
                                filebuffer = AllocVec(cn->cn_Size + 1, MEMF_ANY);
                                if (!filebuffer) break;
                                
                                ok = TRUE;
                            }
                            else
                            {
                                STRPTR new_filebuffer;
                                
                                new_filebuffer = AllocVec(filebuffer_size + cn->cn_Size + 1, MEMF_ANY);
                                if (!new_filebuffer)
                                {
                                    ok = FALSE;
                                    break;
                                }
                                
                                CopyMem(filebuffer, new_filebuffer, filebuffer_size);
                                FreeVec(filebuffer);
                                filebuffer = new_filebuffer;
                            }
                            
                            if (ReadChunkBytes(iff, filebuffer + filebuffer_size, cn->cn_Size) != cn->cn_Size)
                            {
                                ok = FALSE;
                                break;
                            }
                            
                            filebuffer_size += cn->cn_Size;
                            filebuffer[filebuffer_size] = '\0';
                            
                        } /* if ((cn->cn_Type == ID_FTXT) && (cn->cn_ID == ID_CHRS)) */

                    } /* for(;;) */
                    
                } /* if (!StopChunk(iff, ID_FTXT, ID_CHRS)) */
                                          
                CloseIFF(iff);
                
            } /* if (!OpenIFF(iff, IFFF_READ)) */
            
            CloseClipboard((struct ClipboardHandle*)iff->iff_Stream);
            
        } /* if ((iff->iff_Stream = (IPTR)OpenClipboard(arosclipunit))) */
        FreeIFF(iff);
        
    } /* if (iff) */
    

    if (ok)
    {
        BOOL _changed = TRUE;
        if (strcmp(synchronizedstate, filebuffer) == 0)
            _changed = FALSE;

        if (_changed)
        {
            FreeVec(synchronizedstate);
            ULONG len = strlen(filebuffer);
            synchronizedstate = AllocVec(len + 1, MEMF_ANY);
            CopyMem(filebuffer, synchronizedstate, len + 1);
        }

        FreeVec(filebuffer);
        return _changed;
    }

    return FALSE;
}

static void writetohostclipboard(STRPTR contents)
{
    BOOL sent;

    sent = sendclipboardmsg(&msg, 'W', contents);

    WaitPort(replyport);
    GetMsg(replyport);
}

/****************************************************************************************/

int main(void)
{
    init();

    while(TRUE)
    {
        if (hostclipboardchanged())
        {
            writetoarosclipboard(synchronizedstate);
        }
        if (arosclipboardchanged())
        {
            writetohostclipboard(synchronizedstate);
        }
        Delay(50);
    }

    cleanup();
    
    return 0;
}

/****************************************************************************************/
