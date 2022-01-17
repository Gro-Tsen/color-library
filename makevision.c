#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
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

#define IMAGE_HEIGHT 256
#define IMAGE_WIDTH 256

unsigned short colors[8][3];

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
  int n;
  double lw, mw, sw;
  double l, m, s;
  double x, y, z;
  double target_lum;
  char out_of_gamut, out_of_lum;

  xyz_to_cone (srgb_white_x, srgb_white_y, srgb_white_z,
	       &lw, &mw, &sw);
  for ( n=0 ; n<8 ; n++ )
    {
      l = lw;  m = mw;  s = sw;
      if ( n & 1 )
	l *= 1.2;
      if ( n & 2 )
	m *= 1.2;
      if ( n & 4 )
	s *= 1.2;
      cone_to_xyz (l, m, s, &x, &y, &z);
#if 1
      target_lum = cone_lum (l, m, s) / cone_lum (lw, mw, sw) * 0.3;
#else
      target_lum = 0.4;
#endif
      xyz_to_srgb16 (x, y, z,
		     &(colors[n][0]), &(colors[n][1]), &(colors[n][2]),
		     &out_of_gamut, 1, target_lum, &out_of_lum);
      if ( out_of_gamut )
	fprintf (stderr, "Color %d out of gamut\n", n);
      if ( out_of_lum )
	fprintf (stderr, "Color %d out of lum\n", n);
    }
}

void
make_image (void)
{
  int i, j;

  for ( i=0 ; i<IMAGE_HEIGHT ; i++ )
    for ( j=0 ; j<IMAGE_WIDTH ; j++ )
      {
	double u, v;
	int n, k;

	u = ((double)j)/(IMAGE_WIDTH-1);
	v = 1.-((double)i)/(IMAGE_HEIGHT-1);
	n = 0;
	if ( (u-0.3)*(u-0.3) + (v-0.3)*(v-0.3) < 0.3*0.3 )
	  n |= 1;
	if ( (u-0.7)*(u-0.7) + (v-0.3)*(v-0.3) < 0.3*0.3 )
	  n |= 2;
	if ( (u-0.5)*(u-0.5) + (v-0.646)*(v-0.646) < 0.3*0.3 )
	  n |= 4;
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
