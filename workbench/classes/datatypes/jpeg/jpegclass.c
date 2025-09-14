/*
    Copyright (C) 1995-2025, The AROS Development Team. All rights reserved.
*/

/**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <dos/dostags.h>
#include <graphics/gfxbase.h>
#include <graphics/rpattr.h>
#include <cybergraphx/cybergraphics.h>
#include <intuition/imageclass.h>
#include <intuition/icclass.h>
#include <intuition/gadgetclass.h>
#include <intuition/cghooks.h>
#include <datatypes/datatypesclass.h>
#include <datatypes/pictureclass.h>

#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/iffparse.h>
#include <proto/datatypes.h>

#include <aros/symbolsets.h>

//#include <jinclude.h>
#include <jpeglib.h>
#include <jerror.h>
#include <setjmp.h>

#include "debug.h"

#include "methods.h"

ADD2LIBS("datatypes/picture.datatype", 0, struct Library *, PictureBase);

/**************************************************************************************************/

#define QUALITY 90      /* compress quality for saving */

typedef struct {
    struct IFFHandle    *filehandle;
    UBYTE               *linebuf;
} JpegHandleType;

struct my_error_mgr {
  struct jpeg_error_mgr pub;    /* "public" fields */
  jmp_buf setjmp_buffer;        /* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  D(bug("jpeg.datatype/libjpeg: Fatal Error !\n"));
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

METHODDEF(void)
my_output_message (j_common_ptr cinfo)
{
  D(
  char buffer[JMSG_LENGTH_MAX];

  /* Create the message */
  (*cinfo->err->format_message) (cinfo, buffer);

  /* Display debug output, adding a newline */
  bug("jpeg.datatype/libjpeg: %s\n", buffer)
  );
}

#define INPUT_BUF_SIZE  4096    /* must match the size from libjpeg */

typedef struct {
  struct jpeg_source_mgr pub;   /* public fields */

  FILE * infile;                /* source stream */
 /* input buffer */
#if defined(JPEG_LIB_VERSION_MAJOR) && ((JPEG_LIB_VERSION_MAJOR > 9) || ((JPEG_LIB_VERSION_MAJOR == 9)  &&  (JPEG_LIB_VERSION_MINOR > 5)))
  JOCTET buffer[INPUT_BUF_SIZE];
#else
  JOCTET * buffer;
#endif
  boolean start_of_file;        /* have we gotten any data yet? */
} my_source_mgr;

typedef my_source_mgr * my_src_ptr;

METHODDEF(boolean)
my_fill_input_buffer (j_decompress_ptr cinfo)
{
  my_src_ptr src = (my_src_ptr) cinfo->src;
  size_t nbytes;

//  D(bug("jpeg.datatype/my_fill_input_buffer\n"));
  nbytes = Read(MKBADDR(src->infile), src->buffer, INPUT_BUF_SIZE);

  if (nbytes <= 0) {
    if (src->start_of_file)     /* Treat empty input file as fatal error */
      ERREXIT(cinfo, JERR_INPUT_EMPTY);
    WARNMS(cinfo, JWRN_JPEG_EOF);
    /* Insert a fake EOI marker */
    src->buffer[0] = (JOCTET) 0xFF;
    src->buffer[1] = (JOCTET) JPEG_EOI;
    nbytes = 2;
  }

  src->pub.next_input_byte = src->buffer;
  src->pub.bytes_in_buffer = nbytes;
  src->start_of_file = FALSE;

  return TRUE;
}

METHODDEF(void)
my_skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
  my_src_ptr src = (my_src_ptr) cinfo->src;

  if (num_bytes > 0) {
    while (num_bytes > (long) src->pub.bytes_in_buffer) {
      num_bytes -= (long) src->pub.bytes_in_buffer;
      (void)my_fill_input_buffer(cinfo);
    }
    src->pub.next_input_byte += (size_t) num_bytes;
    src->pub.bytes_in_buffer -= (size_t) num_bytes;
  }
}

/* Dummy functions for the linker */
void abort(void)
{
  exit(1);
}

void exit(int bla)
{
  D(bug("jpeg.datatype/exit\n"));
  abort();
}

/**************************************************************************************************/

