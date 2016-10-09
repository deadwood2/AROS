
/* png_linklib.c - STDIO functions extracted to linklib
 * Copyright (c) 1998-2013 Glenn Randers-Pehrson
 * (Version 0.96 Copyright (c) 1996, 1997 Andreas Dilger)
 * (Version 0.88 Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.)
 *
 * This code is released under the libpng license.
 * For conditions of distribution and use, see the disclaimer
 * and license in png.h
 *
 * This file provides a location for all input.  Users who need
 * special handling are expected to write a function that has the same
 * arguments as this and performs a similar function, but that possibly
 * has a different input method.  Note that you shouldn't change this
 * function, but rather write a replacement function and then make
 * libpng use it at run time with png_set_read_fn(...).
 */

#define PNG_STDIO_SUPPORTED
#include <png.h>
#include <stdio.h>

static void PNGCBAPI
png_default_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
   png_size_t check;

   if (png_ptr == NULL)
      return;

   /* fread() returns 0 on error, so it is OK to store this in a png_size_t
    * instead of an int, which is what fread() actually returns.
    */
   check = fread(data, 1, length, (png_FILE_p)png_get_io_ptr(png_ptr));

   if (check != length)
      png_error(png_ptr, "Read Error");
}

static void PNGCBAPI
png_default_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
   png_size_t check;

   if (png_ptr == NULL)
      return;

   check = fwrite(data, 1, length, (png_FILE_p)png_get_io_ptr(png_ptr));

   if (check != length)
      png_error(png_ptr, "Write Error");
}

static void PNGCBAPI
png_default_flush(png_structp png_ptr)
{
   png_FILE_p io_ptr;

   if (png_ptr == NULL)
      return;

   io_ptr = (png_FILE_p)png_get_io_ptr(png_ptr);
   fflush(io_ptr);
}

void __png_init_io(png_structp png_ptr, png_FILE_p fp, png_rw_ptr read_data_fn, png_rw_ptr write_data_fn, png_flush_ptr output_flush_fn);

void PNGAPI
png_init_io(png_structrp png_ptr, png_FILE_p fp)
{
   if (png_ptr == NULL)
      return;

   __png_init_io(png_ptr, fp, png_default_read_data, png_default_write_data, png_default_flush);
}

