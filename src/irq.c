#include "irq.h"
#include "idt.h"

void* irq_routines[16] = { 0 };

void irq_install_handler(int32_t irq, void (*handler)(Reg_State* r)) {
  irq_routines[irq] = handler;
}

void irq_uninstall_handler(int32_t irq) {
  irq_routines[irq] = 0;
}

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

/* reinitialize the PIC controllers, giving them specified vector offsets
   rather than 8h and 70h, as configured by default */

#define ICW1_ICW4	0x01		/* Indicates that ICW4 will be present */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

/* Normally, IRQs 0 to 7 are mapped to entries 8 to 15. This
*  is a problem in protected mode, because IDT entry 8 is a
*  Double Fault! Without remapping, every time IRQ0 fires,
*  you get a Double Fault Exception, which is NOT actually
*  what's happening. We send commands to the Programmable
*  Interrupt Controller (PICs - also called the 8259's) in
*  order to make IRQ0 to 15 be remapped to IDT entries 32 to
*  47 */
void irq_remap(void) {
  _outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
  _outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

  _outb(PIC1_DATA, 0x20); // offset
  _outb(PIC2_DATA, 0x28); // offset

  _outb(PIC1_DATA, 0x04); // Tell PIC1 that PIC2 exists at IRQ2
  _outb(PIC2_DATA, 0x02);

  _outb(PIC1_DATA, ICW4_8086);
  _outb(PIC2_DATA, ICW4_8086);
  
  _outb(PIC1_DATA, 0x0);
  _outb(PIC2_DATA, 0x0);
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
