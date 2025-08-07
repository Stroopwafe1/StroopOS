#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "structs.h"

typedef enum {
  PRESSED,
  RELEASED
} Key_State;

typedef struct {
  uint16_t scancode;
  uint8_t flags;
  Key_State state;
} Key_Packet;

#define MOD_SHIFT_LEFT (1 << 0)
#define MOD_SHIFT_RIGHT (1 << 1)
#define MOD_ALT (1 << 2)
#define MOD_CTRL (1 << 3)
#define MOD_SUPER (1 << 4)
#define TOGGLE_CAPS (1 << 5)
#define TOGGLE_NUM (1 << 6)
#define TOGGLE_SCROLL (1 << 7)

extern char kdbus[128];

void keyboard_install();
void keyboard_handler(Reg_State* r);

#endif
