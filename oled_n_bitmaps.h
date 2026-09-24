#ifndef OLED_N_BITMAPS_H
#define OLED_N_BITMAPS_H

#include <Arduino.h>

#define NUM_SETTING_ITEM 3
#define MAX_SETTING_ITEM_LENGTH 12
#define NUM_LETTER_ITEM 36
#define MAX_LETTER_ITEM_LENGTH 2

#define NUM_NUMBER_ITEM 10
#define MAX_NUMBER_ITEM_LENGTH 2

#define NUM_ITEMS 5
#define MAX_ITEM_LENGTH 9
#define NUM_HELP_LINES 14
#define MAX_HELP_LINE_LENGTH 21

#define NUM_TONE_ITEM 15
#define MAX_TONE_ITEM_LENGTH 3
#define MAX_NAME_TONE_ITEM_LENGTH 8

extern const char toneItems[NUM_TONE_ITEM][MAX_TONE_ITEM_LENGTH];
extern const char nameToneItems[NUM_TONE_ITEM][MAX_NAME_TONE_ITEM_LENGTH];
extern const unsigned char bitmapToneSelOutline [] PROGMEM;

extern const char numberItems[NUM_NUMBER_ITEM][MAX_NUMBER_ITEM_LENGTH];

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



extern const unsigned char settingMapIconProfile[];
extern const unsigned char settingMapIconUnitTime[];
extern const unsigned char settingMapVolume[];

extern const unsigned char bitmapSettingSelOutline[];
extern const unsigned char bitmapLetterSelOutline[];

extern const char letterItems[NUM_LETTER_ITEM][MAX_LETTER_ITEM_LENGTH];
extern const unsigned char* const settingmapIncons[NUM_SETTING_ITEM] PROGMEM;
extern const char settingItems[NUM_SETTING_ITEM][MAX_SETTING_ITEM_LENGTH];

#endif