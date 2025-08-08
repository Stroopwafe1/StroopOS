#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "structs.h"
#include "tty.h"
#include "idt.h"
#include "isrs.h"
#include "irq.h"
#include "timer.h"
#include "keyboard.h"
#include "kernel_functions.h"
#include "graphics.h"
#include "tty.h"
#include "game_pong.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

mb_info* multiboot_info;
mb_framebuffer* mb_fb;

#define MB_FB_INFO_TYPE 8

uint32_t nearest_multiple(uint32_t to_round, uint32_t multiple) {
  return to_round + (multiple - (to_round % multiple)) % multiple;
}

mb_framebuffer* find_framebuffer() {
  uint32_t* tag_start = (uint32_t*)(void*)(multiboot_info+8);
  while (*tag_start) {
	uint32_t tag_size = *(tag_start + 1);
	if (*tag_start == MB_FB_INFO_TYPE) {
	  return (mb_framebuffer*)tag_start;
	} else {
	  tag_start += nearest_multiple(tag_size, 8) / sizeof(uint32_t);
	}
  }
  return NULL;
}

void _fault_handler(Reg_State* r) {
  if (r->intr_index < 32) {
	PrintError("[ERROR] [Kernel]: ", false);
	if (r->intr_index > EXCEPTION_MESSAGES_LENGTH) {
	  PrintError("Unknown Exception ", true);
	} else {
	  PrintError(exception_messages[r->intr_index], true);
	}
	while (true) {} // crude 'halt'
  }
};

void _irq_handler(Reg_State* r) {
  void (*handler)(Reg_State* r);
  //putchar((r->intr_index - 32) + '0', &term_x, &term_y, (ARGB){0xFFFFFFFF}, (ARGB){0x0});
  
  handler = irq_routines[r->intr_index - 32];
  if (handler)
	handler(r);

  if (r->intr_index >= 40) {
	// This came from the slave PIC
	_outb(0xA0, 0x20); // Send EOI
  }

  _outb(0x20, 0x20); // Send EOI to master PIC
  asm("sti");
}

kState current_state = TTY;
void (*kKeyHandlers[KSTATE_COUNT])(Key_Packet key, Reg_State* );
void (*kInitFuncs[KSTATE_COUNT])(void);
void (*kUpdateFuncs[KSTATE_COUNT])(void);
uint32_t update_freqs[KSTATE_COUNT] = { 0 };

void kernel_main(void) {
  mb_fb = find_framebuffer();

  idt_install();
  isrs_install();
  irq_install();
  timer_install();
  keyboard_install();

  tty_register();
  pong_register();

  kChangeState(TTY);
  
  for(;;) { asm("hlt"); }
}

void kChangeState(kState state) {
  current_state = state;
  kInitFuncs[state]();
}

void kHandleKey(Key_Packet key, Reg_State* r) {
  kKeyHandlers[(uint32_t)current_state](key, r);
}

void kSetHandler(kState state, void (*handler)(Key_Packet key, Reg_State* )) {
  kKeyHandlers[(uint32_t)state] = handler;
}

void kSetInit(kState state, void (*init_func)(void)) {
  kInitFuncs[(uint32_t)state] = init_func;
}

void kSetUpdate(kState state, uint32_t update_freq, void (*update_func)(void)) {
  kUpdateFuncs[(uint32_t)state] = update_func;
  update_freqs[state] = update_freq;
}

void kUpdate(uint32_t ticks) {
  if (ticks % update_freqs[current_state] == 0)
	kUpdateFuncs[current_state]();
}
