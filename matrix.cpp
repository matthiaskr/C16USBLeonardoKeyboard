#include "matrix.h"

#include "keymap.h"

#include <Arduino.h>

#define DEBOUNCE_DELAY 15

// row pins on C16:               6,  8, 12, 11, 13,  1, 16, 19
static uint8_t row_pins[ROWS] = {A2, A0,  5,  4,  6, A4,  9, 12};

// col pins on C16:              18, 15, 14,  3,  7,  9, 10, 17
static uint8_t col_pins[COLS] = {11,  8,  7, A3, A1,  2,  3, 10};

static bool keys_down[ROWS][COLS];
static unsigned long last_change_millis[ROWS][COLS];

void matrix_init(void) {
  for (int i=0; i<ROWS; i++) {
    for (int j=0; j<COLS; j++) {
      keys_down[i][j] = false;
      last_change_millis[i][j] = 0;
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
  if ((input_value == LOW) && !keys_down[row][col]) {
    if (last_change_millis[row][col] == 0) {
      last_change_millis[row][col] = now;
    } else if (now - last_change_millis[row][col] > DEBOUNCE_DELAY) {
      keys_down[row][col] = true;
      press_key(row, col);
      last_change_millis[row][col] = 0;
    }
  } else if ((input_value == HIGH) && keys_down[row][col]) {
    if (last_change_millis[row][col] == 0) {
      last_change_millis[row][col] = now;
    } else if (now - last_change_millis[row][col] > DEBOUNCE_DELAY) {
      keys_down[row][col] = false;
      release_key(row, col);
      last_change_millis[row][col] = 0;
    }
  }
}

void matrix_check(void) {
  const unsigned long now = millis();

  for (int row=0; row<ROWS; row++) {
    pinMode(row_pins[row], OUTPUT);
    digitalWrite(row_pins[row], LOW);

    for (int col=0; col<COLS; col++) {
      const int input_value = digitalRead(col_pins[col]);
      check_pressed_or_released(now, row, col, input_value);
    }

    digitalWrite(row_pins[row], HIGH);
    delay(1); // Wait for the pin to get HIGH
    pinMode(row_pins[row], INPUT_PULLUP);
  }
}
