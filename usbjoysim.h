#ifndef USBJOYSIM_H
#define USBJOYSIM_H

#define JOY_BUTTONS 32

void joy_init(void);

void joy_press_button(int button);

void joy_release_button(int button);

void joy_release_all(void);

void joy_send_changes(void);

#endif
