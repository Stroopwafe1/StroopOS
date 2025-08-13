#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "structs.h"
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
MB_MMap mb_mmap;
MB_MMapEntry* mb_mmap_ram;

#define MMU_PD ((uint32_t*)0xFFFFF000)
#define MMU_KERNEL 768
#define MB_INFO_MMAP_TYPE 6
#define MB_INFO_MMAP_RAM_TYPE 1
#define MB_INFO_FB_TYPE 8

extern void _unmap_identity(void);
extern uint32_t _kernel_end;

uint32_t nearest_multiple(uint32_t to_round, uint32_t multiple) {
  return to_round + (multiple - (to_round % multiple)) % multiple;
}

void mmu_map_framebuffer() {
  uint32_t pd_index = mb_fb->address >> 22;
  uint32_t pt_index = mb_fb->address >> 12 & 0x3FF;
  uint32_t end_point = _kernel_end;

  uint32_t page_count = mb_fb->pitch * mb_fb->height / 4096;
  uint32_t* new_page_table = (uint32_t*)nearest_multiple(end_point, 4096);

  // Assign the framebuffer index to a new page table
  MMU_PD[pd_index] = (uint32_t)new_page_table | 0x3;
  //MemSet(new_page_table, 4096, 0);
  for (uint32_t i = 0; i < page_count; i++) {
	uint32_t value = (mb_fb->address + 4096 * i);
	new_page_table[i + pt_index] = value | 0x3;
  }
}

void parse_multiboot_info() {
  uint32_t* tag_start = (uint32_t*)(void*)(multiboot_info+8);
  while (*tag_start) {
	uint32_t tag_size = *(tag_start + 1);
	if (*tag_start == MB_INFO_FB_TYPE) {
	  mb_fb = (mb_framebuffer*)tag_start;
	} else if (*tag_start == MB_INFO_MMAP_TYPE) {
	  mb_mmap = *(MB_MMap*)tag_start;
	  mb_mmap.entries = (MB_MMapEntry*)&mb_mmap.entries;
	  for (uint32_t i = 0; i < mb_mmap.size / mb_mmap.entry_size; i++) {
		if (mb_mmap.entries[i].type == MB_INFO_MMAP_RAM_TYPE) {
		  mb_mmap_ram = &mb_mmap.entries[i];
		}
	  }
	}
	tag_start += nearest_multiple(tag_size, 8) / sizeof(uint32_t);
  }
  return;
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
  parse_multiboot_info();
  mmu_map_framebuffer();
  _unmap_identity();

  //idt_install();
  //isrs_install();
  //irq_install();
  //timer_install();
  //keyboard_install();

  tty_register();
  pong_register();

  kChangeState(TTY);
  
  for(;;) { asm("hlt"); }
}

void kPanic(const char* err) {
  PrintError("[KERNEL PANIC]: ", false);
  PrintError(err, true);
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
