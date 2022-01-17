CC = gcc
RM = rm -f
CFLAGS = -O6 -Wall -std=c99 -pedantic -W -Wwrite-strings -Wcast-qual \
  -Wpointer-arith -Wstrict-prototypes -Wno-unused -Wno-unused-parameter
LDLIBS = -lm -lpng

all: spectrum.png antispectrum.png speclum.png blackbody.png \
  xydiagram-dark.png xydiagram-light.png \
  huediagram-520.png huediagram-550.png huediagram-570.png \
  vision.png gammatest.png conecol.png conecol2.png \
  sample-color.png sample-saturated.png sample-hue.png \
  sample-protanope.png sample-protanomalous.png \
  sample-deuteranope.png sample-deuteranomalous.png \
  sample-tritanope.png sample-tritanomalous.png \
  sample-long.png sample-medium.png sample-short.png sample-lum.png \
  sample2-color.png sample2-saturated.png sample2-hue.png \
  sample2-protanope.png sample2-protanomalous.png \
  sample2-deuteranope.png sample2-deuteranomalous.png \
  sample2-tritanope.png sample2-tritanomalous.png \
  sample2-long.png sample2-medium.png sample2-short.png sample2-lum.png \
  srgbtocie cietosrgb huetosrgb blackbodytosrgb rayleighjeanstosrgb

clean:
	-$(RM) data.o library.o
	-$(RM) makespectrum.o makeantispectrum.o makespeclum.o makeblackbody.o \
	  makevision.o makegammatest.o makeconecol.o makeconecol2.o \
	  filtercolor.o filtervision.o splitvision.o \
	  srgbtocie.o cietosrgb.o huetosrgb.o blackbodytosrgb.o rayleighjeanstosrgb.o
	-$(RM) makexydiagram.o makehuediagram.o
	-$(RM) makespectrum makeantispectrum makespeclum makeblackbody \
	  makevision makegammatest makeconecol makeconecol2 \
	  filtercolor filtervision splitvision \
	  srgbtocie cietosrgb huetosrgb blackbodytosrgb rayleighjeanstosrgb
	-$(RM) makexydiagram makehuediagram
	-$(RM) spectrum.png antispectrum.png speclum.png blackbody.png \
	  xydiagram-dark.png xydiagram-light.png \
	  huediagram-520.png huediagram-550.png huediagram-570.png \
	  vision.png gammatest.png conecol.png conecol2.png \
	  sample-color.png sample-saturated.png sample-hue.png \
	  sample-protanope.png sample-protanomalous.png \
	  sample-deuteranope.png sample-deuteranomalous.png \
	  sample-tritanope.png sample-tritanomalous.png \
	  sample-long.png sample-medium.png sample-short.png sample-lum.png \
	  sample2-color.png sample2-saturated.png sample2-hue.png \
	  sample2-protanope.png sample2-protanomalous.png \
	  sample2-deuteranope.png sample2-deuteranomalous.png \
	  sample2-tritanope.png sample2-tritanomalous.png \
	  sample2-long.png sample2-medium.png sample2-short.png sample2-lum.png
	-$(RM) srgbtocie
	-$(RM) cietosrgb
	-$(RM) huetosrgb blackbodytosrgb rayleighjeanstosrgb

makespectrum.o: data.h library.h
makeantispectrum.o: data.h library.h
makespeclum.o: data.h library.h
makeblackbody.o: data.h library.h
makexydiagram.o: library.h
makehuediagram.o: data.h library.h
makevision.o: data.h library.h
makegammatest.o: data.h library.h
makeconecol.o: data.h library.h
makeconecol2.o: library.h
filtercolor.o: data.h library.h
filtervision.o: data.h library.h
splitvision.o: data.h library.h
srgbtocie.o: library.h
cietosrgb.o: library.h
huetosrgb.o: library.h
blackbodytosrgb.o: library.h
rayleighjeanstosrgb.o: library.h

