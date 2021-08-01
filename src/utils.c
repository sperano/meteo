#include <conio.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gfx.h"
#include "utils.h"

char* alloc_copy(const char *src) {
    char *dest = safe_malloc(strlen(src) + 1);
    strcpy(dest, src);
    return dest;
}

uint16_t str_to_int(const char *src) {
    uint16_t value = 0;
    uint8_t dec = 0;
    char *ptr_token = strchr(src, '.');
    if (ptr_token == NULL) {
        value = atoi(src);
    } else {
        *ptr_token = 0;
        value = atoi(src);
        dec = ptr_token[1] - '0';
        if (dec >= 5) {
            value++;
        }
    }
    return value;
}

// 273.15 == 27315
kelvin str_to_kelvin(const char *src) {
    uint16_t value = 0;
    char *ptr_token = strchr(src, '.');
    if (ptr_token == NULL) {
        // no decimal
        return atoi(src) * 100;
    }
    *ptr_token = 0;
    ptr_token++;
    value = atoi(ptr_token);
    if (strlen(ptr_token) == 1) {
        value *= 10;
    }
    return (atoi(src) * 100) + value;
}

// 2 decimals to 1 decimal
celsius kelvin_to_celsius(const kelvin temp) {
    return (temp - 27315 + 5) / 10;
}

// 1 decimal to no decimal
fahrenheit celsius_to_fahrenheit(const celsius temp) {
    return (((temp * 18) + 50)/100)+32;
}

void celsius_str(char *buffer, const celsius temp) {
    uint8_t i, j;
    i = temp / 10;
    j = temp % 10;
    if (j == 0) {
        snprintf(buffer, 5, "%d", i);
    } else {
        snprintf(buffer, 5, "%d.%d", i, j);
    }
}

void fail(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (errno) {
        perror("Error");
    }
    vprintf(fmt, args);
    va_end(args);
    printf("Press any key to exit.\n");
#ifndef NOCONSOLE
    cgetc();
#endif
    exit(1);
}

void* safe_malloc(size_t size) {
    void *p = malloc(size);
    if (p == NULL) {
        fail("safe_malloc size: %d\n", size);
    }
    return p;
}

void* safe_realloc(void *ptr, size_t size) {
    void *p = realloc(ptr, size);
    if (p == NULL) {
        fail("safe_realloc size: %d\n", size);
    }
    return p;
}

const char *utf8_to_ascii(const char *str) {
    char *str1 = (char *)str;
    char *str2 = (char *)str;
    while (*str2) {
        switch(*str2) {
        case 0xc3:
            str2++;
            switch(*str2) {
            case 0xa9:
                *str1 = 'e';
                break;
            default:
                *str1 = 0xc3;
            str1++;
                *str1 = *str2;
                break;
            }
            break;
        default:
            *str1 = *str2;
            break;
        }
        str1++;
        str2++;
    }
    *str1 = 0;
    return str;
}

void draw_menu(Menu *menu, void *ctx) {
    char buffer[41];
    uint8_t i = 0, j;
    uint8_t item_width = 0;
    char c;
    for (; i < menu->total_items; ++i) {
        j = strlen(menu->items[i].name);
        if (j > item_width) {
            item_width = j;
        }
    }
    for (i = 0; i < menu->total_items; ++i) {
        // skip separator or invisibile
        if (menu->items[i].name[0] != '-' &&
           (menu->items[i].visibility_check == NULL || menu->items[i].visibility_check(ctx))) {
            snprintf(buffer, 40, "%-*s", item_width, menu->items[i].name);
            if (i == menu->selected) {
                for (j = 0; j < item_width; ++j) {
                    c = buffer[j];
                    if (isupper(c) || c == '[' || c == ']') {
                        buffer[j] -= 0x40;
                    }
                }
            } else {
                for (j = 0; j < item_width; ++j) {
                    buffer[j] += 0x80;
                }
            }
            memcpy((void *)(VideoBases[menu->y_pos + ( i* (1 + menu->interlines))] + menu->left_pad), buffer, item_width);
        }
    }
}

uint8_t do_menu(Menu *menu, void *ctx) {
    uint8_t rc = 0;
    uint8_t i = 0;
#ifndef NOCONSOLE
    if (menu->init) {
        menu->init(menu, ctx);
    }
    while (rc == 0) {
        draw_menu(menu, ctx);
        switch (cgetc()) {
        case KeyLeftArrow:
        case KeyUpArrow:
            do {
                menu->selected = menu->selected ? menu->selected - 1 : menu->total_items - 1;
            } while (menu->items[menu->selected].name[0] == '-' ||
                    (menu->items[menu->selected].visibility_check != NULL && !menu->items[menu->selected].visibility_check(ctx)));
            break;
        case KeyRightArrow:
        case KeyDownArrow:
        case '\t':
            do {
                menu->selected = menu->selected - (menu->total_items - 1) ? menu->selected + 1 : 0;
            } while (menu->items[menu->selected].name[0] == '-' ||
                    (menu->items[menu->selected].visibility_check != NULL && !menu->items[menu->selected].visibility_check(ctx)));
            break;
        case ' ':
        case '\r':
            rc = menu->items[menu->selected].action(ctx, menu->selected, 0);
            if (rc == 0 && menu->init) {
                menu->init(menu, ctx);
            }
            break;
        }
    }
#endif
    return rc;
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
