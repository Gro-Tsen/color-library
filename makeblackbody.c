#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include <png.h>

#include "data.h"
#include "library.h"

#ifndef USE_TRUE_CONE_SENSITIVITIES
#define USE_TRUE_CONE_SENSITIVITIES 0
#endif

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
#define IMAGE_WIDTH 300

unsigned short blackbody[IMAGE_WIDTH][3];
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
compute_blackbody (void)
{
  int j;

  for ( j=0 ; j<IMAGE_WIDTH ; j++ )
    {
#if USE_TRUE_CONE_SENSITIVITIES
      double l, m, s;
#endif
      double x, y, z;
      unsigned short r16, g16, b16;

#if USE_TRUE_CONE_SENSITIVITIES
      blackbody_cone (exp2((((double)j)/(IMAGE_WIDTH-1))*7.)*500.,
		      &l, &m, &s);
      cone_to_xyz (l, m, s, &x, &y, &z);
#else
      blackbody_xyz (exp2((((double)j)/(IMAGE_WIDTH-1))*7.)*500.,
		     &x, &y, &z);
#endif
      xyz_to_srgb16 (x, y, z, &r16, &g16, &b16, NULL, 1, 1., NULL);
      blackbody[j][0] = r16;
      blackbody[j][1] = g16;
      blackbody[j][2] = b16;
    }
}

void
make_image (void)
{
  int i, j, k;

  for ( i=0 ; i<IMAGE_HEIGHT ; i++ )
    for ( j=0 ; j<IMAGE_WIDTH ; j++ )
      for ( k=0 ; k<3 ; k++ )
	{
	  image[i][j][k][0] = (blackbody[j][k]>>8);
	  image[i][j][k][1] = (blackbody[j][k]&0xff);
	}
}

int
main (void)
{
  compute_blackbody ();
  make_image ();
  write_image (stdout);
  return 0;
}
