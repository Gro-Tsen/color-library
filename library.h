/* Convert cone (long, medium, short) response values to CIE XYZ */
void cone_to_xyz (double cone_l, double cone_m, double cone_s,
		  double *x, double *y, double *z);

/* Return luminosity function from cone (long, medium, short) response values */
double cone_lum (double cone_l, double cone_m, double cone_s);

/* Convert CIE XYZ to cone (long, medium, short) response values */
void xyz_to_cone (double x, double y, double z,
		  double *cone_l, double *cone_m, double *cone_s);

/* Return luminosity function from CIE XYZ */
double xyz_lum (double x, double y, double z);

/* Normalize CIE XYZ (divide by their sum) */
void renorm_xyz (double x, double y, double z,
		 double *xn, double *yn, double *zn);

/* CIE XYZ values of the sRGB photophors */
extern const double srgb_red_x;
extern const double srgb_red_y;
extern const double srgb_red_z;
extern const double srgb_green_x;
extern const double srgb_green_y;
extern const double srgb_green_z;
extern const double srgb_blue_x;
extern const double srgb_blue_y;
extern const double srgb_blue_z;
extern const double srgb_white_x;
extern const double srgb_white_y;
extern const double srgb_white_z;

/* Convert linear RGB to CIE XYZ */
void lrgb_to_xyz (double r, double g, double b,
		  double *x, double *y, double *z);

/* Return luminosity function from linear RGB */
double lrgb_lum (double r, double g, double b);

/* Convert CIE XYZ to linear RGB */
/* clipsat: 1 to clip to closest available saturation if out of gamut */
/* out_of_gamut: returns 1 if clipping necessary */
/* relum: 1 to target luminosity target_lum (otherwise ignored) */
/* cliplum: 1 to clip to most luminous available if out of light */
/* out_of_lum: returns 1 if clipping necessary */
void xyz_to_lrgb (double x, double y, double z,
		  double *r, double *g, double *b,
		  char clipsat, char *out_of_gamut,
		  char relum, double target_lum,
		  char cliplum, char *out_of_lum);

/* Perform the sRGB gamma conversion function */
/* Input t should be between 0 and 1, output will be too */
double srgb_gamma_function (double t);

/* Perform the sRGB inverse gamma conversion function */
/* Input t should be between 0 and 1, output will be too */
double srgb_inverse_gamma_function (double t);

/* Convert sRGB to linear RGB (i.e., apply the gamma conversion function) */
void srgb_to_lrgb (double sr, double sg, double sb,
		   double *lr, double *lg, double *lb);

/* Convert linear RGB to sRGB (i.e., apply the inverse gamma conversion) */
void lrgb_to_srgb (double lr, double lg, double lb,
		   double *sr, double *sg, double *sb);

/* Convert sRGB to CIE XYZ */
void srgb_to_xyz (double r, double g, double b,
		  double *x, double *y, double *z);

/* Return luminosity function from sRGB */
double srgb_lum (double r, double g, double b);

/* Convert CIE XYZ to sRGB */
/* out_of_gamut: returns 1 if clipping necessary */
/* relum: 1 to target luminosity target_lum (otherwise ignored) */
/* out_of_lum: returns 1 if clipping necessary */
void xyz_to_srgb (double x, double y, double z,
		  double *r, double *g, double *b,
		  char *out_of_gamut,
		  char relum, double target_lum, char *out_of_lum);

/* Convert 8-bit sRGB to sRGB (i.e., divide by 255) */
void srgb8_to_srgb (unsigned char r8, unsigned char g8, unsigned char b8,
		    double *r, double *g, double *b);

/* Convert sRGB to 8-bit sRGB (i.e., multiply by 255 and round) */
void srgb_to_srgb8 (double r, double g, double b,
		    unsigned char *r8, unsigned char *g8, unsigned char *b8);

/* Convert 8-bit sRGB to CIE XYZ */
void srgb8_to_xyz (unsigned char r8, unsigned char g8, unsigned char b8,
		   double *x, double *y, double *z);

/* Return luminosity function from 8-bit sRGB */
double srgb8_lum (unsigned char r8, unsigned char g8, unsigned char b8);

/* Convert CIE XYZ to 8-bit sRGB */
/* out_of_gamut: returns 1 if clipping necessary */
/* relum: 1 to target luminosity target_lum (otherwise ignored) */
/* out_of_lum: returns 1 if clipping necessary */
void xyz_to_srgb8 (double x, double y, double z,
		   unsigned char *r8, unsigned char *g8, unsigned char *b8,
		   char *out_of_gamut,
		   char relum, double target_lum, char *out_of_lum);

void srgb16_to_srgb (unsigned short r16, unsigned short g16, unsigned short b16,
		     double *r, double *g, double *b);

void srgb_to_srgb16 (double r, double g, double b,
		     unsigned short *r16, unsigned short *g16, unsigned short *b16);

void srgb16_to_xyz (unsigned short r16, unsigned short g16, unsigned short b16,
		    double *x, double *y, double *z);

double srgb16_lum (unsigned short r16, unsigned short g16, unsigned short b16);

void xyz_to_srgb16 (double x, double y, double z,
		    unsigned short *r16, unsigned short *g16, unsigned short *b16,
		    char *out_of_gamut,
		    char relum, double target_lum, char *out_of_lum);

extern const double pi;

extern const double speed_of_light;
extern const double planck_constant;
extern const double boltzmann_constant;

extern const double nanometers;

/* Return cone response values for a monochromatic light */
void purelight_cone (double wl,
		     double *cone_l, double *cone_m, double *cone_s);

/* Return CIE XYZ for a monochromatic light */
void purelight_xyz (double wl, double *x, double *y, double *z);

/* Return cone response values for a Planckian blackbody */
void blackbody_cone (double temp,
		     double *cone_l, double *cone_m, double *cone_s);

/* Return CIE XYZ for a Planckian blackbody */
void blackbody_xyz (double temp, double *x, double *y, double *z);

/* Return cone response values for a Rayleigh-Jeans law */
void rayleigh_jeans_cone (double temp,
			  double *cone_l, double *cone_m, double *cone_s);

/* Return CIE XYZ for a Rayleigh-Jeans law */
void rayleigh_jeans_xyz (double temp, double *x, double *y, double *z);
