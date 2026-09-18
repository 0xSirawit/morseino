#ifndef MORSE_UTILS_H
#define MORSE_UTILS_H

#include <Arduino.h>

extern const char LETTERS_NUMBERS[36];
extern const String MORSE_CODE[36];

char morseDecode(String seq);

#endif
