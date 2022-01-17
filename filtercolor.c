#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <png.h>

#include "data.h"
#include "library.h"

void
die_error (const char *fmt, ...)
{
  va_list ap;

  va_start (ap, fmt);
  vfprintf (stderr, fmt, ap);
  va_end (ap);
  exit (EXIT_FAILURE);
}

void
user_error_fn (png_structp png_ptr, png_const_charp error_msg)
{
  die_error ("%s\n", error_msg);
}

void
user_warning_fn (png_structp png_ptr, png_const_charp error_msg)
{
  fprintf (stderr, "%s\n", error_msg);
}

#define IMAGE_HEIGHT 240
#define IMAGE_WIDTH 320

unsigned char image[IMAGE_HEIGHT][IMAGE_WIDTH][3];

char oper;

void
read_image (FILE *inf)
{
  png_structp png_ptr;
  png_infop info_ptr;
  png_byte *row_pointers[IMAGE_HEIGHT];
  int i;

  for ( i=0 ; i<IMAGE_HEIGHT ; i++ )
    row_pointers[i] = (png_byte *)image[i];
  png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL,
				    user_error_fn, user_warning_fn);
  if ( ! png_ptr )
    die_error ("Can't create png_ptr.\n");
  info_ptr = png_create_info_struct (png_ptr);
  if ( ! info_ptr )
    die_error ("Can't create info_ptr.\n");
  png_init_io (png_ptr, inf);
  png_read_info (png_ptr, info_ptr);
  if ( png_get_image_width (png_ptr, info_ptr) != IMAGE_WIDTH
       || png_get_image_height (png_ptr, info_ptr) != IMAGE_HEIGHT )
    die_error ("Image doesn't have expected size.\n");
  if ( png_get_bit_depth (png_ptr, info_ptr) != 8
       || png_get_color_type (png_ptr, info_ptr) != PNG_COLOR_TYPE_RGB )
    die_error ("Image doesn't have expected format.\n");
  png_read_image (png_ptr, row_pointers);
  png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
}

void
write_image (FILE *outf)
{
  png_structp png_ptr;
  png_infop info_ptr;
  png_byte *row_pointers[IMAGE_HEIGHT];
  int i;

  for ( i=0 ; i<IMAGE_HEIGHT ; i++ )
    row_pointers[i] = (png_byte *)image[i];
  png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL,
				     user_error_fn, user_warning_fn);
  if ( ! png_ptr )
    die_error ("Can't create png_ptr.\n");
  info_ptr = png_create_info_struct (png_ptr);
  if ( ! info_ptr )
    die_error ("Can't create info_ptr.\n");
  png_set_IHDR (png_ptr, info_ptr, IMAGE_WIDTH, IMAGE_HEIGHT,
		8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
  png_set_sRGB_gAMA_and_cHRM (png_ptr, info_ptr, 3);
  png_init_io (png_ptr, outf);
  png_write_info (png_ptr, info_ptr);
  png_write_image (png_ptr, row_pointers);
  png_write_end (png_ptr, info_ptr);
  png_write_flush (png_ptr);
  png_destroy_write_struct (&png_ptr, &info_ptr);
}

void
filter_image (void)
{
  int i, j;

  for ( i=0 ; i<IMAGE_HEIGHT ; i++ )
    for ( j=0 ; j<IMAGE_WIDTH ; j++ )
      {
	double sr, sg, sb;
	double lr, lg, lb;

	srgb8_to_srgb (image[i][j][0], image[i][j][1], image[i][j][2],
		       &sr, &sg, &sb);
	srgb_to_lrgb (sr, sg, sb, &lr, &lg, &lb);
	switch ( oper )
	  {
	  case 'H':
	    {
	      double min;
	      double lum, lumw;

	      lum = lrgb_lum (lr, lg, lb);
	      lumw = lrgb_lum (1., 1., 1.);
	      min = lr;
	      if ( lg < min )
		min = lg;
	      if ( lb < min )
		min = lb;
	      if ( lum*(1.-lr)/(lumw-lum) < min )
		min = lum*(1.-lr)/(lumw-lum);
	      if ( lum*(1.-lg)/(lumw-lum) < min )
		min = lum*(1.-lg)/(lumw-lum);
	      if ( lum*(1.-lb)/(lumw-lum) < min )
		min = lum*(1.-lb)/(lumw-lum);
	      lr -= min;
	      lg -= min;
	      lb -= min;
	      if ( lum-lumw*min == 0 )
		{
		  lr = lum/lumw;
		  lg = lum/lumw;
		  lb = lum/lumw;
		}
	      else
		{
		  lr *= lum/(lum-lumw*min);
		  lg *= lum/(lum-lumw*min);
		  lb *= lum/(lum-lumw*min);
		}
	    }
	    break;
	  case 'h':
	    {
	      double min;

	      min = lr;
	      if ( lg < min )
		min = lg;
	      if ( lb < min )
		min = lb;
	      lr -= min;
	      lg -= min;
	      lb -= min;
	    }
	  case 'c':
	    {
	      double max;

	      max = lr;
	      if ( lg > max )
		max = lg;
	      if ( lb > max )
		max = lb;
	      if ( max == 0. )
		{
		  lr = 1.;
		  lg = 1.;
		  lb = 1.;
		}
	      else
		{
		  lr /= max;
		  lg /= max;
		  lb /= max;
		}
	    }
	  }
	lrgb_to_srgb (lr, lg, lb, &sr, &sg, &sb);
	srgb_to_srgb8 (sr, sg, sb,
		      &(image[i][j][0]), &(image[i][j][1]), &(image[i][j][2]));
      }
}

int
main (int argc, char *argv[])
{
  if ( argc == 2 && strcmp (argv[1], "c") == 0 )
    oper = 'c';
  else if ( argc == 2 && strcmp (argv[1], "h") == 0 )
    oper = 'h';
  else if ( argc == 2 && strcmp (argv[1], "H") == 0 )
    oper = 'H';
  else
    die_error ("Usage is %s [c|h|H]\n", argv[0]);
  read_image (stdin);
  filter_image ();
  write_image (stdout);
  return 0;
}
