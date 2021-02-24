#include "keymap.h"

#include "usb_hid_keys.h"
#include "usbmoousesim.h"
#include "usbkeysim.h"

#define MOUSE_CODES_FIRST KEY_RO
#define MOUSE_UP          (MOUSE_CODES_FIRST + MOUSEMOVE_UP)
#define MOUSE_DOWN        (MOUSE_CODES_FIRST + MOUSEMOVE_DOWN)
#define MOUSE_LEFT        (MOUSE_CODES_FIRST + MOUSEMOVE_LEFT)
#define MOUSE_RIGHT       (MOUSE_CODES_FIRST + MOUSEMOVE_RIGHT)
#define MOUSE_WHEEL_UP    (MOUSE_CODES_FIRST + MOUSEWHEEL_UP)
#define MOUSE_WHEEL_DOWN  (MOUSE_CODES_FIRST + MOUSEWHEEL_DOWN)
#define MOUSE_BUTTON1     (MOUSE_CODES_FIRST + 6)
#define MOUSE_BUTTON2     (MOUSE_BUTTON1 + 1)
#define MOUSE_BUTTON3     (MOUSE_BUTTON2 + 1)
#define MOUSE_ACCEL_UP    (MOUSE_BUTTON3 + 1)
#define MOUSE_ACCEL_DOWN  (MOUSE_ACCEL_UP + 1)
#define MOUSE_CODES_LAST  MOUSE_ACCEL_DOWN

#define MOD_CODES_FIRST (MOUSE_CODES_LAST + 1)
#define LAYER_MOD       MOD_CODES_FIRST
#define MOUSE_MOD       (LAYER_MOD + 1)
#define MOD_CODES_LAST  MOUSE_MOD

#define LAYERS 4

uint8_t keymaps[LAYERS][ROWS][COLS] = {
  {
    { KEY_1, KEY_LEFTALT, KEY_LEFTCTRL, LAYER_MOD, KEY_SPACE, KEY_LEFTMETA, KEY_Q, KEY_2 },
    { KEY_3, KEY_W, KEY_A, KEY_LEFTSHIFT, KEY_Z, KEY_S, KEY_E, KEY_4 },
    { KEY_5, KEY_R, KEY_D, KEY_X, KEY_C, KEY_F, KEY_T, KEY_6 },
    { KEY_7, KEY_Y, KEY_G, KEY_V, KEY_B, KEY_H, KEY_U, KEY_8 },
    { KEY_9, KEY_I, KEY_J, KEY_n, KEY_M, KEY_k, KEY_O, KEY_0 },
    { KEY_BACKSPACE, KEY_P, KEY_L, KEY_COMMA, KEY_DOT, KEY_SEMICOLON, KEY_CAPSLOCK, KEY_GRAVE },
    { KEY_MINUS, KEY_BACKSLASH, KEY_APOSTROPHE, KEY_SLASH, KEY_ESC, MOUSE_MOD, KEY_RIGHTBRACE, KEY_EQUAL },
    { KEY_DELETE, KEY_RETURN, KEY_RIGHTALT, KEY_LEFTBRACE, KEY_MUTE, KEY_VOLUMEUP, KEY_VOLUMEDOWN, KEY_POWER }
  },
  {
    { KEY_F1, KEY_LEFTALT, KEY_LEFTCTRL, LAYER_MOD, KEY_TAB, KEY_LEFTMETA, 0, KEY_F2 },
    { KEY_F3, 0, 0, KEY_LEFTSHIFT, 0, 0, 0, KEY_F4 },
    { KEY_F5, 0, 0, 0, 0, 0, 0, KEY_F6 },
    { KEY_F7, KEY_HOME, 0, 0, 0, KEY_LEFTARROW, KEY_PAGEDOWN, KEY_F8 },
    { KEY_F9, KEY_PAGEUP, KEY_DOWNARROW, 0, 0, KEY_UPARROW, KEY_END, KEY_F10 },
    { 0, 0, KEY_RIGHTARROW, 0, 0, 0, 0, 0 },
    { KEY_F11, 0, 0, 0, 0, MOUSE_MOD, 0, KEY_F12 },
    { 0, 0, KEY_RIGHTALT, 0, KEY_MUTE, KEY_VOLUMEUP, KEY_VOLUMEDOWN, KEY_POWER }
  },
  {
    { 0, KEY_LEFTALT, KEY_LEFTCTRL, LAYER_MOD, 0, KEY_LEFTMETA, 0, 0 },
    { 0, 0, 0, KEY_LEFTSHIFT, 0, 0, 0, 0 },
    { 0, 0, 0, 0, MOUSE_BUTTON1, 0, 0, 0 },
    { 0, 0, 0, 0, MOUSE_BUTTON2, 0, 0, 0 },
    { 0, 0, 0, 0, MOUSE_BUTTON3, 0, 0, 0 },
    { MOUSE_DOWN, 0, 0, 0, 0, 0, MOUSE_WHEEL_DOWN, MOUSE_UP },
    { MOUSE_LEFT, MOUSE_ACCEL_DOWN, MOUSE_ACCEL_UP, 0, 0, MOUSE_MOD, MOUSE_WHEEL_UP, MOUSE_RIGHT },
    { 0, 0, KEY_RIGHTALT, 0, KEY_MUTE, KEY_VOLUMEUP, KEY_VOLUMEDOWN, KEY_POWER }
  },
  {
    { 0, KEY_LEFTALT, KEY_LEFTCTRL, LAYER_MOD, 0, KEY_LEFTMETA, 0, 0 },
    { 0, 0, 0, KEY_LEFTSHIFT, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, MOUSE_MOD, 0, 0 },
    { 0, 0, KEY_RIGHTALT, 0, KEY_MUTE, KEY_VOLUMEUP, KEY_VOLUMEDOWN, KEY_POWER }
  },
};

