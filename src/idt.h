#ifndef IDT_H
#define IDT_H

#include <stdint.h>

typedef struct {
  uint16_t base_low;
  uint16_t selector;
  uint8_t zero;
  uint8_t type_attributes;
  uint16_t base_high;
} __attribute__((packed)) IDT_Entry;

typedef struct {
  uint16_t limit;
  void* base;
} __attribute__((packed)) IDT_Ptr;

#define IDT_INT_GATE 0x8E
#define IDT_TRAP_GATE 0x8F
#define IDT_TASK_GATE 0x85

#define IDT_GATE_TYPE_TASK16 0x5
#define IDT_GATE_TYPE_INT16 0x6
#define IDT_GATE_TYPE_TRAP16 0x7
#define IDT_GATE_TYPE_INT32 0xE
#define IDT_GATE_TYPE_TRAP32 0xE

#define IDT_FLAGS(GATE_TYPE, PRIVILEGE) ((0x8 | (PRIVILEGE) << 5 | (GATE_TYPE)))

extern IDT_Entry idt[256];
extern IDT_Ptr _idtp;

extern void _idt_load();

void idt_set_gate(uint8_t index, uint32_t base, uint16_t selector, uint8_t flags);
void idt_install();

#endif
