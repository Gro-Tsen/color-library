#include <math.h>

void
cone_to_xyz (double cone_l, double cone_m, double cone_s,
	     double *x, double *y, double *z)
{
  *x = 0.365614080*cone_s + -1.310888253*cone_m + 1.815754888*cone_l;
  *y = -0.039383589*cone_s + 0.417270990*cone_m + 0.604800215*cone_l;
  *z = 1.864473329*cone_s;
}

double
cone_lum (double cone_l, double cone_m, double cone_s)
{
  return cone_m + 1.5 * cone_l;
}

void
xyz_to_cone (double x, double y, double z,
	     double *cone_l, double *cone_m, double *cone_s)
{
  *cone_s = 0.5363444917371623072437096*z;
  *cone_m = -0.3900710444901218622032849*x + 1.1710865640155307137436732*y
    + 0.1012280814377392603529692*z;
  *cone_l = 0.2691224752701637096180733*x + 0.8454685322119819744124444*y
    + -0.03491462173560204206371395*z;
}

double
xyz_lum (double x, double y, double z)
{
#if 1
  return 0.01361266841512370222382498*x + 2.439289362333503675362340*y
    + 0.04885614883433619725739831*z;
#else
  double cone_l, cone_m, cone_s;

  xyz_to_cone (x, y, z, &cone_l, &cone_m, &cone_s);
  return cone_lum (cone_l, cone_m, cone_s);
#endif
}

void
renorm_xyz (double x, double y, double z,
	    double *xn, double *yn, double *zn)
{
  double t;

  t = x+y+z;
  *xn = x/t;  *yn = y/t;  *zn = z/t;
}

const double srgb_red_x = 0.6400;
const double srgb_red_y = 0.3300;
const double srgb_red_z = 0.0300;
const double srgb_green_x = 0.3000;
const double srgb_green_y = 0.6000;
const double srgb_green_z = 0.1000;
const double srgb_blue_x = 0.1500;
const double srgb_blue_y = 0.0600;
const double srgb_blue_z = 0.7900;
const double srgb_white_x = 0.3127;
const double srgb_white_y = 0.3290;
const double srgb_white_z = 0.3583;

void
lrgb_to_xyz (double r, double g, double b, double *x, double *y, double *z)
{
  *x = 0.4070297188755020080321285*r + 0.3529357429718875502008032*g
    + 0.1781345381526104417670683*b;
  *y = 0.2098746987951807228915663*r + 0.7058714859437751004016064*g
    + 0.0712538152610441767068273*b;
  *z = 0.0190795180722891566265060*r + 0.1176452476572958500669344*g
    + 0.9381752342704149933065596*b;
}

double
lrgb_lum (double r, double g, double b)
{
#if 1
  return 0.5184180325668129299028974*r + 1.7323768978073126861361178*g
    + 0.2220691888712980350317619*b;
#else
  double x, y, z;

  lrgb_to_xyz (r, g, b, &x, &y, &z);
  return xyz_lum (x, y, z);
#endif
}

void
xyz_to_lrgb (double x, double y, double z, double *r, double *g, double *b,
	     char clipsat, char *out_of_gamut,
	     char relum, double target_lum, char cliplum, char *out_of_lum)
{
  double wr, wg, wb;

  wr = 3.2836574892649659004799191*x + -1.5576323987538940809968847*y
    + -0.5051780752715332154584491*z;
  wg = -0.9820097632025124884787383*x + 1.9006762933208922671946534*y
    + 0.0421023884571181484052298*z;
  wb = 0.0563627960455862294416701*x + -0.2066635855004828412861239*y
    + 1.0708931248661383593917327*z;
  if ( wr < 0. || wg < 0. || wb < 0. )
    {
      double min;

      if ( out_of_gamut )
	*out_of_gamut = 1;
      if ( clipsat )
	{
	  min = 0.;
	  if ( wr < min )
	    min = wr;
	  if ( wg < min )
	    min = wg;
	  if ( wb < min )
	    min = wb;
	  wr -= min;  wg -= min;  wb -= min;
	}
    }
  else
    {
      if ( out_of_gamut )
	*out_of_gamut = 0;
    }
  if ( relum )
    {
      double lumscale;

      lumscale = lrgb_lum (1., 1., 1.) * target_lum / lrgb_lum (wr, wg, wb);
      wr *= lumscale;  wg *= lumscale;  wb *= lumscale;
    }
  if ( wr > 1. || wg > 1. || wb > 1. )
    {
      double max;

      if ( out_of_lum )
	*out_of_lum = 1;
      if ( cliplum )
	{
	  max = 1.;
	  if ( wr > max )
	    max = wr;
	  if ( wg > max )
	    max = wg;
	  if ( wb > max )
	    max = wb;
	  wr /= max;  wg /= max;  wb /= max;
	}
    }
  else
    {
      if ( out_of_lum )
	*out_of_lum = 0;
    }
  *r = wr;  *g = wg;  *b = wb;
}

