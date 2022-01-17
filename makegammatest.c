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

#define HALFCOLUMN_WIDTH 40

#define IMAGE_HEIGHT 480
#define IMAGE_WIDTH (HALFCOLUMN_WIDTH*16)

unsigned short intensities[IMAGE_HEIGHT][3];   /* 0=low, 1=medium, 2=high */

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
compute_intensities (void)
{
  int i;

  for ( i=0 ; i<IMAGE_HEIGHT ; i++ )
    {
      double t;
      double v[3];
      int k;

      t = 1. - ((double)i)/(IMAGE_HEIGHT-1);
      if ( t < 0.5 )
	{
	  v[0] = 0.;
	  v[2] = srgb_gamma_function (t*2.);
	}
      else
	{
	  v[0] = srgb_gamma_function (t*2.-1);
	  v[2] = 1.;
	}
      v[1] = (v[0] + v[2]) / 2.;
      for ( k=0 ; k<3 ; k++ )
	intensities[i][k]
	  = (unsigned short)(nearbyint(srgb_inverse_gamma_function(v[k])
				       *65535.));
    }
}

void
make_image (void)
{
  int i, j, k;

  for ( i=0 ; i<IMAGE_HEIGHT ; i++ )
    for ( j=0 ; j<IMAGE_WIDTH ; j++ )
      {
	unsigned short v[3];

	for ( k=0 ; k<3 ; k++ )
	  v[k] = 0;
	if ( j < HALFCOLUMN_WIDTH )
	  for ( k=0 ; k<3 ; k++ )
	    v[k] = intensities[i][1];
	else if ( j < 2*HALFCOLUMN_WIDTH && (i&1) == 0 )
	  for ( k=0 ; k<3 ; k++ )
	    v[k] = intensities[i][0];
	else if ( j < 2*HALFCOLUMN_WIDTH )
	  for ( k=0 ; k<3 ; k++ )
	    v[k] = intensities[i][2];
	else if ( j < 3*HALFCOLUMN_WIDTH  )
	  v[0] = intensities[i][1];
	else if ( j < 4*HALFCOLUMN_WIDTH && (i&1) == 0 )
	  v[0] = intensities[i][0];
	else if ( j < 4*HALFCOLUMN_WIDTH )
	  v[0] = intensities[i][2];
	else if ( j < 5*HALFCOLUMN_WIDTH  )
	  v[1] = intensities[i][1];
	else if ( j < 6*HALFCOLUMN_WIDTH && (i&1) == 0 )
	  v[1] = intensities[i][0];
	else if ( j < 6*HALFCOLUMN_WIDTH )
	  v[1] = intensities[i][2];
	else if ( j < 7*HALFCOLUMN_WIDTH  )
	  v[2] = intensities[i][1];
	else if ( j < 8*HALFCOLUMN_WIDTH && (i&1) == 0 )
	  v[2] = intensities[i][0];
	else if ( j < 8*HALFCOLUMN_WIDTH )
	  v[2] = intensities[i][2];
	else if ( j < 9*HALFCOLUMN_WIDTH  )
	  {
	    v[0] = intensities[IMAGE_HEIGHT-1-i][1];
	    v[1] = intensities[i][1];
	    v[2] = intensities[i][1];
	  }
	else if ( j < 10*HALFCOLUMN_WIDTH && (i&1) == 0 )
	  {
	    v[0] = intensities[IMAGE_HEIGHT-1-i][0];
	    v[1] = intensities[i][0];
	    v[2] = intensities[i][0];
	  }
	else if ( j < 10*HALFCOLUMN_WIDTH )
	  {
	    v[0] = intensities[IMAGE_HEIGHT-1-i][2];
	    v[1] = intensities[i][2];
	    v[2] = intensities[i][2];
	  }
	else if ( j < 11*HALFCOLUMN_WIDTH  )
	  {
	    v[0] = intensities[i][1];
	    v[1] = intensities[IMAGE_HEIGHT-1-i][1];
	    v[2] = intensities[i][1];
	  }
	else if ( j < 12*HALFCOLUMN_WIDTH && (i&1) == 0 )
	  {
	    v[0] = intensities[i][0];
	    v[1] = intensities[IMAGE_HEIGHT-1-i][0];
	    v[2] = intensities[i][0];
	  }
	else if ( j < 12*HALFCOLUMN_WIDTH )
	  {
	    v[0] = intensities[i][2];
	    v[1] = intensities[IMAGE_HEIGHT-1-i][2];
	    v[2] = intensities[i][2];
	  }
	else if ( j < 13*HALFCOLUMN_WIDTH  )
	  {
	    v[0] = intensities[i][1];
	    v[1] = intensities[i][1];
	    v[2] = intensities[IMAGE_HEIGHT-1-i][1];
	  }
	else if ( j < 14*HALFCOLUMN_WIDTH && (i&1) == 0 )
	  {
	    v[0] = intensities[i][0];
	    v[1] = intensities[i][0];
	    v[2] = intensities[IMAGE_HEIGHT-1-i][0];
	  }
	else if ( j < 14*HALFCOLUMN_WIDTH )
	  {
	    v[0] = intensities[i][2];
	    v[1] = intensities[i][2];
	    v[2] = intensities[IMAGE_HEIGHT-1-i][2];
	  }
	else if ( j < 15*HALFCOLUMN_WIDTH  )
	  {
	    v[0] = intensities[i][1];
	    v[1] = intensities[i][1];
	    v[2] = intensities[i][1];
	  }
	else if ( j < 16*HALFCOLUMN_WIDTH && (i&1) == 0 )
	  {
	    v[0] = intensities[i][0];
	    v[1] = intensities[i][0];
	    v[2] = intensities[i][0];
	  }
	else if ( j < 16*HALFCOLUMN_WIDTH )
	  {
	    v[0] = intensities[i][2];
	    v[1] = intensities[i][2];
	    v[2] = intensities[i][2];
	  }
	for ( k=0 ; k<3 ; k++ )
	  {
	    image[i][j][k][0] = (v[k]>>8);
	    image[i][j][k][1] = (v[k]&0xff);
	  }
      }
}

int
main (void)
{
  compute_intensities ();
  make_image ();
  write_image (stdout);
  return 0;
}
