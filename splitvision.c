#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

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
unsigned char outimage[IMAGE_HEIGHT][IMAGE_WIDTH];

char cone_sel;

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
    row_pointers[i] = (png_byte *)outimage[i];
  png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL,
				     user_error_fn, user_warning_fn);
  if ( ! png_ptr )
    die_error ("Can't create png_ptr.\n");
  info_ptr = png_create_info_struct (png_ptr);
  if ( ! info_ptr )
    die_error ("Can't create info_ptr.\n");
  png_set_IHDR (png_ptr, info_ptr, IMAGE_WIDTH, IMAGE_HEIGHT,
		8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
  png_set_gAMA (png_ptr, info_ptr, 1./2.2);
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
  double xr, yr, zr;
  double lr, mr, sr;
  double lumr;
  long cnt_tot, cnt_ool;

  cnt_tot = 0;  cnt_ool = 0;
  lrgb_to_xyz (1., 1., 1., &xr, &yr, &zr);
  xyz_to_cone (xr, yr, zr, &lr, &mr, &sr);
  lumr = cone_lum (lr, mr, sr);
  for ( i=0 ; i<IMAGE_HEIGHT ; i++ )
    for ( j=0 ; j<IMAGE_WIDTH ; j++ )
      {
	double x, y, z;
	double l, m, s;
	double lum;
	double v;

	srgb8_to_xyz (image[i][j][0], image[i][j][1], image[i][j][2],
		      &x, &y, &z);
	xyz_to_cone (x, y, z, &l, &m, &s);
	lum = cone_lum (l, m, s);
	if ( cone_sel == 's' )
	  v = s / sr;
	else if ( cone_sel == 'm' )
	  v = m / mr;
	else if ( cone_sel == 'l' )
	  v = l / lr;
	else if ( cone_sel == 'y' )
	  v = lum / lumr;
	cnt_tot++;
	if ( v < 0. )
	  {
	    cnt_ool++;
	    v = 0.;
	  }
	else if ( v > 1. )
	  {
	    cnt_ool++;
	    v = 1.;
	  }
	v = srgb_inverse_gamma_function (v);
	outimage[i][j] = (int)(nearbyint(v*255.));
      }
  fprintf (stderr, "pixels: %ld; out of lum: %ld\n",
	   cnt_tot, cnt_ool);
}

int
main (int argc, char *argv[])
{
  if ( argc == 2 && strcmp (argv[1], "s") == 0 )
    cone_sel = 's';
  else if ( argc == 2 && strcmp (argv[1], "m") == 0 )
    cone_sel = 'm';
  else if ( argc == 2 && strcmp (argv[1], "l") == 0 )
    cone_sel = 'l';
  else if ( argc == 2 && strcmp (argv[1], "y") == 0 )
    cone_sel = 'y';
  else
    die_error ("Usage is %s [lmsy]\n", argv[0]);
  read_image (stdin);
  filter_image ();
  write_image (stdout);
  return 0;
}