makespectrum: makespectrum.o data.o library.o
makeantispectrum: makeantispectrum.o data.o library.o
makespeclum: makespeclum.o data.o library.o
makeblackbody: makeblackbody.o data.o library.o
makexydiagram: makexydiagram.o data.o library.o
makehuediagram: makehuediagram.o data.o library.o
makevision: makevision.o data.o library.o
makegammatest: makegammatest.o data.o library.o
makeconecol: makeconecol.o data.o library.o
makeconecol2: makeconecol2.o data.o library.o
filtercolor: filtercolor.o data.o library.o
filtervision: filtervision.o data.o library.o
splitvision: splitvision.o data.o library.o
srgbtocie: srgbtocie.o data.o library.o
cietosrgb: cietosrgb.o data.o library.o
huetosrgb: huetosrgb.o data.o library.o
blackbodytosrgb: blackbodytosrgb.o data.o library.o
rayleighjeanstosrgb: rayleighjeanstosrgb.o data.o library.o

spectrum.png: makespectrum
	./makespectrum > spectrum.png
antispectrum.png: makeantispectrum
	./makeantispectrum > antispectrum.png
speclum.png: makespeclum
	./makespeclum > speclum.png
blackbody.png: makeblackbody
	./makeblackbody > blackbody.png
xydiagram-dark.png: makexydiagram
	./makexydiagram 0.1 > xydiagram-dark.png
xydiagram-light.png: makexydiagram
	./makexydiagram 0.8 > xydiagram-light.png
huediagram-520.png: makehuediagram
	./makehuediagram 520 > huediagram-520.png
huediagram-550.png: makehuediagram
	./makehuediagram 550 > huediagram-550.png
huediagram-570.png: makehuediagram
	./makehuediagram 570 > huediagram-570.png
vision.png: makevision
	./makevision > vision.png
gammatest.png: makegammatest
	./makegammatest > gammatest.png
conecol.png: makeconecol
	./makeconecol > conecol.png
conecol2.png: makeconecol2
	./makeconecol2 > conecol2.png

sample-color.png: filtercolor sample.png
	./filtercolor c < sample.png > sample-color.png
sample-saturated.png: filtercolor sample.png
	./filtercolor H < sample.png > sample-saturated.png
sample-hue.png: filtercolor sample.png
	./filtercolor h < sample.png > sample-hue.png
sample-protanope.png: filtervision sample.png
	./filtervision l < sample.png > sample-protanope.png
sample-protanomalous.png: filtervision sample.png
	./filtervision L < sample.png > sample-protanomalous.png
sample-deuteranope.png: filtervision sample.png
	./filtervision m < sample.png > sample-deuteranope.png
sample-deuteranomalous.png: filtervision sample.png
	./filtervision M < sample.png > sample-deuteranomalous.png
sample-tritanope.png: filtervision sample.png
	./filtervision s < sample.png > sample-tritanope.png
sample-tritanomalous.png: filtervision sample.png
	./filtervision S < sample.png > sample-tritanomalous.png
sample-long.png: splitvision sample.png
	./splitvision l < sample.png > sample-long.png
sample-medium.png: splitvision sample.png
	./splitvision m < sample.png > sample-medium.png
sample-short.png: splitvision sample.png
	./splitvision s < sample.png > sample-short.png
sample-lum.png: splitvision sample.png
	./splitvision y < sample.png > sample-lum.png

sample2-color.png: filtercolor sample.png
	./filtercolor c < sample2.png > sample2-color.png
sample2-saturated.png: filtercolor sample.png
	./filtercolor H < sample2.png > sample2-saturated.png
sample2-hue.png: filtercolor sample.png
	./filtercolor h < sample2.png > sample2-hue.png
sample2-protanope.png: filtervision sample2.png
	./filtervision l < sample2.png > sample2-protanope.png
sample2-protanomalous.png: filtervision sample2.png
	./filtervision L < sample2.png > sample2-protanomalous.png
sample2-deuteranope.png: filtervision sample2.png
	./filtervision m < sample2.png > sample2-deuteranope.png
sample2-deuteranomalous.png: filtervision sample2.png
	./filtervision M < sample2.png > sample2-deuteranomalous.png
sample2-tritanope.png: filtervision sample2.png
	./filtervision s < sample2.png > sample2-tritanope.png
sample2-tritanomalous.png: filtervision sample2.png
	./filtervision S < sample2.png > sample2-tritanomalous.png
sample2-long.png: splitvision sample2.png
	./splitvision l < sample2.png > sample2-long.png
sample2-medium.png: splitvision sample2.png
	./splitvision m < sample2.png > sample2-medium.png
sample2-short.png: splitvision sample2.png
	./splitvision s < sample2.png > sample2-short.png
sample2-lum.png: splitvision sample2.png
	./splitvision y < sample2.png > sample2-lum.png