static int layer = 0;

static inline bool is_normal_key(uint8_t key) {
  return key < MOUSE_CODES_FIRST || MOD_CODES_LAST < key;
}

static inline bool unnormal_is_mouse_move(uint8_t key) {
  return key < MOUSE_BUTTON1;
}

void press_key(int row, int col) {
  const uint8_t key = keymaps[layer][row][col];

  if (is_normal_key(key)) {
    usbkeysim_press(key);
  } else (unnormal_is_mousemove(key)) {
    mousemove_press(key - MOUSE_CODES_FIRST);
  } else {
    switch (key) {
      case MOUSE_BUTTON1: mousebutton_press(MOUSE_LEFT); break;
      case MOUSE_BUTTON2: mousebutton_press(MOUSE_MIDDLE); break;
      case MOUSE_BUTTON3: mousebutton_press(MOUSE_RIGHT); break;

      case MOUSE_ACCEL_UP: mouse_increase_accel(); break;
      case MOUSE_ACCEL_DOWN: mouse_decrease_accel(); break;

      case LAYER_MOD:
      case MOUSE_MOD:
        usbkeysim_release_nomod();
        mouse_release_all();
        layer |= (1 << (key - LAYER_MOD));
        break;
    }
  }
}

void release_key(int row, int col) {
  const uint8_t key = keymaps[layer][row][col];

  if (is_normal_key(key)) {
    usbkeysim_release(key);
  } else (unnormal_is_mousemove(key)) {
    mousemove_release(key - MOUSE_CODES_FIRST);
  } else {
    switch (key) {
      case MOUSE_BUTTON1: mousebutton_release(MOUSE_LEFT); break;
      case MOUSE_BUTTON2: mousebutton_release(MOUSE_MIDDLE); break;
      case MOUSE_BUTTON3: mousebutton_release(MOUSE_RIGHT); break;

      case MOUSE_ACCEL_UP: break;
      case MOUSE_ACCEL_DOWN: break;

      case LAYER_MOD:
      case MOUSE_MOD:
        usbkeysim_release_nomod();
        mouse_release_all();
        layer &= ~(1 << (key - LAYER_MOD));
        break;
    }
  }
}
