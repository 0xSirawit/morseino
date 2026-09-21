#include "morseino.h"

volatile SystemState currentState = STATE_IDLE;
volatile SystemState selState = STATE_IDLE;
volatile bool ledFlag = false;
volatile bool buzFlag = false;
volatile uint8_t caesarKey = 0;

String globalSeqBuffer = "";
String globalMessageBuffer = "";
SemaphoreHandle_t seqBufferMutex = NULL;
SemaphoreHandle_t i2cMutex = NULL;

TaskHandle_t commsTaskHandle = NULL;

// Buttons
DebouncedButton btn1(PIN_BT1, BUTTON_PRESSED);
DebouncedButton btn2(PIN_BT2, BUTTON_PRESSED);
DebouncedButton btn3(PIN_BT3, BUTTON_PRESSED);
DebouncedButton btn4(PIN_BT4, BUTTON_PRESSED);

// LCD_ICONS
byte SPEAKER[] = {B00001, B00011, B01111, B01111, B01111, B00011, B00001, B00000};
byte MUTESPEAKER[] = {B00000 ,B10001, B01010, B00100, B01010, B10001, B00000, B00000};
byte UNMUTESPEAKER[] = {B00100, B00010, B10001, B01001, B10001, B00010, B00100, B00000};
byte LOCK[] = {B01110, B10001, B10001, B11111, B11011, B11011, B11111, B00000};
byte UNLOCK[] = {B01110, B10000, B10000, B11111, B11011, B11011, B11111, B00000};

// OLED
volatile int item_selected = 1;
volatile int item_sel_previous = 0;
volatile int item_sel_next = 2;

volatile int help_line = 0;

const SystemState stateLookup[] = {STATE_NORMAL, STATE_PRACTICE, STATE_LOG, STATE_SETTING, STATE_HELP};

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
ESP32Encoder encoder;

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

  pinMode(PIN_SW1, INPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUZ, OUTPUT);

  encoder.attachHalfQuad(PIN_RE_DT, PIN_RE_CLK);
  encoder.setCount(0);

  // Core Tasks
  xTaskCreate(LEDTask, "LED_Task", 2048, NULL, 1, NULL);
  xTaskCreate(BUZTask, "BUZ_Task", 2048, NULL, 1, NULL);
  xTaskCreate(LCDDisplayTask, "LCDDisplay_Task", 2048, NULL, 1, NULL);
  xTaskCreate(OLEDDisplayTask, "OLEDDisplay_Task", 8192, NULL, 1, NULL);
  xTaskCreate(RotaryEncoderTask, "RotaryEncoder_Task", 2048, NULL, 1, NULL);
  xTaskCreate(MainTask, "Main_Task", 2048, NULL, 1, NULL);
  xTaskCreate(DebugTask, "Debug_Task", 2048, NULL, 1, NULL);

  // Suspendable Tasks
  xTaskCreate(CommsTask, "Comms_Task", 2048, NULL, 1, &commsTaskHandle);
  vTaskSuspend(commsTaskHandle);
}

