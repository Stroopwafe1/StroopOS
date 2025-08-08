#include "timer.h"
#include "irq.h"
#include "kernel_functions.h"

uint32_t ticks = 0;


void timer_handler(Reg_State* r) {
  (void)r; // Ununsed
  // ticks % 18 is one second basically
  kUpdate(ticks++);
}

void timer_install() {
  irq_install_handler(0, timer_handler);
}
