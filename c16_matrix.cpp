#include "c16_matrix.h"

#include <Arduino.h>
#include <Keyboard.h>

// row pins on C16:         6,  8, 12, 11, 13,  1, 16, 19
uint8_t row_pins[ROWS] =  {A1,  2,  6,  5,  7, A3, 10, 13};

// col pins on C16:       18, 15, 14,  3,  7,  9, 10, 17
uint8_t col_pins[COLS] = {12,  9,  8, A2, A0,  3,  4, 11};
