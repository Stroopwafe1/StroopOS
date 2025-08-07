#include "keyboard.h"
#include "tty.h"
#include "irq.h"

unsigned char kbdus[128] =
  {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
	'9', '0', '-', '=', '\b',	/* Backspace */
	'\t',			/* Tab */
	'q', 'w', 'e', 'r',	/* 19 */
	't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
	'\'', '`',   0,		/* Left shift */
	'\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
	'm', ',', '.', '/',   0,				/* Right shift */
	'*',
    0,	/* Alt */
	' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
	'-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
	'+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
  };


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

  HandleKey(key, r);
}
