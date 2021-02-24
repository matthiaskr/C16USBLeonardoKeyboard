#ifndef KEYMAP_H
#define KEYMAP_H

#include "c16_matrix.h"

#include <stdbool.h>

void process_key(bool pressed, int row, int col);

void mouse_movement(void);

#endif