static void JPEG_Exit(JpegHandleType *jpeghandle, LONG errorcode)
{
    D(if (errorcode) bug("jpeg.datatype/JPEG_Exit(): IoErr %ld\n", errorcode));
    if( jpeghandle->linebuf )
        FreeVec( jpeghandle->linebuf );
    FreeMem(jpeghandle, sizeof(JpegHandleType));
    SetIoErr(errorcode);
}

/**************************************************************************************************/

static BOOL LoadJPEG(struct IClass *cl, Object *o)
{
    JpegHandleType          *jpeghandle;
    union {
        struct IFFHandle   *iff;
        BPTR                bptr;
    } filehandle;
    long                    width, height;
    IPTR                    sourcetype;
    struct BitMapHeader     *bmhd;
    struct ColorRegister    *colormap;
    ULONG                   *colorregs;
    ULONG                   numcolors;
    STRPTR                  name;

    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;
    JSAMPARRAY buffer;          /* Output row buffer */
    int row_stride;             /* physical row width in output buffer */
    my_src_ptr src;

    D(bug("jpeg.datatype/LoadJPEG()\n"));

    if( !(jpeghandle = AllocMem(sizeof(JpegHandleType), MEMF_ANY)) )
    {
        SetIoErr(ERROR_NO_FREE_STORE);
        return FALSE;
    }
    jpeghandle->linebuf = NULL;
    if( GetDTAttrs(o,   DTA_SourceType    , (IPTR)&sourcetype ,
                        DTA_Handle        , (IPTR)&filehandle,
                        PDTA_BitMapHeader , (IPTR)&bmhd,
                        TAG_DONE) != 3 )
    {
        JPEG_Exit(jpeghandle, ERROR_OBJECT_NOT_FOUND);
        return FALSE;
    }
    
    if ( sourcetype == DTST_RAM && filehandle.iff == NULL && bmhd )
    {
        D(bug("jpeg.datatype/LoadJPEG(): Creating an empty object\n"));
        JPEG_Exit(jpeghandle, ERROR_NOT_IMPLEMENTED);
        return TRUE;
    }
    if ( sourcetype != DTST_FILE || !filehandle.bptr || !bmhd )
    {
        D(bug("jpeg.datatype/LoadJPEG(): unsupported mode\n"));
        JPEG_Exit(jpeghandle, ERROR_NOT_IMPLEMENTED);
        return FALSE;
    }

    D(bug("jpeg.datatype/LoadJPEG(): Setting error handler\n"));
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    jerr.pub.output_message = my_output_message;
    if (setjmp(jerr.setjmp_buffer)) {
        /* If we get here, the JPEG code has signaled an error.
         * We need to clean up the JPEG object, close the input file, and return.
         */
        jpeg_destroy_decompress(&cinfo);
        JPEG_Exit(jpeghandle, 1);
        return FALSE;
    }

    D(bug("jpeg.datatype/LoadJPEG(): Create decompressor\n"));
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, BADDR(filehandle.bptr));
    src = (my_src_ptr) cinfo.src;
    src->pub.fill_input_buffer = my_fill_input_buffer;
    src->pub.skip_input_data = my_skip_input_data;
    
    D(bug("jpeg.datatype/LoadJPEG(): Read Header\n"));
    (void) jpeg_read_header(&cinfo, TRUE);
    if (cinfo.output_components == 1) 
    {
        // force loading as a 8-bit greyscale image
        cinfo.out_color_space = JCS_GRAYSCALE;
    }
    D(bug("jpeg.datatype/LoadJPEG(): Starting decompression\n"));
    (void) jpeg_start_decompress(&cinfo);
    /* set BitMapHeader with image size */
    bmhd->bmh_Width  = bmhd->bmh_PageWidth  = width = cinfo.output_width;
    bmhd->bmh_Height = bmhd->bmh_PageHeight = height = cinfo.output_height;
    bmhd->bmh_Depth  = 24;
    D(bug("jpeg.datatype/LoadJPEG(): Size %ld x %ld x %d bit\n", width, height, (int)(cinfo.output_components*8)));

    if (cinfo.output_components != 1 && cinfo.output_components != 3) /* disallow images with no. components not eq to 1 or 3 */
    {
        D(bug("jpeg.datatype/LoadJPEG(): unsupported colormode\n"));
        JPEG_Exit(jpeghandle, ERROR_NOT_IMPLEMENTED);
        return FALSE;
    }

    int x;
    int comp = 3;
    numcolors = 0;
    ULONG pixelfmt = PBPAFMT_RGB; /* set default to rgb colorspace & image data */
    
    /* set colorspace & image data for 8bit */
    if (cinfo.output_components == 1)
    {
        D(bug("jpeg.datatype/LoadJPEG(): Loading 8bit Greyscale Image.\n"));
        comp = 1; 
        numcolors = 256;       
        bmhd->bmh_Depth  = 8;        
        pixelfmt = PBPAFMT_LUT8;

        D(bug("jpeg.datatype/LoadJPEG(): Colors %ld\n", numcolors));
        if( !(GetDTAttrs(o, PDTA_ColorRegisters, (IPTR)&colormap,
            PDTA_CRegs, (IPTR)&colorregs,
            TAG_DONE ) == 2) ||
            !(colormap && colorregs) )
        {
            JPEG_Exit(jpeghandle, ERROR_OBJECT_NOT_FOUND);
            return FALSE;
        }

        /* Make Greyscale Palette */        
        for(x=0; x<numcolors; x++)
        {    
            /* greyscale */
            colormap->red   = x;
            colormap->green = x;
            colormap->blue  = x;
            colormap++;

            *colorregs++ = (ULONG)x * 0x01010101;
            *colorregs++ = (ULONG)x * 0x01010101;
            *colorregs++ = (ULONG)x * 0x01010101;
        }        
        SetDTAttrs(o, NULL, NULL, PDTA_NumColors, numcolors, TAG_DONE);
    }

    /* Make a one-row-high sample array that will go away when done with image */
    row_stride = width * comp; /* Set generic row_stride instead of  [row_stride = width * 3] */
    buffer = (*cinfo.mem->alloc_sarray)
                ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
    
    /* Here we use the library's state variable cinfo.output_scanline as the
    * loop counter, so that we don't have to keep track ourselves.
    */
    while (cinfo.output_scanline < height)
    {
        /* jpeg_read_scanlines expects an array of pointers to scanlines.
         * Here the array is only one element long, but you could ask for
         * more than one scanline at a time if that's more convenient.
         */
        (void) jpeg_read_scanlines(&cinfo, buffer, 1);
        // D(bug("jpeg.datatype/LoadJPEG(): Copy line %ld\n", (long)cinfo.output_scanline));
        if(!DoSuperMethod(cl, o,
                        PDTM_WRITEPIXELARRAY,           /* Method_ID */
                        (IPTR) buffer[0],               /* PixelData */
                        pixelfmt,                       /* Use PixelFormat */
                        row_stride,                     /* Use PixelArrayMod (number of bytes per row) */
                        0,                              /* Left edge */
                        cinfo.output_scanline-1,        /* Top edge */
                        width,                          /* Width */
                        1))                             /* Height (here: one line) */
        {
            D(bug("jpeg.datatype/LoadJPEG(): WRITEPIXELARRAY failed\n"));
            JPEG_Exit(jpeghandle, ERROR_OBJECT_NOT_FOUND);
            return FALSE;
        }
    }
    D(bug("jpeg.datatype/LoadJPEG(): WRITEPIXELARRAY of whole picture done\n"));
    
    D(bug("jpeg.datatype/LoadJPEG(): Clean up\n"));
    (void) jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    /* Pass picture size to picture.datatype */
    GetDTAttrs( o, DTA_Name, (IPTR)&name, TAG_DONE );
    SetDTAttrs(o, NULL, NULL, DTA_NominalHoriz, width,
                              DTA_NominalVert , height,
                              DTA_ObjName     , (IPTR)name,
                              TAG_DONE);

    D(bug("jpeg.datatype/LoadJPEG(): Normal Exit\n"));
    JPEG_Exit(jpeghandle, 0);
    return TRUE;
}

