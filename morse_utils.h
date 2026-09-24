#ifndef MORSE_UTILS_H
#define MORSE_UTILS_H

#include <Arduino.h>

#define TOTAL_CHARECTERS 36

extern const char LETTERS_NUMBERS[TOTAL_CHARECTERS];
extern const String MORSE_CODE[TOTAL_CHARECTERS];

char morseDecode(String seq);

#endif
