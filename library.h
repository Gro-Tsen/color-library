void cone_to_xyz (double cone_l, double cone_m, double cone_s,
		  double *x, double *y, double *z);

double cone_lum (double cone_l, double cone_m, double cone_s);

void xyz_to_cone (double x, double y, double z,
		  double *cone_l, double *cone_m, double *cone_s);

double xyz_lum (double x, double y, double z);

void renorm_xyz (double x, double y, double z,
		 double *xn, double *yn, double *zn);

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

void lrgb_to_xyz (double r, double g, double b,
		  double *x, double *y, double *z);

double
lrgb_lum (double r, double g, double b);

void xyz_to_lrgb (double x, double y, double z,
		  double *r, double *g, double *b,
		  char clipsat, char *out_of_gamut,
		  char relum, double target_lum,
		  char cliplum, char *out_of_lum);

double srgb_gamma_function (double t);

double srgb_inverse_gamma_function (double t);

void srgb_to_lrgb (double sr, double sg, double sb,
		   double *lr, double *lg, double *lb);

void lrgb_to_srgb (double lr, double lg, double lb,
		   double *sr, double *sg, double *sb);

void srgb_to_xyz (double r, double g, double b,
		  double *x, double *y, double *z);

double srgb_lum (double r, double g, double b);

void xyz_to_srgb (double x, double y, double z,
		  double *r, double *g, double *b,
		  char *out_of_gamut,
		  char relum, double target_lum, char *out_of_lum);

void srgb8_to_srgb (unsigned char r8, unsigned char g8, unsigned char b8,
		    double *r, double *g, double *b);

void srgb_to_srgb8 (double r, double g, double b,
		    unsigned char *r8, unsigned char *g8, unsigned char *b8);

void srgb8_to_xyz (unsigned char r8, unsigned char g8, unsigned char b8,
		   double *x, double *y, double *z);

double srgb8_lum (unsigned char r8, unsigned char g8, unsigned char b8);

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

void purelight_cone (double wl,
		     double *cone_l, double *cone_m, double *cone_s);

void purelight_xyz (double wl, double *x, double *y, double *z);

void blackbody_cone (double temp,
		     double *cone_l, double *cone_m, double *cone_s);

void blackbody_xyz (double temp, double *x, double *y, double *z);