/**************************************************************************************************/

#define OUTPUT_BUF_SIZE  4096   /* choose an efficiently fwrite'able size */

typedef struct {
  struct jpeg_destination_mgr pub; /* public fields */

  FILE * outfile;               /* target stream */
#if defined(JPEG_LIB_VERSION_MAJOR) && ((JPEG_LIB_VERSION_MAJOR > 9) || ((JPEG_LIB_VERSION_MAJOR == 9)  &&  (JPEG_LIB_VERSION_MINOR > 5)))
  JOCTET buffer[OUTPUT_BUF_SIZE];
#else
  JOCTET * buffer;              /* start of buffer */
#endif
} my_destination_mgr;

typedef my_destination_mgr * my_dest_ptr;

METHODDEF(boolean)
my_empty_output_buffer (j_compress_ptr cinfo)
{
  my_dest_ptr dest = (my_dest_ptr) cinfo->dest;

  // D(bug("jpeg.datatype/my_empty_output_buffer\n"));
  if (Write(MKBADDR(dest->outfile), dest->buffer, OUTPUT_BUF_SIZE) !=
      (size_t) OUTPUT_BUF_SIZE)
    ERREXIT(cinfo, JERR_FILE_WRITE);

  dest->pub.next_output_byte = dest->buffer;
  dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;

  return TRUE;
}

