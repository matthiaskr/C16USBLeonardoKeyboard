#include "usbkeysim.h"

#include "usb_hid_keys.h"

#include <stdbool.h>

#ifndef _USING_HID
#error "Using legacy HID library!"
#endif

#define REPORT_COUNT 6

#define REPEATED_REPORTS 3

static const uint8_t hid_descr[] PROGMEM = {
  //  Keyboard
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)  // 47
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x85, 0x02,                    //   REPORT_ID (2)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
   
  0x19, KEY_LEFTCTRL,                    //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, KEY_RIGHTMETA,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    
  0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    
  0x95, REPORT_COUNT,            //   REPORT_COUNT (6)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0xff,                    //   LOGICAL_MAXIMUM (255)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    
  0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0xff,                    //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0xc0,                          // END_COLLECTION
};

static struct {
  uint8_t modifiers;
  uint8_t reserved;
  uint8_t keys[REPORT_COUNT];
} report = {0, 0, {0, 0, 0, 0, 0, 0}};

static int do_send_report = 0;

void usbkeysim_init(void) {
  static HIDSubDescriptor node(hid_descr, sizeof(hid_descr));
  HID().AppendDescriptor(&node);
}

void usbkeysim_send_report(void) {
  if (do_send_report > 0) {
    HID().SendReport(2, &report, sizeof(report));
    do_send_report--;
  }
}

static void send_report(void) {
  do_send_report = REPEATED_REPORTS;
}

static inline bool is_modifier(uint8_t key) {
  return KEY_LEFTCTRL <= key && key <= KEY_RIGHTMETA;
}

static inline void add_modifier(uint8_t key) {
  const uint8_t modmask = 1 << (key - KEY_LEFTCTRL);
  
  if ((report.modifiers & modmask) == 0) {
    report.modifiers |= modmask;
    send_report();
  }
}

static inline void add_key(uint8_t key) {
  int candidate = -1;

  for (int i=0; i<REPORT_COUNT; i++) {
    if (report.keys[i] == key) {
      return;
    }
    if (candidate == -1 && report.keys[i] == 0) {
      candidate = i;
    }
  }

  // we just do not send any more keys (deviating from expected behavior...)
  if (candidate != -1) {
    report.keys[candidate] = key;
    send_report();
  }
}

void usbkeysim_press(uint8_t key) {
  if (is_modifier(key)) {
    add_modifier(key);
  } else {
    add_key(key);
  }
}

static inline void remove_modifier(uint8_t key) {
  const uint8_t modmask = 1 << (key - KEY_LEFTCTRL);
  
  if ((report.modifiers & modmask) != 0) {
    report.modifiers &= ~modmask;
    send_report();
  }
}

static inline void remove_key(uint8_t key) {
  for (int i=0; i<REPORT_COUNT; i++) {
    if (report.keys[i] == key) {
      report.keys[i] = 0;
      send_report();
      return;
    }
  }
}

void usbkeysim_release(uint8_t key) {
  if (is_modifier(key)) {
    remove_modifier(key);
  } else {
    remove_key(key);
  }
}

void usbkeysim_release_nonmod(void) {
  bool should_send = false;

  for (int i=0; i<REPORT_COUNT; i++) {
    should_send = should_send || (report.keys[i] != 0);
    report.keys[i] = 0;
  }

  if (should_send) {
    send_report();
  }
}
