#include <stdio.h>
#include <stdlib.h>

#include "library.h"

int
main (int argc, char *argv[])
{
  int r, g, b;
  double x, y, z;
  double t;

  if ( argc != 4 || sscanf (argv[1], "%d", &r) != 1
       || sscanf (argv[2], "%d", &g) != 1
       || sscanf (argv[3], "%d", &b) != 1 )
    {
      fprintf (stderr, "Usage is %s R G B\n", argv[0]);
      exit (EXIT_FAILURE);
    }
  srgb8_to_xyz (r, g, b, &x, &y, &z);
  t = x+y+z;
  x /= t;  y /= t;  z /= t;
  printf ("CIE (x,y,z) = (%.6f,%.6f,%.6f)\n", x, y, z);
  return 0;
}
