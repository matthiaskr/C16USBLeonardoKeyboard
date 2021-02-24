#include "matrix.h"

#include "keymap.h"

#include <Arduino.h>

// row pins on C16:               6,  8, 12, 11, 13,  1, 16, 19
static uint8_t row_pins[ROWS] = {A1,  2,  6,  5,  7, A3, 10, 13};

// col pins on C16:              18, 15, 14,  3,  7,  9, 10, 17
static uint8_t col_pins[COLS] = {12,  9,  8, A2, A0,  3,  4, 11};

static unsigned long last_millis[ROWS][COLS];

static bool keys_down[ROWS][COLS];

void matrix_init(void) {
  for (int i=0; i<ROWS; i++) {
    for (int j=0; j<COLS; j++) {
      last_millis[i][j] = 0;
      keys_down[i][j] = false;
    }
  }

  for (int i=0; i<ROWS; i++) {
    pinMode(row_pins[i], INPUT_PULLUP);
  }

  for (int i=0; i<COLS; i++) {
    pinMode(col_pins[i], INPUT_PULLUP);
  }
}

static void check_pressed_or_released(unsigned long now, int row, int col, int input_value) {
  if ((now - last_millis[row][col]) > DEBOUNCE_DELAY) {
    if ((input_value == LOW) && !keys_down[row][col]) {
      last_millis[row][col] = now;
      keys_down[row][col] = true;
      press_key(row, col);
    } else if ((input_value == HIGH) && keys_down[row][col]) {
      last_millis[row][col] = now;
      keys_down[row][col] = false;
      release_key(row, col);
    }
  }
}

void matrix_check(void) {
  for (int row=0; row<ROWS; row++) {
    pinMode(row_pins[row], OUTPUT);
    digitalWrite(row_pins[row], LOW);

    unsigned long now = millis();

    for (int col=0; col<COLS; col++) {
      const int input_value = digitalRead(col_pins[col]);
      check_pressed_or_released(now, row, col, input_value);
    }

    digitalWrite(row_pins[row], HIGH);
    delay(1); // Wait for the pin to get HIGH
    pinMode(row_pins[row], INPUT_PULLUP);
  }
}
