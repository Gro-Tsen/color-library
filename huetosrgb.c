#include <stdio.h>
#include <stdlib.h>

#include "library.h"

int
main (int argc, char *argv[])
{
  unsigned char r, g, b;
  double wl, x, y, z, lum;
  char out_of_gamut, out_of_lum;

  if ( argc != 2 || sscanf (argv[1], "%lf", &wl) != 1
       || ! ( wl >= 390. && wl <= 830. ) )
    {
      fprintf (stderr, "Usage is %s wavelength\n", argv[0]);
      fprintf (stderr, "(where 390<=wavelength<=830 (in nanometers))\n");
      exit (EXIT_FAILURE);
    }
  purelight_xyz (wl, &x, &y, &z);
  lum = x+y+z;
  x /= lum;  y /= lum;  z /= lum;
  lum = 1.;
  printf ("CIE (x,y,z) = (%.6f,%.6f,%.6f)\n", x, y, z);
  xyz_to_srgb8 (x, y, z, &r, &g, &b, &out_of_gamut, 1, lum, &out_of_lum);
  if ( ! out_of_gamut )
    printf ("Color is in gamut?!\n");
  if ( ! out_of_lum )
    printf ("Color is in light?!\n");
  printf ("sRGB (r8,g8,b8) = (%d,%d,%d)\n", r, g, b);
  return 0;
}
