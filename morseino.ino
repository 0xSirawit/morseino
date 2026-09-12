#include "morseino.h"

volatile SystemState currentState = STATE_IDLE;
volatile bool ledFlag = false;
volatile bool buzFlag = false;

TaskHandle_t commsTaskHandle = NULL;

DebouncedButton btn1(PIN_BT1, BUTTON_PRESSED);
DebouncedButton btn2(PIN_BT2, BUTTON_PRESSED);
DebouncedButton btn3(PIN_BT3, BUTTON_PRESSED);
DebouncedButton btn4(PIN_BT4, BUTTON_PRESSED);

void setup() {
  Serial.begin(9600);

  btn1.begin();
  btn2.begin();
  btn3.begin();
  btn4.begin();

  pinMode(PIN_P1, INPUT);
  pinMode(PIN_P2, INPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUZ, OUTPUT);
  
  // Core Tasks
  xTaskCreate(LEDTask, "LED_Task", 45, NULL, 1, NULL);
  xTaskCreate(BUZTask, "BUZ_Task", 45, NULL, 1, NULL);
  xTaskCreate(MainTask, "Main_Task", 128, NULL, 1, NULL);
  xTaskCreate(DebugTask, "Debug_Task", 128, NULL, 1, NULL);

  // Suspendable Tasks
  xTaskCreate(CommsTask, "Comms_Task", 100, NULL, 1, &commsTaskHandle);
  vTaskSuspend(commsTaskHandle);
}

void DebugTask(void *pvParameters) {
  for (;;) {
    Serial.print("State: ");
    Serial.println(currentState);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void LEDTask(void *pvParameters) {
  for (;;) {
    if (ledFlag) {
      digitalWrite(PIN_LED, 1);
    } else {
      digitalWrite(PIN_LED, 0);
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void BUZTask(void *pvParameters) {
  for (;;) {
    if (buzFlag) {
      tone(PIN_BUZ, BUZTONE);
    } else {
      noTone(PIN_BUZ);
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void CommsTask(void *pvParameters) {
  for (;;) {
    if (btn4.isPressed()) {
      buzFlag = 1;
      ledFlag = 1;
    } else {
      buzFlag = 0;
      ledFlag = 0;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void MainTask(void *pvParameters) {
  for(;;) {
    switch (currentState) {
      case STATE_IDLE:
        if (btn1.isPressed()) {
          currentState = STATE_NORMAL;
          
          vTaskResume(commsTaskHandle);
        }
      break;

      case STATE_NORMAL:
        if (btn2.isPressed()) {
          currentState = STATE_IDLE;
          
          vTaskSuspend(commsTaskHandle);
          
          ledFlag = 0;
          buzFlag = 0;
        }
      break;

      case STATE_PRACTICE:
      break;

      case STATE_LOG:
      break;

      case STATE_SETTING:
      break;
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void loop() {
}