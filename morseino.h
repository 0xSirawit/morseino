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

#define NOTE_De 1000  // Default

#define NOTE_C4  262  // Do
#define NOTE_D4  294  // Re
#define NOTE_E4  330  // Mi
#define NOTE_F4  349  // Fa
#define NOTE_G4  392  // Sol
#define NOTE_A4  440  // La
#define NOTE_B4  494  // Ti

#define NOTE_C5  523  // Do
#define NOTE_D5  587  // Re
#define NOTE_E5  659  // Mi
#define NOTE_F5  698  // Fa
#define NOTE_G5  784  // Sol
#define NOTE_A5  880  // La
#define NOTE_B5  988  // Ti




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
  STATE_SETTING_DEVICENAME,
  STATE_SETTING_UNITTIME,
  STATE_SETTING_TONE
  
};

extern volatile int globalBuzTone;
extern volatile int toneSelected;
extern volatile int toneSelectPrevious0;
extern volatile int toneSelectPrevious1;
extern volatile int toneSelectPrevious2;
extern volatile int toneSelectNext2;
extern volatile int toneSelectNext1;
extern volatile int toneSelectNext0;
extern const int toneFrequencies[NUM_TONE_ITEM];

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
