#define CONE_SENS_WAVELENGTH_OFFSET 390
#define CONE_SENS_DATA_SIZE 441
extern struct {
  int wavelength;
  double cone_l, cone_m, cone_s;
} cone_sens[CONE_SENS_DATA_SIZE];

#define CIE_CMF_WAVELENGTH_OFFSET 360
#define CIE_CMF_DATA_SIZE 471
extern struct {
  int wavelength;
  double xm, ym, zm;
} cie_cmf[CIE_CMF_DATA_SIZE];

#define CIE_ILL_WAVELENGTH_OFFSET 300
#define CIE_ILL_DATA_SIZE 531
extern struct {
  int wavelength;
  double d65;
} cie_ill[CIE_ILL_DATA_SIZE];
