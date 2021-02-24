#include "c16_keymap.h"

#include "c16_input.h"

#include <Arduino.h>
#include <HID.h>
#include <Keyboard.h>
#include <Mouse.h>

#define DEBOUNCE_DELAY 50

#define LAYERS 4

#define NO_KEY 0

#define MOUSE_BUTTON1      256
#define MOUSE_BUTTON2      257
#define MOUSE_BUTTON3      258
#define MOUSE_MOVE_UP      512
#define MOUSE_MOVE_DOWN    513
#define MOUSE_MOVE_LEFT    514
#define MOUSE_MOVE_RIGHT   515
#define MOUSE_WHEEL_UP     768
#define MOUSE_WHEEL_DOWN   769
#define MOUSE_ACCEL_UP     998
#define MOUSE_ACCEL_DOWN   999

#define KEY_LAYER1 1024
#define KEY_LAYER2 1025

uint16_t keymap[LAYERS][KEYS] = {
  {
    '1', KEY_LAYER2, KEY_LEFT_CTRL, KEY_LAYER1, ' ', KEY_LEFT_GUI, 'q', '2',
    '3', 'w', 'a', KEY_LEFT_SHIFT, 'z', 's', 'e', '4',
    '5', 'r', 'd', 'x', 'c', 'f', 't', '6',
    '7', 'y', 'g', 'v', 'b', 'h', 'u', '8',
    '9', 'i', 'j', 'n', 'm', 'k', 'o', '0',
    KEY_DOWN_ARROW, 'p', 'l', ',', '.', ';', '`', KEY_UP_ARROW,
    KEY_LEFT_ARROW, '\\', '\'', '/', KEY_ESC, '=', ']', KEY_RIGHT_ARROW,
    KEY_BACKSPACE, KEY_RETURN, '-', '[', KEY_F1, KEY_F2, KEY_F3, KEY_F4
  },
  {
    '1', KEY_LAYER2, KEY_LEFT_CTRL, KEY_LAYER1, KEY_TAB, KEY_LEFT_ALT, 'q', '2',
    '3', 'w', 'a', KEY_LEFT_SHIFT, 'z', 's', 'e', '4',
    '5', 'r', 'd', 'x', 'c', 'f', 't', '6',
    '7', 'y', 'g', 'v', 'b', 'h', 'u', '8',
    '9', 'i', 'j', 'n', 'm', 'k', 'o', '0',
    KEY_PAGE_DOWN, 'p', 'l', ',', '.', ';', '`', KEY_PAGE_UP,
    KEY_HOME, '\\', '\'', '/', KEY_CAPS_LOCK, '=', ']', KEY_END,
    KEY_DELETE, KEY_RETURN, '-', '[', KEY_F5, KEY_F6, KEY_F7, KEY_F8
  },
  {
    NO_KEY, KEY_LAYER2, KEY_LEFT_CTRL, KEY_LAYER1, NO_KEY, KEY_LEFT_GUI, NO_KEY, NO_KEY,
    NO_KEY, NO_KEY, NO_KEY, KEY_LEFT_SHIFT, NO_KEY, NO_KEY, NO_KEY, NO_KEY,
    NO_KEY, NO_KEY, NO_KEY, NO_KEY, MOUSE_BUTTON1, NO_KEY, NO_KEY, NO_KEY,
    NO_KEY, NO_KEY, NO_KEY, NO_KEY, MOUSE_BUTTON2, NO_KEY, NO_KEY, NO_KEY,
    NO_KEY, NO_KEY, NO_KEY, NO_KEY, MOUSE_BUTTON3, NO_KEY, NO_KEY, NO_KEY,
    MOUSE_MOVE_DOWN, NO_KEY, NO_KEY, NO_KEY, NO_KEY, NO_KEY, MOUSE_WHEEL_DOWN, MOUSE_MOVE_UP,
    MOUSE_MOVE_LEFT, MOUSE_ACCEL_DOWN, MOUSE_ACCEL_UP, NO_KEY, NO_KEY, NO_KEY, MOUSE_WHEEL_UP, MOUSE_MOVE_RIGHT,
    NO_KEY, NO_KEY, NO_KEY, NO_KEY, NO_KEY, NO_KEY, NO_KEY, NO_KEY
  },
  {
    '1', KEY_LAYER2, KEY_LEFT_CTRL, KEY_LAYER1, ' ', KEY_RIGHT_ALT, 'q', '2',
    '3', 'w', 'a', KEY_LEFT_SHIFT, 'z', 's', 'e', '4',
    '5', 'r', 'd', 'x', 'c', 'f', 't', '6',
    '7', 'y', 'g', 'v', 'b', 'h', 'u', '8',
    '9', 'i', 'j', 'n', 'm', 'k', 'o', '0',
    KEY_DOWN_ARROW, 'p', 'l', ',', '.', ';', '`', KEY_UP_ARROW,
    KEY_LEFT_ARROW, '\\', '\'', '/', KEY_ESC, '=', ']', KEY_RIGHT_ARROW,
    KEY_INSERT, KEY_RETURN, '-', '[', KEY_F9, KEY_F10, KEY_F11, KEY_F12
  },
};