double
srgb_gamma_function (double t)
{
  if ( t > 0.0392865589442982795747610 )
    return pow ((t+0.055)/1.055, 2.4);
  else
    return t/12.9232101790454867022240029;
}

double
srgb_inverse_gamma_function (double t)
{
  if ( t > 0.00304 )
    return 1.055 * pow (t, 1.0/2.4) - 0.055;
  else
    return 12.9232101790454867022240029*t;
}

void
srgb_to_lrgb (double sr, double sg, double sb,
	      double *lr, double *lg, double *lb)
{
  *lr = srgb_gamma_function (sr);
  *lg = srgb_gamma_function (sg);
  *lb = srgb_gamma_function (sb);
}

void
lrgb_to_srgb (double lr, double lg, double lb,
	      double *sr, double *sg, double *sb)
{
  *sr = srgb_inverse_gamma_function (lr);
  *sg = srgb_inverse_gamma_function (lg);
  *sb = srgb_inverse_gamma_function (lb);
}

void
srgb_to_xyz (double r, double g, double b, double *x, double *y, double *z)
{
  double lr, lg, lb;

  srgb_to_lrgb (r, g, b, &lr, &lg, &lb);
  lrgb_to_xyz (lr, lg, lb, x, y, z);
}

double
srgb_lum (double r, double g, double b)
{
  double lr, lg, lb;

  srgb_to_lrgb (r, g, b, &lr, &lg, &lb);
  return lrgb_lum (lr, lg, lb);
}

void
xyz_to_srgb (double x, double y, double z, double *r, double *g, double *b,
	     char *out_of_gamut,
	     char relum, double target_lum, char *out_of_lum)
{
  double lr, lg, lb;

  xyz_to_lrgb (x, y, z, &lr, &lg, &lb, 1, out_of_gamut,
	       relum, target_lum, 1, out_of_lum);
  lrgb_to_srgb (lr, lg, lb, r, g, b);
}

void
srgb8_to_srgb (unsigned char r8, unsigned char g8, unsigned char b8,
	       double *r, double *g, double *b)
{
  *r = r8/255.;
  *g = g8/255.;
  *b = b8/255.;
}

void
srgb_to_srgb8 (double r, double g, double b,
	       unsigned char *r8, unsigned char *g8, unsigned char *b8)
{
  *r8 = (unsigned char)(nearbyint(r*255.));
  *g8 = (unsigned char)(nearbyint(g*255.));
  *b8 = (unsigned char)(nearbyint(b*255.));
}

void
srgb8_to_xyz (unsigned char r8, unsigned char g8, unsigned char b8,
	      double *x, double *y, double *z)
{
  double r, g, b;

  srgb8_to_srgb (r8, g8, b8, &r, &g, &b);
  srgb_to_xyz (r, g, b, x, y, z);
}

double
srgb8_lum (unsigned char r8, unsigned char g8, unsigned char b8)
{
  double r, g, b;

  srgb8_to_srgb (r8, g8, b8, &r, &g, &b);
  return srgb_lum (r, g, b);
}

void
xyz_to_srgb8 (double x, double y, double z,
	      unsigned char *r8, unsigned char *g8, unsigned char *b8,
	      char *out_of_gamut,
	      char relum, double target_lum, char *out_of_lum)
{
  double r, g, b;

  xyz_to_srgb (x, y, z, &r, &g, &b, out_of_gamut,
	       relum, target_lum, out_of_lum);
  srgb_to_srgb8 (r, g, b, r8, g8, b8);
}

void
srgb16_to_srgb (unsigned short r16, unsigned short g16, unsigned short b16,
		double *r, double *g, double *b)
{
  *r = r16/65535.;
  *g = g16/65535.;
  *b = b16/65535.;
}

void
srgb_to_srgb16 (double r, double g, double b,
		unsigned short *r16, unsigned short *g16, unsigned short *b16)
{
  *r16 = (unsigned short)(nearbyint(r*65535.));
  *g16 = (unsigned short)(nearbyint(g*65535.));
  *b16 = (unsigned short)(nearbyint(b*65535.));
}

void
srgb16_to_xyz (unsigned short r16, unsigned short g16, unsigned short b16,
	       double *x, double *y, double *z)
{
  double r, g, b;

  srgb16_to_srgb (r16, g16, b16, &r, &g, &b);
  srgb_to_xyz (r, g, b, x, y, z);
}

double
srgb16_lum (unsigned short r16, unsigned short g16, unsigned short b16)
{
  double r, g, b;

  srgb16_to_srgb (r16, g16, b16, &r, &g, &b);
  return srgb_lum (r, g, b);
}