void RotaryEncoderTask(void *pvParameters) {
  long position;
  int NUM_OP = NUM_ITEMS*2;

  for (;;) {
    int64_t raw_position = encoder.getCount();
    switch (currentState) {
      case STATE_IDLE:
        position = (long)(raw_position % NUM_OP);
        if (position < 0) {
          position += NUM_OP;
        }

        item_selected = position / 2;
        item_sel_previous = (item_selected + (NUM_ITEMS - 1)) % NUM_ITEMS;
        item_sel_next     = (item_selected + 1) % NUM_ITEMS;
        selState = stateLookup[item_selected];
        break;

      case STATE_NORMAL:
        position = (long)(raw_position % 72);
        if (position < 0) {
          position += 72;
        }

        caesarKey = position / 2;
        break;

      case STATE_PRACTICE:
        break;

      case STATE_LOG:
        break;

      case STATE_SETTING:
        break;

      case STATE_HELP:
        position = (long)(raw_position % ((NUM_HELP_LINES - 2) * 2));
        if (position < 0) {
          position += (NUM_HELP_LINES - 2) * 2;
        }

        help_line = position / 2;
        break;
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void DebugTask(void *pvParameters) {
  for (;;) {
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
    if (buzFlag && !lastBuzState && digitalRead(PIN_SW1)) {
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
  for (;;) {
    u8g2.firstPage();
    do {
      if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        switch (currentState) {
          case STATE_IDLE:
            u8g2.drawBitmap(0, 22, 128/8, 21, bitmap_item_sel_outline);
            u8g2.setFont(u8g_font_7x14);
            u8g2.drawStr(25, 15, menu_items[item_sel_previous]);
            u8g2.drawBitmap(4, 2, 16/8, 16, bitmap_icons[item_sel_previous]);

            u8g2.setFont(u8g_font_7x14B);
            u8g2.drawStr(25, 15+20+2, menu_items[item_selected]);
            u8g2.drawBitmap(4, 24, 16/8, 16, bitmap_icons[item_selected]);

            u8g2.setFont(u8g_font_7x14);
            u8g2.drawStr(25, 15+20+20+2+2, menu_items[item_sel_next]);
            u8g2.drawBitmap(4, 46, 16/8, 16, bitmap_icons[item_sel_next]);

            u8g2.drawBitmap(128-8, 0, 8/8, 64, bitmap_scrollbar_background);
            u8g2.drawBox(125, 64/NUM_ITEMS * item_selected, 3, 64/NUM_ITEMS);
            break;
          case STATE_HELP:
            u8g2.setFont(u8g_font_7x14B);
            u8g2.drawStr(25, 15, "HELP");
            u8g2.drawBitmap(4, 2, 16/8, 16, bitmap_icon_help);
            u8g2.drawBitmap(128-8, 0, 8/8, 64, bitmap_scrollbar_background);
            u8g2.drawBox(125, 64/(NUM_HELP_LINES - 2) * help_line, 3, 64/(NUM_HELP_LINES - 2));
            
            u8g2.setFont(u8g_font_6x12);
            for (int i = 0; i < 3; i++) {
              u8g2.drawStr(4, 30 + (15 * i), morsecode_cs[i+help_line]);
            }
            break;
        }
        xSemaphoreGive(i2cMutex);
      }
    } while (u8g2.nextPage());
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
  lcd.createChar(4, UNMUTESPEAKER);
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
          lcd.print("--- MORSEINO ---");

          lcd.setCursor(0, 1);
          lcd.print("NAME: STATION101");
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

          lcd.setCursor(15, 0);
          if (digitalRead(PIN_SW1)){
            lcd.write(byte(4));
          } else {
            lcd.write(byte(1));
          }

          if ((xTaskGetTickCount() - blinkTime) >= pdMS_TO_TICKS(500)) {
            blinkState ^= 1;
            blinkTime = xTaskGetTickCount();
            lcd.setCursor(0, 0);
            lcd.print(blinkState ? ">" : " ");
          }

          if (xSemaphoreTake(seqBufferMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            displayBuffer = globalSeqBuffer;
            messageDisplay = globalMessageBuffer;
            xSemaphoreGive(seqBufferMutex);
          }

          if (displayBuffer.length() > 5) {
            displayBuffer = displayBuffer.substring(0, 5);
          }

          if (messageDisplay.length() > 16) {
            messageDisplay = messageDisplay.substring(messageDisplay.length() - 16);
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
          break;

        case STATE_PRACTICE:
        case STATE_LOG:
        case STATE_SETTING:
        case STATE_HELP:
          lcd.setCursor(0, 0);
          lcd.print("SW1:SEL SW2:BACK");
          lcd.setCursor(0, 1);
          lcd.print("SW3:SAVELOG");
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
  int saved_item_selected = 0;

  for (;;) {
    switch (currentState) {
      case STATE_IDLE:
        if (btn1.isPressed()) {
          saved_item_selected = item_selected;
          if (selState == STATE_NORMAL) {
            vTaskResume(commsTaskHandle);
          }
          currentState = selState;
          encoder.clearCount();
        }
        break;

      case STATE_NORMAL:
        if (btn2.isPressed()) {
          currentState = STATE_IDLE;
          encoder.setCount(saved_item_selected * 2);
          ledFlag = 0;
          buzFlag = 0;
          vTaskSuspend(commsTaskHandle);
        }
        break;

      case STATE_PRACTICE:
      case STATE_LOG:
      case STATE_SETTING:
        break;
      case STATE_HELP:
        if (btn2.isPressed()) {
          currentState = STATE_IDLE;
          encoder.setCount(saved_item_selected * 2);
        }
        break;
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
void loop() {
}