extern inline uint16_t key_of(int layer, int row, int col);

static bool mouse_move[6];

static const int mouse_accels[] = {1, 10, 20, 40};
#define MOUSE_ACCELS (sizeof(mouse_accels) / sizeof(int))
static int mouse_accel = 2;

static void release_different(int from_layer, int to_layer);

static int press_key(int layer, int i) {
  uint16_t key = keymap[layer][i];
  int new_layer = layer;

  switch (key) {
    case NO_KEY: break;

    case KEY_LAYER1:
      new_layer = layer | 1;
      release_different(layer, new_layer);
      break;
    case KEY_LAYER2:
      new_layer = layer | 2;
      release_different(layer, new_layer);
      break;

    case MOUSE_MOVE_UP: mouse_move[0] = true; break;
    case MOUSE_MOVE_DOWN: mouse_move[1] = true; break;
    case MOUSE_MOVE_LEFT: mouse_move[2] = true; break;
    case MOUSE_MOVE_RIGHT: mouse_move[3] = true; break;
    case MOUSE_WHEEL_UP: mouse_move[4] = true; break;
    case MOUSE_WHEEL_DOWN: mouse_move[5] = true; break;

    case MOUSE_ACCEL_UP:
    case MOUSE_ACCEL_DOWN:
      break;

    case MOUSE_BUTTON1: Mouse.press(MOUSE_LEFT); break;
    case MOUSE_BUTTON2: Mouse.press(MOUSE_MIDDLE); break;
    case MOUSE_BUTTON3: Mouse.press(MOUSE_RIGHT); break;

    default: Keyboard.press(key); break;
  }

  return new_layer;
}

static int release_key(int layer, int i) {
  uint16_t key = keymap[layer][i];
  int new_layer = layer;

  switch (key) {
    case NO_KEY: break;

    case KEY_LAYER1:
      new_layer = layer & ~1;
      release_different(layer, new_layer);
      break;
    case KEY_LAYER2:
      new_layer = layer & ~2;
      release_different(layer, new_layer);
      break;

    case MOUSE_MOVE_UP: mouse_move[0] = false; break;
    case MOUSE_MOVE_DOWN: mouse_move[1] = false; break;
    case MOUSE_MOVE_LEFT: mouse_move[2] = false; break;
    case MOUSE_MOVE_RIGHT: mouse_move[3] = false; break;
    case MOUSE_WHEEL_UP: mouse_move[4] = false; break;
    case MOUSE_WHEEL_DOWN: mouse_move[5] = false; break;

    case MOUSE_ACCEL_UP: if (mouse_accel < MOUSE_ACCELS - 1) { mouse_accel++; } break;
    case MOUSE_ACCEL_DOWN: if (mouse_accel > 0) { mouse_accel--; } break;

    case MOUSE_BUTTON1: Mouse.release(MOUSE_LEFT); break;
    case MOUSE_BUTTON2: Mouse.release(MOUSE_MIDDLE); break;
    case MOUSE_BUTTON3: Mouse.release(MOUSE_RIGHT); break;

    default: Keyboard.release(key); break;
  }

  return new_layer;
}

static void release_different(int from_layer, int to_layer) {
  for (int i=0; i<KEYS; i++) {
    if (keys_down[i] && keymap[from_layer][i] != keymap[to_layer][i]) {
      release_key(from_layer, i);
    }
  }
}

void process_key(bool pressed, int row, int col) {
  static int layer = 0;

  const int key = row * COLS + col;
  
  if (pressed) {
    layer = press_key(layer, key);
  } else {
    layer = release_key(layer, key);
  }
}

void mouse_movement(void) {
  static unsigned long last_millis = 0;

  const unsigned long now = millis();

  if ((now - last_millis) > DEBOUNCE_DELAY) {
    int up_down = mouse_move[1] - mouse_move[0];
    int left_right = mouse_move[3] - mouse_move[2];
    int wheel = mouse_move[4] - mouse_move[5];

    Mouse.move(left_right * mouse_accels[mouse_accel], up_down * mouse_accels[mouse_accel], wheel);

    last_millis = now;
  }
}
