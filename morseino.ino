#include "morseino.h"

volatile SystemState currentState = STATE_IDLE;
volatile bool ledFlag = false;
volatile bool buzFlag = false;
volatile uint8_t caesarKey = 0;

String globalSeqBuffer = "";
String globalMessageBuffer = "";
SemaphoreHandle_t seqBufferMutex = NULL;
SemaphoreHandle_t i2cMutex = NULL;

TaskHandle_t commsTaskHandle = NULL;

DebouncedButton btn1(PIN_BT1, BUTTON_PRESSED);
DebouncedButton btn2(PIN_BT2, BUTTON_PRESSED);
DebouncedButton btn3(PIN_BT3, BUTTON_PRESSED);
DebouncedButton btn4(PIN_BT4, BUTTON_PRESSED);

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

void setup() {
  Serial.begin(115200);

  seqBufferMutex = xSemaphoreCreateMutex();
  i2cMutex = xSemaphoreCreateMutex();

  Wire.begin();
  u8g2.begin();

  btn1.begin();
  btn2.begin();
  btn3.begin();
  btn4.begin();

  pinMode(PIN_P1, INPUT);
  pinMode(PIN_P2, INPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUZ, OUTPUT);

  // Core Tasks
  xTaskCreate(LEDTask, "LED_Task", 2048, NULL, 1, NULL);
  xTaskCreate(BUZTask, "BUZ_Task", 2048, NULL, 1, NULL);
  xTaskCreate(LCDDisplayTask, "LCDDisplay_Task", 2048, NULL, 1, NULL);
  xTaskCreate(OLEDDisplayTask, "OLEDDisplay_Task", 8192, NULL, 1, NULL);
  xTaskCreate(MainTask, "Main_Task", 2048, NULL, 1, NULL);
  xTaskCreate(DebugTask, "Debug_Task", 2048, NULL, 1, NULL);

  // Suspendable Tasks
  xTaskCreate(CommsTask, "Comms_Task", 2048, NULL, 1, &commsTaskHandle);
  vTaskSuspend(commsTaskHandle);
}

