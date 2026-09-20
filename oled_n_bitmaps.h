#ifndef OLED_N_BITMAPS_H
#define OLED_N_BITMAPS_H

#include <Arduino.h>

#define NUM_ITEMS 4
#define MAX_ITEM_LENGTH 10

extern const unsigned char bitmap_icon_signaling[];
extern const unsigned char bitmap_icon_practice[];
extern const unsigned char bitmap_icon_log[];
extern const unsigned char bitmap_icon_settings[];

extern const unsigned char bitmap_item_sel_outline[];
extern const unsigned char bitmap_scrollbar_background[];

extern const unsigned char* const bitmap_icons[NUM_ITEMS] PROGMEM;
extern const char menu_items[NUM_ITEMS][MAX_ITEM_LENGTH];

#endif