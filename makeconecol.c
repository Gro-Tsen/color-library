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
conecol_cone (char n, double *cone_l, double *cone_m, double *cone_s)
{
  int i;
  double runl, runm, runs;
  double spec;

  runl = 0.;  runm = 0.;  runs = 0.;
  for ( i=0 ; i<CONE_SENS_DATA_SIZE ; i++ )
    {
      spec = cie_ill[i+CONE_SENS_WAVELENGTH_OFFSET-CIE_ILL_WAVELENGTH_OFFSET].d65;
      if ( n == 0 )
	spec *= cone_sens[i].cone_l;
      else if ( n == 1 )
	spec *= cone_sens[i].cone_m;
      else if ( n == 2 )
	spec *= cone_sens[i].cone_s;
      runl += cone_sens[i].cone_l * spec;
      runm += cone_sens[i].cone_m * spec;
      runs += cone_sens[i].cone_s * spec;
    }
  *cone_l = runl;
  *cone_m = runm;
  *cone_s = runs;
}

void
conecol_xyz (char n, double *x, double *y, double *z)
{
  int i;
  double runx, runy, runz;
  double spec;

  runx = 0.;  runy = 0.;  runz = 0.;
  for ( i=0 ; i<CONE_SENS_DATA_SIZE ; i++ )
    {
      spec = cie_ill[i+CONE_SENS_WAVELENGTH_OFFSET-CIE_ILL_WAVELENGTH_OFFSET].d65;
      if ( n == 0 )
	spec *= cone_sens[i].cone_l;
      else if ( n == 1 )
	spec *= cone_sens[i].cone_m;
      else if ( n == 2 )
	spec *= cone_sens[i].cone_s;
      runx += cie_cmf[i+CONE_SENS_WAVELENGTH_OFFSET-CIE_CMF_WAVELENGTH_OFFSET].xm * spec;
      runy += cie_cmf[i+CONE_SENS_WAVELENGTH_OFFSET-CIE_CMF_WAVELENGTH_OFFSET].ym * spec;
      runz += cie_cmf[i+CONE_SENS_WAVELENGTH_OFFSET-CIE_CMF_WAVELENGTH_OFFSET].zm * spec;
    }
  *x = runx;
  *y = runy;
  *z = runz;
}

void
compute_colors (void)
{
  int j;

  for ( j=0 ; j<3 ; j++ )
    {
#if USE_TRUE_CONE_SENSITIVITIES
      double l, m, s;
#endif
      double x, y, z;
      unsigned short r16, g16, b16;

#if USE_TRUE_CONE_SENSITIVITIES
      conecol_cone (j, &l, &m, &s);
      cone_to_xyz (l, m, s, &x, &y, &z);
#else
      conecol_xyz (j, &x, &y, &z);
#endif
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