void DebugTask(void *pvParameters) {
  for (;;) {
    // Serial.print("State: ");
    // Serial.println(currentState);
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
  bool lastBuzState = false;

  for (;;) {
    if (buzFlag && !lastBuzState) {
      tone(PIN_BUZ, BUZTONE);
      lastBuzState = true;
    }

    else if (!buzFlag && lastBuzState) {
      noTone(PIN_BUZ);
      lastBuzState = false;
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void OLEDDisplayTask(void *pvParameters) {
  int p1_value = 0;
  char textBuffer[16];

  for (;;) {
    p1_value = analogRead(PIN_P1);

    if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_ncenB08_tr);

      u8g2.drawStr(0, 10, "Hello World!");

      sprintf(textBuffer, "P1: %d", p1_value);
      u8g2.drawStr(0, 25, textBuffer);

      u8g2.sendBuffer();

      xSemaphoreGive(i2cMutex);
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void LCDDisplayTask(void *pvParameters) {
  TickType_t blinkTime = xTaskGetTickCount();
  bool blinkState = 0;
  String displayBuffer = "";
  String messageDisplay = "";
  String lastDisplayBuffer = "";
  String lastMessageDisplay = "";

  SystemState lastState = (SystemState)-1;

  LiquidCrystal_I2C lcd(0x27, 16, 2);

  xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(50));

  lcd.init();
  lcd.backlight();

  lcd.createChar(0, SPEAKER);
  lcd.createChar(1, MUTESPEAKER);
  lcd.createChar(2, LOCK);
  lcd.createChar(3, UNLOCK);

  xSemaphoreGive(i2cMutex);

  for (;;) {
    if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(50)) == pdTRUE) {

      if (currentState != lastState) {
        lcd.clear();
        lastState = currentState;
      }

      switch (currentState) {
        case STATE_IDLE:
          lcd.setCursor(0, 0);
          lcd.print("MORSEINO");
          break;

        case STATE_NORMAL:
          lcd.setCursor(10, 0);
          if (caesarKey == 0) {
            lcd.write(byte(3));
          } else {
            lcd.write(byte(2));
          }
          

          if (caesarKey < 10) {
            lcd.print(0);
          }

          lcd.print(caesarKey);

          lcd.setCursor(14, 0);
          lcd.write(byte(0));

          if (xSemaphoreTake(seqBufferMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            displayBuffer = globalSeqBuffer;
            messageDisplay = globalMessageBuffer;  // อ่านพร้อมกันในล็อกเดียวกัน
            xSemaphoreGive(seqBufferMutex);
          }

          if (displayBuffer.length() > 5) {
            displayBuffer = displayBuffer.substring(0, 5);
          }

          if (messageDisplay.length() > 16) {
            messageDisplay = messageDisplay.substring(messageDisplay.length() - 16);  // ตัดเอา 16 ตัวท้าย
          }

          if (displayBuffer != lastDisplayBuffer) {
            lcd.setCursor(1, 0);
            lcd.print("     ");
            lcd.setCursor(1, 0);
            lcd.print(displayBuffer);
            lastDisplayBuffer = displayBuffer;
          }

          if (messageDisplay != lastMessageDisplay) {
            lcd.setCursor(0, 1);
            lcd.print("                ");
            lcd.setCursor(0, 1);
            lcd.print(messageDisplay);
            lastMessageDisplay = messageDisplay;
          }

        case STATE_PRACTICE:
        case STATE_LOG:
        case STATE_SETTING:
          break;
      }

      xSemaphoreGive(i2cMutex);
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void CommsTask(void *pvParameters) {
  TickType_t pressStartTick = 0;
  TickType_t releaseStartTick = 0;
  float unitTime = 100.0;
  String localSeqBuffer = "";

  for (;;) {
    caesarKey = map(analogRead(PIN_P1), 0, 4095, 0, 35);
    if (btn4.isPressed()) {
      buzFlag = 1;
      ledFlag = 1;
      pressStartTick = xTaskGetTickCount();

      while (btn4.isPressed()) {
        vTaskDelay(pdMS_TO_TICKS(5));
      }

      buzFlag = false;
      ledFlag = false;
      releaseStartTick = xTaskGetTickCount();

      unsigned long duration = (releaseStartTick - pressStartTick) * portTICK_PERIOD_MS;
      char symbol = (duration < (unsigned long)(unitTime * 2.0)) ? '.' : '-';

      localSeqBuffer = localSeqBuffer + symbol;

      if (xSemaphoreTake(seqBufferMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        globalSeqBuffer = localSeqBuffer;
        xSemaphoreGive(seqBufferMutex);
      }

      if (symbol == '.') {
        Serial.print(".");
        unitTime = (unitTime * 3.0 + (float)duration) / 4.0;
      } else {
        Serial.print("-");
        unitTime = (unitTime * 3.0 + ((float)duration / 3.0)) / 4.0;
      }

    } else {
      if (localSeqBuffer.length() > 0) {
        if (((xTaskGetTickCount() - releaseStartTick) * portTICK_PERIOD_MS) > (unitTime * 2.5)) {
          char decodedChar = morseDecode(localSeqBuffer);

          Serial.print(" -> ");
          Serial.println(decodedChar);

          localSeqBuffer = "";

          if (xSemaphoreTake(seqBufferMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            globalSeqBuffer = "";
            globalMessageBuffer = globalMessageBuffer + decodedChar;
            xSemaphoreGive(seqBufferMutex);
          }
        }
      }
      buzFlag = 0;
      ledFlag = 0;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void MainTask(void *pvParameters) {
  bool btn1PrevPressed = false;
  bool btn2PrevPressed = false;
  for (;;) {
    bool btn1NowPressed = btn1.isPressed();
    bool btn2NowPressed = btn2.isPressed();
    if (btn1NowPressed && !btn1PrevPressed) {
      if (currentState == STATE_IDLE) {
        currentState = STATE_NORMAL;
        vTaskResume(commsTaskHandle);
      } else if (currentState == STATE_NORMAL) {
        currentState = STATE_IDLE;
        vTaskSuspend(commsTaskHandle);
        ledFlag = 0;
        buzFlag = 0;
      }
    }

    btn1PrevPressed = btn1NowPressed;
    if (btn2NowPressed && !btn2PrevPressed && currentState == STATE_NORMAL) {
      if (xSemaphoreTake(seqBufferMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        if (globalMessageBuffer.length() > 0) {
          globalMessageBuffer = globalMessageBuffer.substring(0, globalMessageBuffer.length() - 1);
        }
        xSemaphoreGive(seqBufferMutex);
      }
    }
    btn2PrevPressed = btn2NowPressed;
    // switch (currentState) {
    //   case STATE_IDLE:
    //     if (btn1.isPressed()) {
    //       currentState = STATE_NORMAL;

    //       vTaskResume(commsTaskHandle);
    //     }
    //   break;

    //   case STATE_NORMAL:
    //     if (btn1.isPressed()) {
    //       currentState = STATE_IDLE;

    //       vTaskSuspend(commsTaskHandle);

    //       ledFlag = 0;
    //       buzFlag = 0;
    //     }ี
    //   break;

    //   case STATE_PRACTICE:
    //   break;

    //   case STATE_LOG:
    //   break;

    //   case STATE_SETTING:
    //   break;
    // }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void loop() {
}
