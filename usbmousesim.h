#ifndef USBMOUSESIM_H
#define USBMOUSESIM_H

#include <stdbool.h>

#define MOUSEMOVE_UP    0
#define MOUSEMOVE_DOWN  1
#define MOUSEMOVE_LEFT  2
#define MOUSEMOVE_RIGHT 3
#define MOUSEWHEEL_UP   4
#define MOUSEWHEEL_DOWN 5

void mouse_init(void);

void mousemove_press(int kind);

void mousemove_release(int kind);

void mousebutton_press(int button);

void mousebutton_release(int button);

void mouse_release_all(void);

void mouse_move_action(void);

#endif
