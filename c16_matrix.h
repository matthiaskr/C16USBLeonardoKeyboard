#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>

#define ROWS 8
#define COLS 8
#define KEYS (ROWS * COLS)

extern uint8_t row_pins[ROWS];
extern uint8_t col_pins[COLS];

#endif
