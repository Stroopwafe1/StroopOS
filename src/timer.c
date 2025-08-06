#include "timer.h"
#include "tty.h"
#include "irq.h"

uint32_t ticks = 0;
bool caret_shown = true;

void timer_handler(Reg_State* r) {
  ticks++;
  
  // ticks % 18 is one second basically
  if (ticks % 6 == 0) {
	caret_shown = !caret_shown;
	ARGB colour = caret_shown ? (ARGB){0xFFFFFFFF} : (ARGB){0x0};
	DrawCaret(colour);
  }
}

void timer_install() {
  irq_install_handler(0, timer_handler);
}