void
xyz_to_srgb16 (double x, double y, double z,
	       unsigned short *r16, unsigned short *g16, unsigned short *b16,
	       char *out_of_gamut,
	       char relum, double target_lum, char *out_of_lum)
{
  double r, g, b;

  xyz_to_srgb (x, y, z, &r, &g, &b, out_of_gamut,
	       relum, target_lum, out_of_lum);
  srgb_to_srgb16 (r, g, b, r16,g16, b16);
}

#include "data.h"

const double pi = 3.1415926535897932384626434;

const double speed_of_light = 299792458.;
const double planck_constant = 6.62606876e-34;
const double boltzmann_constant = 1.3806503e-23;

const double nanometers = 1.e-9;

void
purelight_cone (double wl, double *cone_l, double *cone_m, double *cone_s)
{
  double p, q;
  int i;

  i = floor (wl);
  p = wl-i;
  q = 1.-p;
  i -= CONE_SENS_WAVELENGTH_OFFSET;
  if ( i < -1 )
    {
      *cone_l = 0.;
      *cone_m = 0.;
      *cone_s = 0.;
    }
  else if ( i >= CONE_SENS_DATA_SIZE )
    {
      *cone_l = 0.;
      *cone_m = 0.;
      *cone_s = 0.;
    }
  else if ( i == -1 )
    {
      *cone_l = p*cone_sens[i+1].cone_l;
      *cone_m = p*cone_sens[i+1].cone_m;
      *cone_s = p*cone_sens[i+1].cone_s;
    }
  else if ( i == CONE_SENS_DATA_SIZE-1 )
    {
      *cone_l = q*cone_sens[i].cone_l;
      *cone_m = q*cone_sens[i].cone_m;
      *cone_s = q*cone_sens[i].cone_s;
    }
  else
    {
      *cone_l = q*cone_sens[i].cone_l + p*cone_sens[i+1].cone_l;
      *cone_m = q*cone_sens[i].cone_m + p*cone_sens[i+1].cone_m;
      *cone_s = q*cone_sens[i].cone_s + p*cone_sens[i+1].cone_s;
    }
}

void
purelight_xyz (double wl, double *x, double *y, double *z)
{
  double p, q;
  int i;

  i = floor (wl);
  p = wl-i;
  q = 1.-p;
  i -= CIE_CMF_WAVELENGTH_OFFSET;
  if ( i < -1 )
    {
      *x = 0.;
      *y = 0.;
      *z = 0.;
    }
  else if ( i >= CIE_CMF_DATA_SIZE )
    {
      *x = 0.;
      *y = 0.;
      *z = 0.;
    }
  else if ( i == -1 )
    {
      *x = p*cie_cmf[i+1].xm;
      *y = p*cie_cmf[i+1].ym;
      *z = p*cie_cmf[i+1].zm;
    }
  else if ( i == CIE_CMF_DATA_SIZE-1 )
    {
      *x = q*cie_cmf[i].xm;
      *y = q*cie_cmf[i].ym;
      *z = q*cie_cmf[i].zm;
    }
  else
    {
      *x = q*cie_cmf[i].xm + p*cie_cmf[i+1].xm;
      *y = q*cie_cmf[i].ym + p*cie_cmf[i+1].ym;
      *z = q*cie_cmf[i].zm + p*cie_cmf[i+1].zm;
    }
}

void
blackbody_cone (double temp, double *cone_l, double *cone_m, double *cone_s)
{
  int i;
  double runl, runm, runs;
  double lambda;
  double spec;

  runl = 0.;  runm = 0.;  runs = 0.;
  for ( i=0 ; i<CONE_SENS_DATA_SIZE ; i++ )
    {
      lambda = cone_sens[i].wavelength * nanometers;
      spec = 2.*planck_constant*speed_of_light*speed_of_light*nanometers
	/ (pow(lambda,5.) * (exp((planck_constant*speed_of_light)
				 /(lambda*boltzmann_constant*temp))-1.));
      runl += cone_sens[i].cone_l * spec;
      runm += cone_sens[i].cone_m * spec;
      runs += cone_sens[i].cone_s * spec;
    }
  *cone_l = runl;
  *cone_m = runm;
  *cone_s = runs;
}

void
blackbody_xyz (double temp, double *x, double *y, double *z)
{
  int i;
  double runx, runy, runz;
  double lambda;
  double spec;

  runx = 0.;  runy = 0.;  runz = 0.;
  for ( i=0 ; i<CIE_CMF_DATA_SIZE ; i++ )
    {
      lambda = cie_cmf[i].wavelength * nanometers;
      spec = 2.*planck_constant*speed_of_light*speed_of_light*nanometers
	/ (pow(lambda,5.) * (exp((planck_constant*speed_of_light)
				 /(lambda*boltzmann_constant*temp))-1.));
      runx += cie_cmf[i].xm * spec;
      runy += cie_cmf[i].ym * spec;
      runz += cie_cmf[i].zm * spec;
    }
  *x = runx;
  *y = runy;
  *z = runz;
}
