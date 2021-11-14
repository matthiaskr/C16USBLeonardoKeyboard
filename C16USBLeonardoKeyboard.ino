#include <HID.h>
#include <Mouse.h>

#include "matrix.h"
#include "usbkeysim.h"
#include "usbmousesim.h"
#include "usbjoysim.h"

void setup() {
  usbkeysim_init();
  mouse_init();
  joy_init();
  matrix_init();
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
}

void loop() {
  matrix_check();
  usbkeysim_send_report();
  mouse_move_action();
  joy_send_changes();
}
