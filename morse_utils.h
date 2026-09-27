#ifndef MORSE_UTILS_H
#define MORSE_UTILS_H

#include <Arduino.h>
#include <esp_now.h>

#define TOTAL_CHARECTERS 36

extern const char LETTERS_NUMBERS[TOTAL_CHARECTERS];
extern const String MORSE_CODE[TOTAL_CHARECTERS];
extern uint8_t broadcastAddress[6];
extern const char* ssid;
extern const char* password;
extern const char* apiUrl;

// ' ' = จบตัวอักษร ให้ฝั่งรับ decode
struct MorseData {
  char character;
  long duration;
};

char morseDecode(String seq);
void broadcastChar(char c);
String getMacAddress(void);

#endif
