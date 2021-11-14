#include "keymap.h"

#include "usb_hid_keys.h"
#include "usbmousesim.h"
#include "usbkeysim.h"
#include "usbjoysim.h"
#include "matrix.h"

#include <Mouse.h>

#define MOUSE_CODES_FIRST KEY_RO
#define MOUSE_KEY_UP          (MOUSE_CODES_FIRST + MOUSEMOVE_UP)
#define MOUSE_KEY_DOWN        (MOUSE_CODES_FIRST + MOUSEMOVE_DOWN)
#define MOUSE_KEY_LEFT        (MOUSE_CODES_FIRST + MOUSEMOVE_LEFT)
#define MOUSE_KEY_RIGHT       (MOUSE_CODES_FIRST + MOUSEMOVE_RIGHT)
#define MOUSE_WHEEL_UP    (MOUSE_CODES_FIRST + MOUSEWHEEL_UP)
#define MOUSE_WHEEL_DOWN  (MOUSE_CODES_FIRST + MOUSEWHEEL_DOWN)
#define MOUSE_BUTTON1     (MOUSE_CODES_FIRST + 6)
#define MOUSE_BUTTON2     (MOUSE_BUTTON1 + 1)
#define MOUSE_BUTTON3     (MOUSE_BUTTON2 + 1)
#define MOUSE_CODES_LAST  MOUSE_BUTTON3

#define JOY_CODES_FIRST (MOUSE_CODES_LAST + 1)
#define JOY_BUTTON(N)   (JOY_CODES_FIRST + N - 1)
#define JOY_CODES_LAST  (JOY_CODES_FIRST + JOY_BUTTONS - 1)

#define MOD_CODES_FIRST (JOY_CODES_LAST + 1)
#define LAYER_MOD       MOD_CODES_FIRST
#define MOUSE_MOD       (LAYER_MOD + 1)
#define JOY_SWITCH_MOD  (MOUSE_MOD + 1)
#define MOD_CODES_LAST  JOY_SWITCH_MOD

#define LAYERS 4

