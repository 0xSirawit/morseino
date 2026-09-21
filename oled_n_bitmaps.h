#ifndef OLED_N_BITMAPS_H
#define OLED_N_BITMAPS_H

#include <Arduino.h>

#define NUM_ITEMS 5
#define MAX_ITEM_LENGTH 9
#define NUM_HELP_LINES 14
#define MAX_HELP_LINE_LENGTH 21

extern const unsigned char bitmap_icon_signaling[];
extern const unsigned char bitmap_icon_practice[];
extern const unsigned char bitmap_icon_log[];
extern const unsigned char bitmap_icon_settings[];
extern const unsigned char bitmap_icon_help[];

extern const unsigned char bitmap_item_sel_outline[];
extern const unsigned char bitmap_scrollbar_background[];

extern const unsigned char* const bitmap_icons[NUM_ITEMS] PROGMEM;
extern const char menu_items[NUM_ITEMS][MAX_ITEM_LENGTH];
extern const char morsecode_cs[NUM_HELP_LINES][MAX_HELP_LINE_LENGTH];

#endif