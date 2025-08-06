#ifndef TIMER_H
#define TIMER_H

#include "structs.h"

extern uint32_t ticks;

void timer_handler(Reg_State* r);
void timer_install();

#endif
