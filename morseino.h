#ifndef MORSEINO_H
#define MORSEINO_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "debounce_button.h"
#include "morse_utils.h"

#define PIN_BT4 19
#define PIN_BT1 18
#define PIN_BT2 13
#define PIN_BT3 14 
#define PIN_LED 17
#define PIN_P1 34
#define PIN_P2 35
#define PIN_BUZ 16

#define BUZTONE 1000

#define BUTTON_PRESSED LOW
#define BUTTON_RELEASED HIGH

enum SystemState {
  STATE_IDLE,
  STATE_NORMAL,
  STATE_PRACTICE,
  STATE_LOG,
  STATE_SETTING,
};

extern volatile SystemState currentState;
extern volatile bool ledFlag;
extern volatile bool buzFlag;
extern TaskHandle_t commsTaskHandle;

// TODO: MUTESPEAKER ICON
byte SPEAKER[] = {B00001, B00011, B01111, B01111, B01111, B00011, B00001, B00000};
byte MUTESPEAKER[] = {B00001, B00011, B01111, B01111, B01111, B00011, B00001, B00000};
byte LOCK[] = {B01110, B10001, B10001, B11111, B11011, B11011, B11111, B00000};
byte UNLOCK[] = {B01110, B10000, B10000, B11111, B11011, B11011, B11111, B00000};

void DebugTask(void *pvParameters);
void LEDTask(void *pvParameters);
void BUZTask(void *pvParameters);
void CommsTask(void *pvParameters);
void MainTask(void *pvParameters);

#endif
