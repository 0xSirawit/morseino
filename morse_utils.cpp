#include "morse_utils.h"
#include <esp_mac.h>

const char* ssid = "NINE";
const char* password = "ninenine";
const char* apiUrl = "https://lectures-strategic-merchant-pottery.trycloudflare.com/messages";

const char LETTERS_NUMBERS[36] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
  'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
  'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2', '3', '4',
  '5', '6', '7', '8', '9', '0'
};

uint8_t broadcastAddress[] = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

const String MORSE_CODE[36] = {
  ".-",    // A
  "-...",  // B
  "-.-.",  // C
  "-..",   // D
  ".",     // E
  "..-.",  // F
  "--.",   // G
  "....",  // H
  "..",    // I
  ".---",  // J
  "-.-",   // K
  ".-..",  // L
  "--",    // M
  "-.",    // N
  "---",   // O
  ".--.",  // P
  "--.-",  // Q
  ".-.",   // R
  "...",   // S
  "-",     // T
  "..-",   // U
  "...-",  // V
  ".--",   // W
  "-..-",  // X
  "-.--",  // Y
  "--..",  // Z
  ".----", // 1
  "..---", // 2
  "...--", // 3
  "....-", // 4
  ".....", // 5
  "-....", // 6
  "--...", // 7
  "---..", // 8
  "----.", // 9
  "-----"  // 0
};

char morseDecode(String seq) {
  char result = '?';
  for(int i = 0; i < 36; i++) {
    if (MORSE_CODE[i] == seq) {
      result = LETTERS_NUMBERS[i];
      break;
    }
  }
  return result;
}

void broadcastChar(char c) {
  uint8_t data = (uint8_t)c;

  esp_err_t result = esp_now_send(
    broadcastAddress,
    &data,
    sizeof(data)
  );

  if (result == ESP_OK) {
    Serial.print("Broadcast: ");
    Serial.println(c);
  } else {
    Serial.println("Broadcast failed");
  }
}

String getMacAddress() {
  uint8_t mac[6];
  esp_read_mac(mac, ESP_MAC_WIFI_STA);
  char macStr[18];

  snprintf(
    macStr,
    sizeof(macStr),
    "%02X:%02X:%02X:%02X:%02X:%02X",
    mac[0], mac[1], mac[2],
    mac[3], mac[4], mac[5]
  );
  return String(macStr);
}
