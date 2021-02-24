#include <HID.h>
#include <Keyboard.h>
#include <Mouse.h>

#include "c16_matrix.h"
#include "c16_input.h"
#include "c16_keymap.h"

void setup() {
  Keyboard.begin();
  Mouse.begin();

  input_setup();

  for (int i=0; i<ROWS; i++) {
    pinMode(row_pins[i], INPUT_PULLUP);
  }

  for (int i=0; i<COLS; i++) {
    pinMode(col_pins[i], INPUT_PULLUP);
  }
}

void loop() {
  for (int row=0; row<ROWS; row++) {
    pinMode(row_pins[row], OUTPUT);
    digitalWrite(row_pins[row], LOW);

    for (int col=0; col<COLS; col++) {
      int input_value = digitalRead(col_pins[col]);
      switch (check_pressed_or_released(row, col, input_value)) {
        case UNCHANGED:
          break;

        case PRESSED:
          process_key(true, row, col);
          break;

        case RELEASED:
          process_key(false, row, col);
          break;
      }
    }

    digitalWrite(row_pins[row], HIGH);
    delay(1); // Wait for the pin to get HIGH
    pinMode(row_pins[row], INPUT_PULLUP);
  }

  mouse_movement();
}