uint8_t keymaps[LAYERS][ROWS][COLS] = {
  {
    { KEY_1, KEY_LEFTALT, KEY_LEFTCTRL, LAYER_MOD, KEY_SPACE, KEY_LEFTMETA, KEY_Q, KEY_2 },
    { KEY_3, KEY_W, KEY_A, KEY_LEFTSHIFT, KEY_Z, KEY_S, KEY_E, KEY_4 },
    { KEY_5, KEY_R, KEY_D, KEY_X, KEY_C, KEY_F, KEY_T, KEY_6 },
    { KEY_7, KEY_Y, KEY_G, KEY_V, KEY_B, KEY_H, KEY_U, KEY_8 },
    { KEY_9, KEY_I, KEY_J, KEY_N, KEY_M, KEY_K, KEY_O, KEY_0 },
    { KEY_RIGHTALT, KEY_P, KEY_L, KEY_COMMA, KEY_DOT, KEY_SEMICOLON, KEY_TAB, KEY_GRAVE },
    { KEY_MINUS, KEY_BACKSLASH, KEY_APOSTROPHE, KEY_SLASH, KEY_ESC, MOUSE_MOD, KEY_RIGHTBRACE, KEY_EQUAL },
    { KEY_BACKSPACE, KEY_ENTER, JOY_SWITCH_MOD, KEY_LEFTBRACE, KEY_PROPS, KEY_OPEN, KEY_STOP, KEY_HELP }
  },
  {
    { KEY_F1, KEY_LEFTALT, KEY_LEFTCTRL, LAYER_MOD, 0, KEY_LEFTMETA, KEY_F11, KEY_F2 },
    { KEY_F3, KEY_F12, 0, KEY_LEFTSHIFT, 0, 0, 0, KEY_F4 },
    { KEY_F5, 0, 0, 0, 0, 0, 0, KEY_F6 },
    { KEY_F7, KEY_HOME, 0, 0, 0, KEY_LEFT, KEY_PAGEDOWN, KEY_F8 },
    { KEY_F9, KEY_PAGEUP, KEY_DOWN, 0, 0, KEY_UP, KEY_END, KEY_F10 },
    { KEY_DOWN, 0, KEY_RIGHT, 0, 0, 0, 0, KEY_UP },
    { KEY_LEFT, 0, 0, 0, 0, 0, 0, KEY_RIGHT },
    { KEY_DELETE, 0, KEY_RIGHTALT, 0, 0, 0, 0, 0 }
  },
  {
    { 0, KEY_LEFTALT, KEY_LEFTCTRL, 0, 0, KEY_LEFTMETA, 0, 0 },
    { 0, 0, 0, KEY_LEFTSHIFT, 0, 0, 0, 0 },
    { 0, 0, 0, 0, MOUSE_BUTTON1, 0, 0, 0 },
    { 0, 0, 0, 0, MOUSE_BUTTON2, MOUSE_KEY_LEFT, 0, 0 },
    { 0, 0, MOUSE_KEY_DOWN, 0, MOUSE_BUTTON3, MOUSE_KEY_UP, 0, 0 },
    { MOUSE_KEY_DOWN, 0, MOUSE_KEY_RIGHT, 0, 0, 0, MOUSE_WHEEL_DOWN, MOUSE_KEY_UP },
    { MOUSE_KEY_LEFT, 0, 0, 0, 0, MOUSE_MOD, MOUSE_WHEEL_UP, MOUSE_KEY_RIGHT },
    { 0, 0, KEY_RIGHTALT, 0, MOUSE_BUTTON1, MOUSE_BUTTON2, MOUSE_BUTTON3, 0 }
  },
  {
    { JOY_BUTTON(5), 0, 0, 0, 0, 0, 0, JOY_BUTTON(6) },
    { JOY_BUTTON(7), JOY_BUTTON(16), JOY_BUTTON(17), 0, 0, JOY_BUTTON(15), 0, JOY_BUTTON(8) },
    { JOY_BUTTON(9), 0, JOY_BUTTON(18), 0, 0, 0, 0, JOY_BUTTON(10) },
    { JOY_BUTTON(11), 0, 0, 0, 0, 0, 0, JOY_BUTTON(12) },
    { JOY_BUTTON(13), 0, 0, 0, 0, 0, 0, JOY_BUTTON(14) },
    { JOY_BUTTON(15), 0, 0, 0, 0, 0, 0, JOY_BUTTON(16) },
    { JOY_BUTTON(17), 0, 0, 0, 0, 0, 0, JOY_BUTTON(18) },
    { 0, 0, JOY_SWITCH_MOD, 0, JOY_BUTTON(1), JOY_BUTTON(2), JOY_BUTTON(3), JOY_BUTTON(4) }
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
  } else if (unnormal_is_mouse_move(key)) {
    mousemove_press(key - MOUSE_CODES_FIRST);
  } else {
    switch (key) {
      case MOUSE_BUTTON1: mousebutton_press(MOUSE_KEY_LEFT); break;
      case MOUSE_BUTTON2: mousebutton_press(MOUSE_MIDDLE); break;
      case MOUSE_BUTTON3: mousebutton_press(MOUSE_KEY_RIGHT); break;

      case LAYER_MOD:
      case MOUSE_MOD:
        usbkeysim_release_nonmod();
        mouse_release_all();
        joy_release_all();
        layer = key - LAYER_MOD + 1;
        break;

      case JOY_SWITCH_MOD:
        usbkeysim_release_all();
        mouse_release_all();
        joy_release_all();
        if (layer == 3) {
            layer = 0;
            digitalWrite(13, HIGH);
        } else {
            layer = 3;
            digitalWrite(13, LOW);
        }
        break;

      default:
        joy_press_button(key - JOY_CODES_FIRST);
        break;
    }
  }
}

void release_key(int row, int col) {
  const uint8_t key = keymaps[layer][row][col];

  if (is_normal_key(key)) {
    usbkeysim_release(key);
  } else if (unnormal_is_mouse_move(key)) {
    mousemove_release(key - MOUSE_CODES_FIRST);
  } else {
    switch (key) {
      case MOUSE_BUTTON1: mousebutton_release(MOUSE_LEFT); break;
      case MOUSE_BUTTON2: mousebutton_release(MOUSE_MIDDLE); break;
      case MOUSE_BUTTON3: mousebutton_release(MOUSE_RIGHT); break;

      case LAYER_MOD:
      case MOUSE_MOD:
        usbkeysim_release_nonmod();
        mouse_release_all();
        layer = 0;
        break;

      case JOY_SWITCH_MOD:
        break;

      default:
        joy_release_button(key - JOY_CODES_FIRST);
        break;
    }
  }
}
