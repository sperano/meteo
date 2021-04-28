#ifndef _GFX_H
#define _GFX_H

#include "types.h"

#define _80STORE 0xc001
#define TEXTOFF 0xc050
#define TEXTON 0xc051
#define MIXEDOFF 0xc052
#define MIXEDON 0xc053
#define PAGE2OFF 0xc054
#define PAGE2ON 0xc055
#define HIRESOFF 0xc056
#define HIRESON 0xc057

void init_gfx(void);
void exit_gfx(void);
void clear_screen(void);
void set_menu_text(void);
void update_gfx_text(CityWeather *cw, enum Units units);
void update_gfx_image(CityWeather *cw);
Bitmap get_bitmap_for_icon(char *icon);

#endif
