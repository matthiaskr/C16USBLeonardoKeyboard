#include "usbjoysim.h"

#include <Joystick.h>

#include <stdbool.h>

Joystick_ Joystick;

static bool has_changes = false;

void joy_init(void) {
    Joystick.begin(false);
}

void joy_send_changes(void) {
    if (has_changes) {
        Joystick.sendState();
        has_changes = true;
    }
}

void joy_press_button(int button) {
    Joystick.pressButton(button);
    has_changes = true;
}

void joy_release_button(int button) {
    Joystick.releaseButton(button);
    has_changes = true;
}

void joy_release_all(void) {
    int i;
    for (i=0; i<JOY_BUTTONS; i++) {
        Joystick.releaseButton(i);
        has_changes = true;
    }
}
