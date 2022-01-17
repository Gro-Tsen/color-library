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

#define IMAGE_HEIGHT 512
#define IMAGE_WIDTH 512

unsigned char image[IMAGE_HEIGHT][IMAGE_WIDTH][3][2];

double target_lum;

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
make_curve (void)
{
  double wl;

  for ( wl=400. ; wl<=800. ; wl+=0.05 )
    {
#if USE_TRUE_CONE_SENSITIVITIES
      double l, m, s;
#endif
      double u, v;
      int i, j;
      double x, y, z;
      double t;
      unsigned short r, g, b;

#if USE_TRUE_CONE_SENSITIVITIES
      purelight_cone (wl, &l, &m, &s);
      cone_to_xyz (l, m, s, &x, &y, &z);
#else
      purelight_xyz (wl, &x, &y, &z);
#endif
      t = x + y + z;
      x /= t;  y /= t;  z /= t;
      u = x;  v = y;
      i = (int)(nearbyint((1.-v)*(IMAGE_HEIGHT-1)));
      j = (int)(nearbyint(u*(IMAGE_WIDTH-1)));
      if ( i<0 || j<0 || i>=IMAGE_HEIGHT || j>=IMAGE_WIDTH )
	{
	  fprintf (stderr, "WARNING: (%d, %d)\n", i, j);
	  continue;
	}
      xyz_to_srgb16 (x, y, z, &r, &g, &b,
		     NULL, 1., 1., NULL);
      image[i][j][0][0] = (r>>8);
      image[i][j][0][1] = (r&0xff);
      image[i][j][1][0] = (g>>8);
      image[i][j][1][1] = (g&0xff);
      image[i][j][2][0] = (b>>8);
      image[i][j][2][1] = (b&0xff);
    }
}

void
make_gamut (void)
{
  int i, j;

  for ( i=0 ; i<IMAGE_HEIGHT ; i++ )
    for ( j=0 ; j<IMAGE_WIDTH ; j++ )
      {
	double u, v;
	double x, y, z;
	unsigned short r, g, b;
	char out_of_gamut, out_of_lum;

	u = ((double)j)/(IMAGE_WIDTH-1);
	v = 1.-((double)i)/(IMAGE_HEIGHT-1);
	x = u;  y = v;  z = 1.-u-v;
	xyz_to_srgb16 (x, y, z, &r, &g, &b,
		       &out_of_gamut, 1, target_lum, &out_of_lum);
	if ( ! ( out_of_gamut || out_of_lum ) )
	  {
	    image[i][j][0][0] = (r>>8);
	    image[i][j][0][1] = (r&0xff);
	    image[i][j][1][0] = (g>>8);
	    image[i][j][1][1] = (g&0xff);
	    image[i][j][2][0] = (b>>8);
	    image[i][j][2][1] = (b&0xff);
	  }
      }
}

int
main (int argc, char *argv[])
{
  if ( argc != 2 || sscanf (argv[1], "%lf", &target_lum) < 1 )
    die_error ("Usage is %s [target luminosity]\n", argv[0]);
  make_gamut ();
  make_curve ();
  write_image (stdout);
  return 0;
}
