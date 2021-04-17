#ifndef _METEO_H
#define _METEO_H

#define METEO_VERSION "0.1"

#include <stdint.h>
#include <stdio.h>

#define _80STORE 0xc001
#define TEXTOFF 0xc050
#define TEXTON 0xc051
#define MIXEDOFF 0xc052
#define MIXEDON 0xc053
#define PAGE2OFF 0xc054
#define PAGE2ON 0xc055
#define HIRESOFF 0xc056
#define HIRESON 0xc057

extern uint8_t Bitmap[20][40];

extern char TxtLine1[41];
extern char TxtLine2[41];
extern char TxtLine3[41];
extern char TxtLine4[41];

#endif
