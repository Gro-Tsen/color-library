#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include <png.h>

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

#define IMAGE_HEIGHT 24
#define IMAGE_WIDTH 72

unsigned short colors[3][3];

unsigned char image[IMAGE_HEIGHT][IMAGE_WIDTH][3][2];

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
		16, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
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
compute_colors (void)
{
  int j;

  for ( j=0 ; j<3 ; j++ )
    {
      double l, m, s;
      double x, y, z;
      unsigned short r16, g16, b16;

      if ( j == 0 ) { l=1.; m=0.; s=0.; }
      else if ( j == 1 ) { l=0.; m=1.; s=0.; }
      else if ( j == 2 ) { l=0.; m=0.; s=1.; }
      cone_to_xyz (l, m, s, &x, &y, &z);
      xyz_to_srgb16 (x, y, z, &r16, &g16, &b16, NULL, 1, 1., NULL);
      colors[j][0] = r16;
      colors[j][1] = g16;
      colors[j][2] = b16;
    }
}

void
make_image (void)
{
  int i, j, k, n;

  for ( i=0 ; i<IMAGE_HEIGHT ; i++ )
    for ( j=0 ; j<IMAGE_WIDTH ; j++ )
      {
	n = 0;
	if ( j >= IMAGE_WIDTH/3 )
	  n++;
	if ( j >= 2*IMAGE_WIDTH/3 )
	  n++;
	for ( k=0 ; k<3 ; k++ )
	  {
	    image[i][j][k][0] = (colors[n][k]>>8);
	    image[i][j][k][1] = (colors[n][k]&0xff);
	  }
      }
}

int
main (void)
{
  compute_colors ();
  make_image ();
  write_image (stdout);
  return 0;
}
