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

char cone_cut;

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
  double lw, mw, sw;
  long cnt_tot, cnt_oog, cnt_ool;

  cnt_tot = 0;  cnt_oog = 0;  cnt_ool = 0;
  xyz_to_cone (srgb_white_x, srgb_white_y, srgb_white_z,
	       &lw, &mw, &sw);
  for ( i=0 ; i<IMAGE_HEIGHT ; i++ )
    for ( j=0 ; j<IMAGE_WIDTH ; j++ )
      {
	double x, y, z;
	double l, m, s;
	double l2, m2, s2;
	double x2, y2, z2;
	char out_of_gamut, out_of_lum;

	srgb8_to_xyz (image[i][j][0], image[i][j][1], image[i][j][2],
		      &x, &y, &z);
	xyz_to_cone (x, y, z, &l, &m, &s);
	l2 = l;  m2 = m;  s2 = s;
	if ( cone_cut == 's' || cone_cut == 'S' )
	  s2 = sw * cone_lum (l2, m2, s2) / cone_lum (lw, mw, sw);
	else if ( cone_cut == 'm' || cone_cut == 'M' )
	  m2 = mw * l2 / lw;
	else if ( cone_cut == 'l' || cone_cut == 'L' )
	  l2 = lw * m2 / mw;
	if ( cone_cut == 'S' || cone_cut == 'M' || cone_cut == 'L' )
	  {
	    l2 = 0.3*l + 0.7*l2;
	    m2 = 0.3*m + 0.7*m2;
	    s2 = 0.3*s + 0.7*s2;
	  }
	cone_to_xyz (l2, m2, s2, &x2, &y2, &z2);
	xyz_to_srgb8 (x2, y2, z2,
		      &(image[i][j][0]), &(image[i][j][1]), &(image[i][j][2]),
		      &out_of_gamut, 0, 0., &out_of_lum);
	cnt_tot++;
	if ( out_of_gamut )
	  cnt_oog++;
	if ( out_of_lum )
	  cnt_ool++;
      }
  fprintf (stderr, "pixels: %ld; out of gamut: %ld; out of lum: %ld\n",
	   cnt_tot, cnt_oog, cnt_ool);
}

int
main (int argc, char *argv[])
{
  if ( argc == 2 && strcmp (argv[1], "0") == 0 )
    cone_cut = '0';
  else if ( argc == 2 && strcmp (argv[1], "s") == 0 )
    cone_cut = 's';
  else if ( argc == 2 && strcmp (argv[1], "m") == 0 )
    cone_cut = 'm';
  else if ( argc == 2 && strcmp (argv[1], "l") == 0 )
    cone_cut = 'l';
  else if ( argc == 2 && strcmp (argv[1], "S") == 0 )
    cone_cut = 'S';
  else if ( argc == 2 && strcmp (argv[1], "M") == 0 )
    cone_cut = 'M';
  else if ( argc == 2 && strcmp (argv[1], "L") == 0 )
    cone_cut = 'L';
  else
    die_error ("Usage is %s [0|l|m|s|L|M|S]\n", argv[0]);
  read_image (stdin);
  if ( cone_cut != '0' )
    filter_image ();
  write_image (stdout);
  return 0;
}
