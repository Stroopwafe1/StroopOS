#ifndef KERNEL_FUNCTIONS_H
#define KERNEL_FUNCTIONS_H

#include "keyboard.h"

typedef enum {
  TTY,
  GAME_PONG
} kState;

void kChangeState(kState state);
void kHandleKey(Key_Packet key, Reg_State* r);
void kUpdate(uint32_t delta);
void kSetHandler(kState state, void (*handler)(Key_Packet key, Reg_State* ));
void kSetInit(kState state, void (*init_func)(void));
void kSetUpdate(kState state, uint32_t update_freq, void (*update_func)(uint32_t delta_ticks));

#endif
