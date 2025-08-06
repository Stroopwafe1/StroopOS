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



size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

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
  ARGB red = {0xFFFF0000};
  if (r->intr_index < 32) {
	DrawString("[ERROR] [Kernel]:", &term_x, &term_y, red, false);
	if (r->intr_index > EXCEPTION_MESSAGES_LENGTH) {
	  DrawString(" Unknown exception ", &term_x, &term_y, red, true);
	} else {
	  DrawString(exception_messages[r->intr_index], &term_x, &term_y, red, true);
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

void kernel_main(void) {
  mb_fb = find_framebuffer();

  idt_install();
  isrs_install();
  irq_install();
  timer_install();
  keyboard_install();

  ARGB yellow = { 0xFFFFFF00 };
  ARGB purple = { 0xFFFF00FF };
  
  DrawString("Hello from kernel", &term_x, &term_y, yellow, true);
  DrawString("From another line as well", &term_x, &term_y, purple, true);
  
  for(;;) { asm("hlt"); }
}
