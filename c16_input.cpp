#include "c16_input.h"

#include "Arduino.h"

#define DEBOUNCE_DELAY 50

static unsigned long last_millis[KEYS];
bool keys_down[KEYS];

void input_setup(void) {
  int i;

  for (i=0; i<KEYS; i++) {
    last_millis[i] = 0;
    keys_down[i] = false;
  }
}

int check_pressed_or_released(int row, int col, int input_value) {
  int keypos = col + row * COLS;

  unsigned long now = millis();

  if ((now - last_millis[keypos]) > DEBOUNCE_DELAY) {
    if ((input_value == LOW) && !keys_down[keypos]) {
      last_millis[keypos] = now;
      keys_down[keypos] = true;
      return PRESSED;
    } else if ((input_value == HIGH) && keys_down[keypos]) {
      last_millis[keypos] = now;
      keys_down[keypos] = false;
      return RELEASED;
    }
  }

  return UNCHANGED;
}
