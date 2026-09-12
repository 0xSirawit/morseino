#ifndef MORSEINO_H
#define MORSEINO_H

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include "debounce_button.h"

#define PIN_BT4 2
#define PIN_BT1 5
#define PIN_BT2 6 
#define PIN_BT3 7 
#define PIN_LED 8
#define PIN_P1 14 
#define PIN_P2 15
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

extern TaskHandle_t txTaskHandle;
extern TaskHandle_t rxTaskHandle;

void DebugTask(void *pvParameters);
void LEDTask(void *pvParameters);
void BUZTask(void *pvParameters);
void TxTask(void *pvParameters);
void RxTask(void *pvParameters);
void MainTask(void *pvParameters);

#endif
