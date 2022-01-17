#include <stdio.h>
#include <stdlib.h>

#include "library.h"

int
main (int argc, char *argv[])
{
  unsigned char r, g, b;
  double x, y, z, lum;
  char out_of_gamut, out_of_lum;

#if 1
  if ( argc != 4 || sscanf (argv[1], "%lf", &x) != 1
       || sscanf (argv[2], "%lf", &y) != 1
       || sscanf (argv[3], "%lf", &lum) != 1 )
    {
      fprintf (stderr, "Usage is %s x y Y\n", argv[0]);
      exit (EXIT_FAILURE);
    }
  z = 1.-x-y;
#else
  purelight_xyz (570., &x, &y, &z);
  lum = x+y+z;
  x /= lum;  y /= lum;  z /= lum;
  lum = 1.;
#endif
  printf ("CIE (x,y,z) = (%.6f,%.6f,%.6f)\n", x, y, z);
  xyz_to_srgb8 (x, y, z, &r, &g, &b, &out_of_gamut, 1, lum, &out_of_lum);
  if ( out_of_gamut )
    printf ("Color is out of gamut: clipping\n");
  if ( out_of_lum )
    printf ("Color is out of light: clipping\n");
  printf ("sRGB (r8,g8,b8) = (%d,%d,%d)\n", r, g, b);
  return 0;
}
