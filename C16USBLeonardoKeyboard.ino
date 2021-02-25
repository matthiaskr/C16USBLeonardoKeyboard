#include <HID.h>
#include <Mouse.h>

#include "matrix.h"
#include "usbkeysim.h"
#include "usbmousesim.h"

void setup() {
  usbkeysim_init();
  mouse_init();
  matrix_init();
}

void loop() {
  matrix_check();
  mouse_move_action();
}
