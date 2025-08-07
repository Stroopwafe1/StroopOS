#include "keyboard.h"
#include "tty.h"
#include "irq.h"
#include "kernel_functions.h"

extern uint8_t _inb(uint8_t port);
uint8_t kbd_state = 0;
uint8_t kbd_mods = 0;

void keyboard_install() {
  irq_install_handler(1, keyboard_handler);
}

// modifiers
// 0x2a - left shift
// 0x1d - left control / kdb_state1 right control
// 0x38 - left alt / kbd_state1 right alt+gr
// 0x3a - caps lock
// 0x36 - right shift
// 0x5b - kbd_state1 super
void keyboard_handler(Reg_State* r) {
  uint8_t scancode = _inb(0x60);
  if (scancode == 0xE0) {
	// extension byte, next byte follows
	kbd_state = 1;
	return;
  }

  Key_State state = (scancode & 0x80) ? RELEASED : PRESSED;
  uint16_t keycode = scancode & (~0x80);
  if (kbd_state) {
	keycode = 0xE0 << 8 | (scancode & (~0x80));
	kbd_state = 0;
  }

  switch (scancode) {
  case 0x1d: kbd_mods |= MOD_CTRL; break;
  case 0x2a: kbd_mods |= MOD_SHIFT_LEFT; break;
  case 0x36: kbd_mods |= MOD_SHIFT_RIGHT; break;
  case 0x38: kbd_mods |= MOD_ALT; break;
  case 0x5b: kbd_mods |= MOD_SUPER; break;

  case 0x1d | 0x80: kbd_mods ^= MOD_CTRL; break;
  case 0x2a | 0x80: kbd_mods ^= MOD_SHIFT_LEFT; break;
  case 0x36 | 0x80: kbd_mods ^= MOD_SHIFT_RIGHT; break;
  case 0x38 | 0x80: kbd_mods ^= MOD_ALT; break;
  case 0x5b | 0x80: kbd_mods ^= MOD_SUPER; break;

  case 0x3a | 0x80: kbd_mods ^= TOGGLE_CAPS; break;
  case 0x45 | 0x80: kbd_mods ^= TOGGLE_NUM; break;
  case 0x46 | 0x80: kbd_mods ^= TOGGLE_SCROLL; break;
  }

  Key_Packet key = {
	.scancode = keycode,
	.flags = kbd_mods,
	.state = state
  };

  kHandleKey(key, r);
}
