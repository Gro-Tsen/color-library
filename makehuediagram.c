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

#define IMAGE_HEIGHT 256
#define IMAGE_WIDTH 256

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

int wavelength;
double xm, ym, zm;
double xw, yw, zw;

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
	x = u*xm+(1.-u)*xw;
	y = u*ym+(1.-u)*yw;
	z = u*zm+(1.-u)*zw;
	xyz_to_srgb16 (x, y, z, &r, &g, &b,
		       &out_of_gamut, 1., v, &out_of_lum);
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
  double t;

  if ( argc != 2 || sscanf (argv[1], "%d", &wavelength) < 1 )
    die_error ("Usage is %s [wavelength in nm]\n", argv[0]);
  purelight_xyz (wavelength, &xm, &ym, &zm);
  xw = srgb_white_x;  yw = srgb_white_y;  zw = srgb_white_z;
  t = xyz_lum (xw, yw, zw) / xyz_lum (xm, ym, zm);
  xm *= t;  ym *= t;  zm *= t;
  make_gamut ();
  write_image (stdout);
  return 0;
}
