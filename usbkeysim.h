#ifndef USBKEYSIM_H
#define USBKEYSIM_H

#include "HID.h"

#ifndef _USING_HID
#error "Using legacy HID library!"
#endif

void usbkeysim_init(void);

void usbkeysim_press(uint8_t key);

void usbkeysim_release(uint8_t key);

void usbkeysim_release_nonmod(void);

#endif
