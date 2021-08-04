#ifndef _UI_H
#define _UI_H

//#include <stdio.h>

#include "types.h"

#define KeyLeftArrow 0x08
#define KeyRightArrow 0x15
#define KeyUpArrow 0x0b
#define KeyDownArrow 0x0a
#define KeyEscape 0x1b

#define MENU_LEFT_PAD 2
#define MENU_INTERLINES 0

uint8_t max_item_name_length(MenuItem *items, uint8_t total);

//ActionResult do_menu(Menu *menu, void *ctx);

ActionResult do_menu(uint8_t y, uint8_t *selected, MenuItem *items, uint8_t total_items, MenuInit init, void *ctx);

int8_t text_input(uint8_t x, uint8_t y, uint8_t len, char *dest, char *src, uint8_t flags);

#endif
