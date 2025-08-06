#include "timer.h"
#include "tty.h"
#include "irq.h"

uint32_t ticks = 0;

void timer_handler(Reg_State* r) {
  ticks++;
  

  if (ticks % 18 == 0) {
	//DrawString("One second has passed", &term_x, &term_y, (ARGB){0xFFFFFFFF}, true);
  }
}

void timer_install() {
  irq_install_handler(0, timer_handler);
}