METHODDEF(void)
my_term_destination (j_compress_ptr cinfo)
{
  my_dest_ptr dest = (my_dest_ptr) cinfo->dest;
  size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;

  /* Write any data remaining in the buffer */
  if (datacount > 0) {
    if (Write(MKBADDR(dest->outfile), dest->buffer, datacount) != datacount)
      ERREXIT(cinfo, JERR_FILE_WRITE);
  }
}

/**************************************************************************************************/

static BOOL SaveJPEG(struct IClass *cl, Object *o, struct dtWrite *dtw )
{
    JpegHandleType *jpeghandle;
    BPTR filehandle;
    unsigned int width, height, numplanes, comp;
    UBYTE *linebuf;
    struct BitMapHeader *bmhd;
    long *colorregs;
    ULONG pixelfmt;

    struct jpeg_compress_struct cinfo;
    struct my_error_mgr jerr;
    JSAMPROW row_pointer[1];    /* pointer to JSAMPLE row[s] */
    int row_stride;		/* physical row width in output buffer */
    my_dest_ptr dest;

    D(bug("jpeg.datatype/SaveJPEG()\n"));

    /* A NULL file handle is a NOP */
    if( !dtw->dtw_FileHandle )
    {
        D(bug("jpeg.datatype/SaveJPEG(): empty Filehandle - just testing\n"));
        return TRUE;
    }
    filehandle = dtw->dtw_FileHandle;

    /* Get BitMapHeader and color palette */
    if( GetDTAttrs( o,  PDTA_BitMapHeader, (IPTR) &bmhd,
                        PDTA_CRegs,        (IPTR) &colorregs,
                        TAG_DONE ) != 2UL ||
        !bmhd || !colorregs )
    {
        D(bug("jpeg.datatype/SaveJPEG(): missing attributes\n"));
        SetIoErr(ERROR_OBJECT_WRONG_TYPE);
        return FALSE;
    }

    width = bmhd->bmh_Width;
    height = bmhd->bmh_Height;
    numplanes = bmhd->bmh_Depth;
    if(( numplanes != 8 ) && ( numplanes != 24 ))   /* disallow images with numplanes not eq to 8 or 24 */
    {
        D(bug("jpeg.datatype/SaveJPEG(): color depth %d, can save only depths of 24\n", numplanes));
        SetIoErr(ERROR_OBJECT_WRONG_TYPE);
        return FALSE;
    }
    D(bug("jpeg.datatype/SaveJPEG(): Picture size %d x %d (x %d bit)\n", width, height, numplanes));

    if( !(jpeghandle = AllocMem(sizeof(JpegHandleType), MEMF_ANY)) )
    {
        SetIoErr(ERROR_NO_FREE_STORE);
        return FALSE;
    }
    jpeghandle->linebuf = NULL;

    D(bug("jpeg.datatype/SaveJPEG(): Setting error handler\n"));
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    jerr.pub.output_message = my_output_message;
    if (setjmp(jerr.setjmp_buffer)) {
        /* If we get here, the JPEG code has signaled an error.
         * We need to clean up the JPEG object, close the input file, and return.
         */
        jpeg_destroy_compress(&cinfo);
        JPEG_Exit(jpeghandle, 1);
        return FALSE;
    }

    D(bug("jpeg.datatype/SaveJPEG(): Create compressor\n"));
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, (FILE *)filehandle);
    dest = (my_dest_ptr) cinfo.dest;
    dest->pub.empty_output_buffer = my_empty_output_buffer;
    dest->pub.term_destination = my_term_destination;
    
    /* determine colorspace & image data by bitdepth */
    if(numplanes == 8)
    {
        comp = 1;
        cinfo.in_color_space = JCS_GRAYSCALE;  /* greyscale colorspace of input image */
        cinfo.input_components = 1;		     /* # of color components per pixel */
        pixelfmt = PBPAFMT_LUT8;
    }    
    else if(numplanes == 24)
    {
        comp = 3;
        cinfo.in_color_space = JCS_RGB; 	/* rgb colorspace of input image */
        cinfo.input_components = 3;		/* # of color components per pixel */
        pixelfmt = PBPAFMT_RGB;
    }

    row_stride = width * comp;
    cinfo.image_width = width;          /* image width and height, in pixels */
    cinfo.image_height = height;    
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, QUALITY, TRUE /* limit to baseline-JPEG values */);
    D(bug("jpeg.datatype/SaveJPEG(): Starting compression\n"));
    jpeg_start_compress(&cinfo, TRUE);

    /* Now read the picture data line by line and write it to a chunky buffer */
    if( !(linebuf = AllocVec(row_stride, MEMF_ANY)) )   /* allocate memory using row_stride */
    {
        JPEG_Exit(jpeghandle, ERROR_NO_FREE_STORE);
        return FALSE;
    }
    jpeghandle->linebuf = linebuf;

    row_pointer[0] = linebuf;
    while (cinfo.next_scanline < cinfo.image_height)
    {
        // D(bug("jpeg.datatype/SaveJPEG(): READPIXELARRAY line %ld\n", (long)cinfo.next_scanline));
        if(!DoSuperMethod(cl, o,
                        PDTM_READPIXELARRAY,    /* Method_ID */
                        (IPTR)linebuf,          /* PixelData */
                        pixelfmt,               /* PixelFormat */
                        row_stride,             /* PixelArrayMod (number of bytes per row) */
                        0,                      /* Left edge */
                        cinfo.next_scanline,    /* Top edge */
                        width,                  /* Width */
                        1))                     /* Height */
        {
            D(bug("jpeg.datatype/SaveJPEG(): READPIXELARRAY failed!\n"));
            JPEG_Exit(jpeghandle, ERROR_OBJECT_WRONG_TYPE);
            return FALSE;
        }
        (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    D(bug("jpeg.datatype/SaveJPEG() --- Normal Exit\n"));
    JPEG_Exit(jpeghandle, 0);
    return TRUE;
}

/**************************************************************************************************/

IPTR JPEG__OM_NEW(Class *cl, Object *o, Msg msg)
{
    Object *newobj;
    
    D(bug("jpeg.datatype/DT_Dispatcher: Method OM_NEW\n"));
    newobj = (Object *)DoSuperMethodA(cl, o, (Msg)msg);
    if (newobj)
    {
        if (!LoadJPEG(cl, newobj))
        {
            CoerceMethod(cl, newobj, OM_DISPOSE);
            newobj = NULL;
        }
    }
    
    return (IPTR)newobj;
}

/**************************************************************************************************/

IPTR JPEG__DTM_WRITE(Class *cl, Object *o, struct dtWrite *dtw)
{
    D(bug("jpeg.datatype/DT_Dispatcher: Method DTM_WRITE\n"));
    if( (dtw -> dtw_Mode) == DTWM_RAW )
    {
        /* Local data format requested */
        return SaveJPEG(cl, o, dtw );
    }
    else
    {
        /* Pass msg to superclass (which writes an IFF ILBM picture)... */
        return DoSuperMethodA( cl, o, (Msg)dtw );
    }
}

/**************************************************************************************************/
