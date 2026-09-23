#ifndef MORSEINO_H
#define MORSEINO_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <U8g2lib.h>
#include <ESP32Encoder.h>
#include <Wire.h>
#include "debounce_button.h"
#include "morse_utils.h"
#include "oled_n_bitmaps.h"

#define PIN_BT4 19
#define PIN_BT1 18
#define PIN_BT2 13
#define PIN_BT3 14 
#define PIN_LED 17
#define PIN_RE_DT 34
#define PIN_RE_CLK 35
#define PIN_BUZ 16
#define PIN_SW1 26

#define BUZTONE 1000

#define BUTTON_PRESSED LOW
#define BUTTON_RELEASED HIGH

enum SystemState {
  STATE_IDLE,
  STATE_NORMAL,
  STATE_PRACTICE,
  STATE_LOG,
  STATE_SETTING,
  STATE_HELP,
  // In setting state
  STATE_DEVICENAME,
  STATE_UNITTIME,
  STATE_TONE
  
};

extern const SystemState stateSettingLookup[];
extern volatile SystemState settingSelState;
extern volatile int settingSelected;
extern volatile int settingSelectPrevious;
extern volatile int settingSelectNext;

extern volatile float unitTime;

extern volatile int letterSelected;
extern volatile int letterSelectPrevious0;
extern volatile int letterSelectPrevious1;
extern volatile int letterSelectPrevious2;
extern volatile int letterSelectNext2;
extern volatile int letterSelectNext1;
extern volatile int letterSelectNext0;

extern volatile int numberSelected;
extern volatile int numberSelectPrevious0;
extern volatile int numberSelectPrevious1;
extern volatile int numberSelectPrevious2;
extern volatile int numberSelectNext2;
extern volatile int numberSelectNext1;
extern volatile int numberSelectNext0;

extern const SystemState stateLookup[];
extern volatile SystemState currentState;
extern volatile SystemState selState;
extern volatile bool ledFlag;
extern volatile bool buzFlag;
extern volatile uint8_t caesarKey;
extern volatile char practice_challengingNum;
extern volatile bool practice_correctFlag;

extern volatile int item_selected;
extern volatile int item_sel_previous;
extern volatile int item_sel_next;

extern volatile int help_line;

extern TaskHandle_t commsTaskHandle;
extern TaskHandle_t practiceTaskHandle;

extern byte SPEAKER[8];
extern byte MUTESPEAKER[8];
extern byte UNMUTESPEAKER[8];
extern byte LOCK[8];
extern byte UNLOCK[8];

void DebugTask(void *pvParameters);
void LEDTask(void *pvParameters);
void BUZTask(void *pvParameters);
void LCDDisplayTask(void *pvParameters);
void OLEDDisplayTask(void *pvParameters);
void RotaryEncoderTask(void *pvParameters);
void CommsTask(void *pvParameters);
void MainTask(void *pvParameters);

#endif
