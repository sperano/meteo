#include <conio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "gfx.h"
#include "ui.h"
#include "utils.h"

uint8_t max_item_name_length(MenuItem *items, uint8_t total) {
    uint8_t max = 0, i, j;
    for (i = 0; i < total; ++i) {
        j = strlen(items[i].name);
        if (j > max) {
            max = j;
        }
    }
    return max;
}

void _draw_menu(uint8_t y, uint8_t selected, MenuItem *items, uint8_t total_items, void *ctx) {
    char buffer[41];
    uint8_t i, j;
    // so we know how long we print we reverse color
    uint8_t max_length = max_item_name_length(items, total_items);
    char c;
    for (i = 0; i < total_items; ++i) {
        // skip separator or invisibile
        if (items[i].name[0] != '-' &&
           (items[i].visibility_check == NULL || items[i].visibility_check(ctx))) {
            snprintf(buffer, 40, "%-*s", max_length, items[i].name);
            if (i == selected) {
                for (j = 0; j < max_length; ++j) {
                    c = buffer[j];
                    if (isupper(c) || c == '[' || c == ']') {
                        buffer[j] -= 0x40;
                    }
                }
            } else {
                for (j = 0; j < max_length; ++j) {
                    buffer[j] += 0x80;
                }
            }
            memcpy((void *)(VideoBases[y + i] + MENU_LEFT_PAD), buffer, max_length);
        }
    }
}

ActionResult do_menu(uint8_t y, uint8_t *selected, MenuItem *items, uint8_t total_items, MenuInit init, void *ctx) {
//ActionResult do_menu(Menu *menu, void *ctx) {
    bool action_executed = false;
    ActionResult ar;
#ifndef NOCONSOLE
    if (init) {
        init(ctx);
    }
    while (!action_executed) {
        _draw_menu(y, *selected, items, total_items, ctx);
        switch (cgetc()) {
        case KeyLeftArrow:
        case KeyUpArrow:
            do {
                *selected = *selected ? *selected - 1 : total_items - 1;
            } while (items[*selected].name[0] == '-' ||
                    (items[*selected].visibility_check != NULL && !items[*selected].visibility_check(ctx)));
            break;
        case KeyRightArrow:
        case KeyDownArrow:
        case '\t':
            do {
                *selected = (*selected - (total_items - 1)) ? *selected + 1 : 0;
            } while (items[*selected].name[0] == '-' ||
                    (items[*selected].visibility_check != NULL && !items[*selected].visibility_check(ctx)));
            break;
        case ' ':
        case '\r':
            ar = items[*selected].action(ctx, *selected, 0);
            action_executed = true;
            break;
        }
    }
#endif
    return ar;
}

int8_t text_input(uint8_t x, uint8_t y, uint8_t len, char *dest, char *src, uint8_t flags) {
#ifndef NOCONSOLE
    void *screen_ptr = (void*)(VideoBases[y]+x);
    bool stop = false;
    char ch;
    char *work_copy = safe_malloc(len);
    uint8_t pos = 0;

    memcpy(work_copy, src, len);
    for (ch = 0; ch < len; ++ch) {
        if (work_copy[ch] == 0) {
            work_copy[ch] = ' ';
        } else if (work_copy[ch] >= 'A' && work_copy[ch] <= 'F') {
            work_copy[ch] -= 0x40;
        }
    }
    while (!stop) {
        work_copy[pos] = work_copy[pos] += 0x80;
        memcpy(screen_ptr, work_copy, len);
        ch = cgetc();
        if (((flags & ACCEPT_HEXA) || (flags & ACCEPT_NUMBER)) && ch >= '0' && ch <= '9') {
            work_copy[pos] = ch;
            pos = pos == len - 1 ? len - 1 : pos + 1;
        } else if ((flags & ACCEPT_HEXA) && ((ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))) {
            work_copy[pos] = toupper(ch) - 0x40;
            pos = pos == len - 1 ? len - 1 : pos + 1;
        } else {
            switch (ch) {
            case ' ':
                work_copy[pos] -= 0x80;
                if (flags & ACCEPT_SPACE) {
                    work_copy[pos] = ' ';
                }
                pos = pos == len - 1 ? len - 1 : pos + 1;
                break;
            case '\r':
                work_copy[pos] -= 0x80;
                stop = true;
                break;
            case KeyLeftArrow:
                work_copy[pos] -= 0x80;
                if (pos) {
                    pos--;
                }
                break;
            case KeyRightArrow:
                work_copy[pos] -= 0x80;
                if (pos < len - 1) {
                    pos++;
                }
                break;
            case KeyEscape:
                //if (flags & ESCAPE_TO_EXIT) {
                //    exit(1);
                //}
                if (flags & ACCEPT_ESCAPE) {
                    return -1;
                }
                break;
            default:
                work_copy[pos] -= 0x80;
            }
        }
    }
    for (ch = 0; ch < len; ++ch) {
        dest[ch] = work_copy[ch];
        if (dest[ch] >= ('A'-0x40) && dest[ch] <= ('F'-0x40)) {
            dest[ch] += 0x40;
        } else if (dest[ch] == ' ') {
            dest[ch] = 0;
        }
    }
    free(work_copy);
#endif
}
