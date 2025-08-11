#include "idt.h"

IDT_Entry idt[256] = { 0 };
IDT_Ptr _idtp;

void idt_set_gate(uint8_t index, uint32_t base, uint16_t selector, uint8_t flags) {
  idt[index] = (IDT_Entry) {
	.base_low = base & 0xFFFF,
	.selector = selector,
	0,
	.type_attributes = flags,
	.base_high = (base & 0xFFFF0000) >> 16
  };
}

void idt_install() {
  _idtp.limit = (sizeof(IDT_Entry) * 256) - 1;
  _idtp.base = (void*)&idt;

  
  
  _idt_load();
}
