#ifndef IRQ_H
#define IRQ_H

#include <stdint.h>
#include "structs.h"

extern void _irq0();
extern void _irq1();
extern void _irq2();
extern void _irq3();
extern void _irq4();
extern void _irq5();
extern void _irq6();
extern void _irq7();
extern void _irq8();
extern void _irq9();
extern void _irq10();
extern void _irq11();
extern void _irq12();
extern void _irq13();
extern void _irq14();
extern void _irq15();

extern void _outb(uint8_t port, uint8_t value);


extern void* irq_routines[16];
void irq_install_handler(int32_t irq, void (*handler)(Reg_State* r));
void irq_uninstall_handler(int32_t irq);
void irq_remap(void);
void irq_install();
void irq_handler(Reg_State* r);

#endif
