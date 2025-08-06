#include "irq.h"
#include "idt.h"

void* irq_routines[16] = { 0 };

void irq_install_handler(int32_t irq, void (*handler)(Reg_State* r)) {
  irq_routines[irq] = handler;
}

void irq_uninstall_handler(int32_t irq) {
  irq_routines[irq] = 0;
}


/* Normally, IRQs 0 to 7 are mapped to entries 8 to 15. This
*  is a problem in protected mode, because IDT entry 8 is a
*  Double Fault! Without remapping, every time IRQ0 fires,
*  you get a Double Fault Exception, which is NOT actually
*  what's happening. We send commands to the Programmable
*  Interrupt Controller (PICs - also called the 8259's) in
*  order to make IRQ0 to 15 be remapped to IDT entries 32 to
*  47 */
void irq_remap(void) {
  _outb(0x20, 0x11);
  _outb(0xA0, 0x11);

  _outb(0x21, 0x20);
  _outb(0xA1, 0x28);

  _outb(0x21, 0x04);
  _outb(0xA1, 0x02);

  _outb(0x21, 0x01);
  _outb(0xA1, 0x01);
  
  _outb(0x21, 0x0);
  _outb(0xA1, 0x0);
}

void irq_install() {
  irq_remap();

  idt_set_gate(32, (uint32_t)_irq0, 0x08, IDT_INT_GATE);
  idt_set_gate(33, (uint32_t)_irq1, 0x08, IDT_INT_GATE);
  idt_set_gate(34, (uint32_t)_irq2, 0x08, IDT_INT_GATE);
  idt_set_gate(35, (uint32_t)_irq3, 0x08, IDT_INT_GATE);
  idt_set_gate(36, (uint32_t)_irq4, 0x08, IDT_INT_GATE);
  idt_set_gate(37, (uint32_t)_irq5, 0x08, IDT_INT_GATE);
  idt_set_gate(38, (uint32_t)_irq6, 0x08, IDT_INT_GATE);
  idt_set_gate(39, (uint32_t)_irq7, 0x08, IDT_INT_GATE);
  idt_set_gate(40, (uint32_t)_irq8, 0x08, IDT_INT_GATE);
  idt_set_gate(41, (uint32_t)_irq9, 0x08, IDT_INT_GATE);
  idt_set_gate(42, (uint32_t)_irq10, 0x08, IDT_INT_GATE);
  idt_set_gate(43, (uint32_t)_irq11, 0x08, IDT_INT_GATE);
  idt_set_gate(44, (uint32_t)_irq12, 0x08, IDT_INT_GATE);
  idt_set_gate(45, (uint32_t)_irq13, 0x08, IDT_INT_GATE);
  idt_set_gate(46, (uint32_t)_irq14, 0x08, IDT_INT_GATE);
  idt_set_gate(47, (uint32_t)_irq15, 0x08, IDT_INT_GATE);

  __asm__ __volatile__("sti");
}

void _irq_handler(Reg_State* r);
