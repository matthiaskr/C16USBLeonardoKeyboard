#ifndef INPUT_H
#define INPUT_H

#include "c16_matrix.h"

#include <stdbool.h>

#define UNCHANGED 0
#define PRESSED 1
#define RELEASED 2

extern bool keys_down[KEYS];

void input_setup(void);

int check_pressed_or_released(int row, int col, int input_value);

#endif
