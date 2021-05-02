#include "usbmousesim.h"

#include <HID.h>
#include <Mouse.h>

#define DEBOUNCE_DELAY 25

static bool mouse_move[6] = {false, false, false, false, false, false};
#define MOUSE_MOVE_NOS (sizeof(mouse_move) / sizeof(bool))

static const int mouse_accels[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 5, 5, 5, 5, 5, 5, 10, 10, 10, 10, 15, 15, 15, 20};
#define MOUSE_ACCELS (sizeof(mouse_accels) / sizeof(int))
static int cur_accel = 0;

void mousemove_press(int kind) {
  mouse_move[kind] = true;
}

void mousemove_release(int kind) {
  mouse_move[kind] = false;
}

void mousebutton_press(int button) {
  Mouse.press(button);
}

void mousebutton_release(int button) {
  Mouse.release(button);
}

static void opt_release_button(int button) {
  if (Mouse.isPressed(button)) {
    Mouse.release(button);
  }
}

void mouse_release_all(void) {
  for (int i=0; i<6; i++) {
    mouse_move[i] = false;
  }

  opt_release_button(MOUSE_LEFT);
  opt_release_button(MOUSE_MIDDLE);
  opt_release_button(MOUSE_RIGHT);
}

void mouse_move_action(void) {
  static unsigned long last_millis = 0;

  const unsigned long now = millis();

  if ((now - last_millis) > DEBOUNCE_DELAY) {
    bool should_move = false;
    for (int i=0; !should_move && i<6; i++) {
      should_move = mouse_move[i];
    }
  
    if (should_move) {
      int up_down = mouse_move[MOUSEMOVE_DOWN] - mouse_move[MOUSEMOVE_UP];
      int left_right = mouse_move[MOUSEMOVE_RIGHT] - mouse_move[MOUSEMOVE_LEFT];
      int wheel = mouse_move[MOUSEWHEEL_UP] - mouse_move[MOUSEWHEEL_DOWN];

      const int accel = mouse_accels[cur_accel];

      Mouse.move(left_right * accel, up_down * accel, wheel);

      if (cur_accel < MOUSE_ACCELS - 1) {
        cur_accel++;
      }
    } else {
      cur_accel = 0;
    }
  
    last_millis = now;
  }
}

void mouse_init(void) {
  Mouse.begin();
}
